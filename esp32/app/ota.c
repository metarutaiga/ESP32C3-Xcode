#include "esp32.h"
#include <sys/socket.h>
#include <esp_ota_ops.h>
#include <esp_flash.h>
#include <miniz.h>
#include <spi_flash_mmap.h>
#include "ota.h"

#define TAG __FILE_NAME__

struct ota_context
{
    int udp_socket;
    int tcp_socket;
    int offset;
    int size;
    void* temp;
    esp_ota_handle_t handle;
    tinfl_decompressor* decompressor;
    void* decompressor_dictionary;
    int decompressor_offset;
    int decompressor_finish;
};
static struct ota_context* context IRAM_BSS_ATTR;

static void ota_handler(TimerHandle_t timer)
{
    if (context->tcp_socket >= 0)
    {
        char* data = context->temp;
        int length = lwip_recv(context->tcp_socket, data, 1536, 0);
        if (length < 0)
        {
            return;
        }
        if (context->offset == 0 && context->handle == 0)
        {
            if (memcmp("ESP32C3", data, 7) == 0)
            {
                memcpy(&context->size, data + 7, 3);
                context->decompressor = realloc(context->decompressor, sizeof(tinfl_decompressor));
                context->decompressor_dictionary = realloc(context->decompressor_dictionary, TINFL_LZ_DICT_SIZE);
                context->decompressor_offset = 0;
                context->decompressor_finish = 0;
                tinfl_init(context->decompressor);
            }
            esp_ota_begin(esp_ota_get_next_update_partition(NULL), context->size, &context->handle);
        }
        if (context->decompressor)
        {
            size_t avail_in = context->offset ? length : length - 10;
            void* next_in = context->offset ? data : data + 10;
            while (context->decompressor_finish == 0)
            {
                void* next_out = context->decompressor_dictionary + context->decompressor_offset;

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
        if (length > 0 && length < 1536)
        {
            lwip_send(context->tcp_socket, "OK", 2, 0);
        }
        if (length == 0 || context->offset == context->size)
        {
            if (context->offset == context->size)
            {
                esp_ota_end(context->handle);
                context->handle = 0;

                esp_ota_set_boot_partition(esp_ota_get_next_update_partition(NULL));

                lwip_send(context->tcp_socket, "OK", 2, 0);
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                esp_restart();
            }
            lwip_close(context->tcp_socket);
            context->tcp_socket = -1;

            free(context->temp);
            free(context->decompressor);
            free(context->decompressor_dictionary);
            context->temp = NULL;
            context->decompressor = NULL;
            context->decompressor_dictionary = NULL;

            xTimerChangePeriod(timer, 1000 / portTICK_PERIOD_MS, 0);
        }
    }
    else if (context->udp_socket >= 0)
    {
        char data[256];
        struct sockaddr_storage from;
        socklen_t fromlen = sizeof(from);
        if (lwip_recvfrom(context->udp_socket, data, 256, 0, (struct sockaddr*)&from, &fromlen) > 0)
        {
            char* buffer = data;
            const char* command = strsep(&buffer, " \n");
            const char* remote_port = strsep(&buffer, " \n");
            const char* content_size = strsep(&buffer, " \n");
            const char* file_md5 = strsep(&buffer, " \n");
            if (command && remote_port && content_size && file_md5)
            {
                ESP_LOGI(TAG, "Command: %s", command);
                ESP_LOGI(TAG, "Remote port: %s", remote_port);
                ESP_LOGI(TAG, "Context size: %s", content_size);
                ESP_LOGI(TAG, "File MD5: %s", file_md5);

                context->offset = 0;
                context->size = strtol(content_size, NULL, 10);
                context->temp = realloc(context->temp, 1536);

                context->tcp_socket = lwip_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

                int mode = 1;
                lwip_ioctl(context->tcp_socket, FIONBIO, &mode);

                struct sockaddr_in sockaddr = {};
                sockaddr.sin_len = sizeof(sockaddr);
                sockaddr.sin_family = AF_INET;
                sockaddr.sin_port = htons(strtol(remote_port, NULL, 10));
                sockaddr.sin_addr = ((struct sockaddr_in*)&from)->sin_addr;
                lwip_connect(context->tcp_socket, (struct sockaddr*)&sockaddr, sizeof(sockaddr));

                lwip_sendto(context->udp_socket, "OK", 2, 0, (struct sockaddr*)&from, fromlen);

                xTimerChangePeriod(timer, 10 / portTICK_PERIOD_MS, 0);
            }
        }
    }
}

void ota_init(int port)
{
    if (context == NULL)
    {
        context = calloc(1, sizeof(struct ota_context));
        context->udp_socket = lwip_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        context->tcp_socket = -1;

        int mode = 1;
        lwip_ioctl(context->udp_socket, FIONBIO, &mode);

        struct sockaddr_in sockaddr = {};
        sockaddr.sin_len = sizeof(sockaddr);
        sockaddr.sin_family = AF_INET;
        sockaddr.sin_port = htons(port);
        sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        lwip_bind(context->udp_socket, (struct sockaddr*)&sockaddr, sizeof(sockaddr));

        TimerHandle_t timer = xTimerCreate("OTA", 1000 / portTICK_PERIOD_MS, pdTRUE, (void*)"OTA", ota_handler);
        xTimerStart(timer, 0);
    }
}
