#include "esp32.h"
#include "app/fs.h"
#include <nvs.h>

#define TAG __FILE_NAME__

static const char* nvs_name(char* buffer, const char* left, const char* right)
{
    int index = 0;
    for (char c; (c = *left); left++) {
        buffer[index++] = c;
    }
    buffer[index++] = '/';
    for (char c; (c = *right); right++) {
        if (c == '/')
            c = ':';
        buffer[index++] = c;
    }
    buffer[index++] = 0;
    return buffer;
}

esp_err_t nvs_open(const char* name, nvs_open_mode_t open_mode, nvs_handle_t* out_handle)
{
    fs_mkdir(name);
    (*out_handle) = (nvs_handle_t)strdup(name);
    return ESP_OK;
}
esp_err_t nvs_open_from_partition(const char* part_name, const char* name, nvs_open_mode_t open_mode, nvs_handle_t* out_handle)
{
    char* temp = malloc(strlen(part_name) + 1 + strlen(name) + 1);
    nvs_name(temp, part_name, name);
    fs_mkdir(part_name);
    fs_mkdir(temp);
    (*out_handle) = (nvs_handle_t)temp;
    return ESP_OK;
}

#define NVS_SET(data, length) \
{ \
    size_t count = length; \
    if (fs_stat(nvs_name(number, (char*)handle, key)) == count) { \
        char current[count]; \
        if (nvs_get_blob(handle, key, current, &count) == ESP_OK && memcmp(data, current, count) == 0) { \
            return ESP_OK; \
        } \
    } \
    int fd = fs_open(number, "wb"); \
    if (fd < 0) \
        return ESP_ERR_NVS_NOT_FOUND; \
    fs_write(data, count, fd); \
    fs_close(fd); \
    return ESP_OK; \
}

esp_err_t nvs_set_i8(nvs_handle_t handle, const char* key, int8_t value) NVS_SET(&value, sizeof(value));
esp_err_t nvs_set_u8(nvs_handle_t handle, const char* key, uint8_t value) NVS_SET(&value, sizeof(value));
esp_err_t nvs_set_i16(nvs_handle_t handle, const char* key, int16_t value) NVS_SET(&value, sizeof(value));
esp_err_t nvs_set_u16(nvs_handle_t handle, const char* key, uint16_t value) NVS_SET(&value, sizeof(value));
esp_err_t nvs_set_i32(nvs_handle_t handle, const char* key, int32_t value) NVS_SET(&value, sizeof(value));
esp_err_t nvs_set_u32(nvs_handle_t handle, const char* key, uint32_t value) NVS_SET(&value, sizeof(value));
esp_err_t nvs_set_i64(nvs_handle_t handle, const char* key, int64_t value) NVS_SET(&value, sizeof(value));
esp_err_t nvs_set_u64(nvs_handle_t handle, const char* key, uint64_t value) NVS_SET(&value, sizeof(value));
esp_err_t nvs_set_str(nvs_handle_t handle, const char* key, const char* value) NVS_SET(value, strlen(value));
esp_err_t nvs_set_blob(nvs_handle_t handle, const char* key, const void* value, size_t length) NVS_SET(value, length);

#define NVS_GET(data, length) \
{ \
    size_t count = length; \
    if (fs_stat(nvs_name(number, (char*)handle, key)) < 0) \
        return ESP_ERR_NVS_NOT_FOUND; \
    int fd = fs_open(number, "rb"); \
    if (fd < 0) \
        return ESP_ERR_NVS_NOT_FOUND; \
    fs_read(data, count, fd); \
    fs_close(fd); \
    return ESP_OK; \
}

esp_err_t nvs_get_i8(nvs_handle_t handle, const char* key, int8_t* out_value) NVS_GET(out_value, sizeof(*out_value));
esp_err_t nvs_get_u8(nvs_handle_t handle, const char* key, uint8_t* out_value) NVS_GET(out_value, sizeof(*out_value));
esp_err_t nvs_get_i16(nvs_handle_t handle, const char* key, int16_t* out_value) NVS_GET(out_value, sizeof(*out_value));
esp_err_t nvs_get_u16(nvs_handle_t handle, const char* key, uint16_t* out_value) NVS_GET(out_value, sizeof(*out_value));
esp_err_t nvs_get_i32(nvs_handle_t handle, const char* key, int32_t* out_value) NVS_GET(out_value, sizeof(*out_value));
esp_err_t nvs_get_u32(nvs_handle_t handle, const char* key, uint32_t* out_value) NVS_GET(out_value, sizeof(*out_value));
esp_err_t nvs_get_i64(nvs_handle_t handle, const char* key, int64_t* out_value) NVS_GET(out_value, sizeof(*out_value));
esp_err_t nvs_get_u64(nvs_handle_t handle, const char* key, uint64_t* out_value) NVS_GET(out_value, sizeof(*out_value));
esp_err_t nvs_get_str(nvs_handle_t handle, const char* key, char* out_value, size_t* length)
{
    int stat = fs_stat(nvs_name(number, (char*)handle, key));
    if (stat < 0)
        return ESP_ERR_NVS_NOT_FOUND;
    if (out_value) {
        int fd = fs_open(number, "rb");
        if (fd < 0)
            return ESP_ERR_NVS_NOT_FOUND;
        stat = fs_read(out_value, stat, fd);
        out_value[stat] = 0;
        fs_close(fd);
    }
    (*length) = stat + 1;
    return ESP_OK;
}
esp_err_t nvs_get_blob(nvs_handle_t handle, const char* key, void* out_value, size_t* length)
{
    int stat = fs_stat(nvs_name(number, (char*)handle, key));
    if (stat < 0)
        return ESP_ERR_NVS_NOT_FOUND;
    if (out_value) {
        int fd = fs_open(number, "rb");
        if (fd < 0)
            return ESP_ERR_NVS_NOT_FOUND;
        stat = fs_read(out_value, stat, fd);
        fs_close(fd);
    }
    (*length) = stat;
    return ESP_OK;
}

esp_err_t nvs_erase_key(nvs_handle_t handle, const char* key) { sprintf(number, "%s/%s", (char*)handle, key); fs_remove(number); return ESP_OK; }
esp_err_t nvs_erase_all(nvs_handle_t handle) { fs_remove((char*)handle); return ESP_OK; }
esp_err_t nvs_commit(nvs_handle_t handle) { return ESP_OK; }
void nvs_close(nvs_handle_t handle) { free((void*)handle); }

esp_err_t nvs_get_stats(const char* part_name, nvs_stats_t* nvs_stats) { return ESP_FAIL; }
esp_err_t nvs_get_used_entry_count(nvs_handle_t handle, size_t* used_entries) { return ESP_FAIL; }

esp_err_t nvs_entry_find(const char* part_name, const char* namespace_name, nvs_type_t type, nvs_iterator_t* output_iterator)
{
    int dir = fs_dir_open(nvs_name(number, part_name, namespace_name));
    if (dir < 0)
        return ESP_ERR_NVS_NOT_FOUND;
    nvs_entry_info_t* info = calloc(1, sizeof(nvs_entry_info_t));
    info->type = dir;
    (*output_iterator) = (nvs_iterator_t)info;
    return nvs_entry_next(output_iterator);
}
esp_err_t nvs_entry_next(nvs_iterator_t* iterator)
{
    if (iterator == NULL)
        return ESP_ERR_INVALID_ARG;
    nvs_entry_info_t* info = (nvs_entry_info_t*)(*iterator);
    if (info == NULL)
        return ESP_ERR_INVALID_ARG;
    int dir = info->type;
    if (fs_dir_read(dir, info->key, NVS_KEY_NAME_MAX_SIZE, NULL) < 0) {
        nvs_release_iterator(*iterator);
        (*iterator) = NULL;
        return ESP_ERR_NVS_NOT_FOUND;
    }
    return ESP_OK;
}
esp_err_t nvs_entry_info(const nvs_iterator_t iterator, nvs_entry_info_t* out_info)
{
    nvs_entry_info_t* info = (nvs_entry_info_t*)iterator;
    if (info == NULL)
        return ESP_ERR_INVALID_ARG;
    memcpy(out_info, info, sizeof(nvs_entry_info_t));
    return ESP_OK;
}
void nvs_release_iterator(nvs_iterator_t iterator)
{
    nvs_entry_info_t* info = (nvs_entry_info_t*)iterator;
    if (info == NULL)
        return;
    int dir = info->type;
    fs_dir_close(dir);
    free(info);
}

#include <nvs_flash.h>

esp_err_t nvs_flash_init(void) { return ESP_OK; }
esp_err_t nvs_flash_init_partition(const char* partition_label) { return ESP_OK; }
esp_err_t nvs_flash_init_partition_ptr(const esp_partition_t* partition) { return ESP_OK; }
esp_err_t nvs_flash_deinit(void) { return ESP_OK; }
esp_err_t nvs_flash_deinit_partition(const char* partition_label) { return ESP_OK; }
esp_err_t nvs_flash_erase(void) { return ESP_OK; }
esp_err_t nvs_flash_erase_partition(const char* part_name) { return ESP_OK; }
esp_err_t nvs_flash_erase_partition_ptr(const esp_partition_t* partition) { return ESP_OK; }
esp_err_t nvs_flash_secure_init(nvs_sec_cfg_t* cfg) { return ESP_OK; }
esp_err_t nvs_flash_secure_init_partition(const char* partition_label, nvs_sec_cfg_t* cfg) { return ESP_OK; }
esp_err_t nvs_flash_generate_keys(const esp_partition_t* partition, nvs_sec_cfg_t* cfg) { return ESP_OK; }
esp_err_t nvs_flash_read_security_cfg(const esp_partition_t* partition, nvs_sec_cfg_t* cfg) { return ESP_OK; }
