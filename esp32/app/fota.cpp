#include "esp32.h"
#include <string>
#include <esp_ota_ops.h>
#include <esp_flash.h>
#include <miniz.h>
#include <spi_flash_mmap.h>
#include "https.h"
#include "fota.h"

#define TAG __FILE_NAME__

struct fota_context
{
    int offset;
    int size;
    void* temp = nullptr;
    esp_ota_handle_t handle = 0;
    string header;
    tinfl_decompressor* decompressor = nullptr;
    uint8_t* decompressor_dictionary = nullptr;
    int decompressor_offset = 0;
    int decompressor_finish = 0;
};

static void fota_recv(void* arg, char* data, int length)
{
    void** other_context = (void**)arg;
    struct fota_context* context = (struct fota_context*)*other_context;

    if (context == NULL)
    {
        *other_context = context = new fota_context;

        context->offset = 0;
        context->size = 0;
        context->temp = realloc(context->temp, 1536);
    }

    if (context->size == 0)
    {
        context->header += string(data, length);
        size_t pos = context->header.find("\r\n\r\n");
        if (pos == string::npos)
            return;
        int skip = length - (context->header.length() - (pos + 4));
        data += skip;
        length -= skip;

        // Content-Length:
        pos = context->header.find("Content-Length: ");
        if (pos != string::npos)
            context->size = strtol(context->header.data() + pos + sizeof("Content-Length: ") - 1, 0, 10);
        if (context->size == 0)
        {
            https_disconnect(arg);
            return;
        }
        context->header = string();
        if (length == 0)
            return;
    }

    if (context->offset == 0 && context->handle == 0)
    {
        if (memcmp("ESP32C3", data, 7) == 0)
        {
            memcpy(&context->size, data + 7, 3);
            context->decompressor = (tinfl_decompressor*)realloc(context->decompressor, sizeof(tinfl_decompressor));
            context->decompressor_dictionary = (uint8_t*)realloc(context->decompressor_dictionary, TINFL_LZ_DICT_SIZE);
            context->decompressor_offset = 0;
            context->decompressor_finish = 0;
            tinfl_init(context->decompressor);
        }
        esp_ota_begin(esp_ota_get_next_update_partition(nullptr), context->size, &context->handle);
    }
    if (context->decompressor)
    {
        size_t avail_in = context->offset ? length : length - 10;
        uint8_t* next_in = context->offset ? (uint8_t*)data : (uint8_t*)data + 10;
        while (context->decompressor_finish == 0)
        {
            uint8_t* next_out = context->decompressor_dictionary + context->decompressor_offset;

            size_t in_bytes = avail_in;
            size_t out_bytes = TINFL_LZ_DICT_SIZE - context->decompressor_offset;
            tinfl_status status = tinfl_decompress(context->decompressor, next_in, &in_bytes, context->decompressor_dictionary, next_out, &out_bytes, TINFL_FLAG_HAS_MORE_INPUT);
            context->decompressor_offset = (context->decompressor_offset + out_bytes) & (TINFL_LZ_DICT_SIZE - 1);

            if (status >= TINFL_STATUS_DONE)
            {
                esp_ota_write_with_offset(context->handle, next_out, out_bytes, context->offset);
                context->offset += out_bytes;
                context->decompressor_finish = status == TINFL_STATUS_DONE ? 1 : 0;
            }

            avail_in -= in_bytes;
            next_in += in_bytes;

            if (status == TINFL_STATUS_DONE || avail_in == 0)
            {
                break;
            }
        }
    }
    else
    {
        esp_ota_write_with_offset(context->handle, data, length, context->offset);
        context->offset += length;
    }
    ESP_LOGI(TAG, "%d/%d", context->offset, context->size);
    if (context->offset == context->size)
    {
        esp_ota_end(context->handle);
        context->handle = 0;

        esp_ota_set_boot_partition(esp_ota_get_next_update_partition(nullptr));

        vTaskDelay(1000 / portTICK_PERIOD_MS);
        esp_restart();
    }
}

static void fota_disconn(void* arg)
{
    void** other_context = (void**)arg;
    struct fota_context* context = (struct fota_context*)*other_context;

    delete context;
}

void fota(const char* url)
{
    https_connect(url, nullptr, fota_recv, fota_disconn);
}

void fota_callback(void* arg)
{
    https_callback(arg, fota_recv, fota_disconn);
}
