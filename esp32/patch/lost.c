#include "esp32.h"

char* url_decode(char* param)
{
    int l = 0;
    int r = 0;
    for (;;) {
        char c = param[r++];
        if (c == '%') {
            char temp[3] = { param[r], param[r + 1] };
            c = strtol(temp, 0, 16);
            r += 2;
        }
        param[l++] = c;
        if (c == 0)
            break;
    }
    return param;
}

uint32_t mesh_sta_auth_expire_time(void)
{
    return 0;
}

esp_err_t esp_wifi_sta_wpa2_ent_disable(void)
{
    return ESP_OK;
}
