#include "esp32.h"
#include <sys/time.h>
#include <esp_http_server.h>
#include <lwip/apps/sntp.h>
#include "app/fs.h"
#include "app/httpd.h"
#include "app/mqtt.h"
#include "app/ota.h"
#include "app/temperature.h"

extern "C" void init_udp_console(const char* ip);

extern const char web_css[] __attribute__((weak));
const char web_css[] = "";

esp_err_t web_system(httpd_req_t* req)
{
    string html;

    // Head
    wifi_mode_t mode = WIFI_MODE_APSTA;
    esp_wifi_get_mode(&mode);
    html += "<html>";
    html += "<head>";
    html +=     mode == WIFI_MODE_STA ? web_css : "";
    html +=     "<title>";
    html +=         thisname;
    html +=     "</title>";
    html += "</head>";
    html += "<body>";
    html += "<table>";
    if (httpd_resp_send_chunk(req, html.data(), html.length()) != ESP_OK)
        return ESP_FAIL;
    html.clear();

    // SSID
    string ssid;
    int fd = fs_open("ssid", "r");
    if (fd >= 0)
    {
        ssid = fs_gets(number, 256, fd);
        fs_close(fd);
    }
    html += "<form method='get' action='ssid'>";
    html +=     "<tr>";
    html +=         "<td>";
    html +=             "<label>SSID</label>";
    html +=         "</td>";
    html +=         "<td>";
    html +=             "<input name='ssid' length=32 value='" + ssid + "'>";
    html +=         "</td>";
    html +=     "</tr>";
    html +=     "<tr>";
    html +=         "<td>";
    html +=             "<label>PASS</label>";
    html +=         "</td>";
    html +=         "<td>";
    html +=             "<input type='password' name='pass' length=32>";
    html +=         "</td>";
    html +=         "<td>";
    html +=             "<input type='submit'>";
    html +=         "</td>";
    html +=     "</tr>";
    html += "</form>";
    if (httpd_resp_send_chunk(req, html.data(), html.length()) != ESP_OK)
        return ESP_FAIL;
    html.clear();

    // OTA
    string ota;
    fd = fs_open("ota", "r");
    if (fd >= 0)
    {
        ota = fs_gets(number, 256, fd);
        fs_close(fd);
    }
    html += "<form method='get' action='ota'>";
    html +=     "<tr>";
    html +=         "<td>";
    html +=             "<label>OTA</label>";
    html +=         "</td>";
    html +=         "<td>";
    html +=             "<input name='ota' length=32 value='" + ota + "'>";
    html +=         "</td>";
    html +=         "<td>";
    html +=             "<input type='submit'>";
    html +=         "</td>";
    html +=     "</tr>";
    html += "</form>";
    if (httpd_resp_send_chunk(req, html.data(), html.length()) != ESP_OK)
        return ESP_FAIL;
    html.clear();

    // UDP
    string udp;
    fd = fs_open("udp", "r");
    if (fd >= 0)
    {
        udp = fs_gets(number, 256, fd);
        fs_close(fd);
    }
    html += "<form method='get' action='udp'>";
    html +=     "<tr>";
    html +=         "<td>";
    html +=             "<label>UDP</label>";
    html +=         "</td>";
    html +=         "<td>";
    html +=             "<input name='udp' length=32 value='" + udp + "'>";
    html +=         "</td>";
    html +=         "<td>";
    html +=             "<input type='submit'>";
    html +=         "</td>";
    html +=     "</tr>";
    html += "</form>";
    if (httpd_resp_send_chunk(req, html.data(), html.length()) != ESP_OK)
        return ESP_FAIL;
    html.clear();

    // MQTT
    string mqtt;
    string mqttPort;
    fd = fs_open("mqtt", "r");
    if (fd >= 0)
    {
        mqtt = fs_gets(number, 256, fd);
        mqttPort = fs_gets(number, 256, fd);
        fs_close(fd);
    }
    if (mqttPort.empty())
    {
        mqttPort = "1883";
    }
    html += "<form method='get' action='mqtt'>";
    html +=     "<tr>";
    html +=         "<td>";
    html +=             "<label>MQTT</label>";
    html +=         "</td>";
    html +=         "<td>";
    html +=             "<input name='mqtt' length=32 value='" + mqtt + "'>";
    html +=         "</td>";
    html +=     "</tr>";
    html +=     "<tr>";
    html +=         "<td>";
    html +=             "<label>PORT</label>";
    html +=         "</td>";
    html +=         "<td>";
    html +=             "<input name='port' length=32 value='" + mqttPort + "'>";
    html +=         "</td>";
    html +=         "<td>";
    html +=             "<input type='submit'>";
    html +=         "</td>";
    html +=     "</tr>";
    html += "</form>";
    if (httpd_resp_send_chunk(req, html.data(), html.length()) != ESP_OK)
        return ESP_FAIL;
    html.clear();

    // NTP
    string ntp;
    string ntpZone;
    fd = fs_open("ntp", "r");
    if (fd >= 0)
    {
        ntp = fs_gets(number, 256, fd);
        ntpZone = fs_gets(number, 256, fd);
        fs_close(fd);
    }
    if (ntp.empty())
    {
        ntp = "pool.ntp.org";
    }
    if (ntpZone.empty())
    {
        ntpZone = "GMT-8";
    }
    html += "<form method='get' action='ntp'>";
    html +=     "<tr>";
    html +=         "<td>";
    html +=             "<label>NTP</label>";
    html +=         "</td>";
    html +=         "<td>";
    html +=             "<input name='name' length=32 value='" + ntp + "'>";
    html +=         "</td>";
    html +=     "</tr>";
    html +=     "<tr>";
    html +=         "<td>";
    html +=             "<label>ZONE</label>";
    html +=         "</td>";
    html +=         "<td>";
    html +=             "<input name='zone' length=32 value='" + ntpZone + "'>";
    html +=         "</td>";
    html +=         "<td>";
    html +=             "<input type='submit'>";
    html +=         "</td>";
    html +=     "</tr>";
    html += "</form>";
    if (httpd_resp_send_chunk(req, html.data(), html.length()) != ESP_OK)
        return ESP_FAIL;
    html.clear();

    // Time
    time_t now;
    struct tm timeinfo;
    char strftime_buf[64];
    time(&now);
    localtime_r(&now, &timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    html += "</table>";
    html += strftime_buf;

    // Reset
    html += "<form method='get' action='reset'>";
    html +=     "<button type='submit'>Reset</button>";
    html += "</form>";

    // Temperature
    sprintf(number, "%d", temperature());
    html += "Temperature : " + string(number) + "</br>";

    // IP
    esp_netif_ip_info_t ip_info = {};
    if (esp_netif_get_ip_info(sta_netif, &ip_info) == ESP_OK)
    {
        sprintf(number, IPSTR, IP2STR(&ip_info.ip));
        html += "STA IP : " + string(number) + "</br>";
    }
    sprintf(number, "%d", wifi_sta_disconnected);
    html += "STA Connected : " + string(number) + "</br>";
    sprintf(number, "%d", wifi_sta_got_ip);
    html += "STA Disconnected : " + string(number) + "</br>";

    // MQTT
    html += "MQTT Connected : " + string(mqtt_connected_internal() ? "YES" : "NO") + "</br>";
    html += "MQTT Ready : " + string(mqtt_ready_internal() ? "YES" : "NO") + "</br>";

    // Tail
    html += "</body>";
    html += "</html>";
    if (httpd_resp_send_chunk(req, html.data(), html.length()) != ESP_OK)
        return ESP_FAIL;
    html.clear();

    return httpd_resp_send_chunk(req, NULL, 0);
}

esp_err_t web_ssid(httpd_req_t* req)
{
    httpd_resp_set_status(req, "302 Found");
    httpd_resp_set_hdr(req, "Location", "/");
    httpd_resp_send(req, NULL, 0);

    size_t len;
    char buffer[(len = httpd_req_get_url_query_len(req) + 1)];
    httpd_req_get_url_query_str(req, buffer, len);

    string text;
    char value[64];
    value[0] = 0; httpd_query_key_value(buffer, "ssid", value, 64); text += url_decode(value); text += '\n';
    value[0] = 0; httpd_query_key_value(buffer, "pass", value, 64); text += url_decode(value); text += '\n';

    int fd = fs_open("ssid", "w");
    if (fd >= 0)
    {
        fs_write(text.data(), text.length(), fd);
        fs_close(fd);
    }

    wifi_mode_t mode = WIFI_MODE_NULL;
    esp_wifi_get_mode(&mode);
    if (/*mode == WIFI_MODE_STA || */mode == WIFI_MODE_APSTA)
    {
        int fd = fs_open("ssid", "r");
        if (fd >= 0)
        {
            wifi_config_t config = {};
            strcpy((char*)config.sta.ssid, fs_gets(number, 256, fd));
            strcpy((char*)config.sta.password, fs_gets(number, 256, fd));
            config.sta.scan_method = WIFI_ALL_CHANNEL_SCAN;
            config.sta.sort_method = WIFI_CONNECT_AP_BY_SIGNAL;
            fs_close(fd);
            ESP_ERROR_CHECK(esp_wifi_disconnect());
            ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &config));
            ESP_ERROR_CHECK(esp_wifi_connect());
        }
    }

    return ESP_OK;
}

esp_err_t web_ota(httpd_req_t* req)
{
    httpd_resp_set_status(req, "302 Found");
    httpd_resp_set_hdr(req, "Location", "/");
    httpd_resp_send(req, NULL, 0);

    size_t len;
    char buffer[(len = httpd_req_get_url_query_len(req) + 1)];
    httpd_req_get_url_query_str(req, buffer, len);

    string text;
    char value[64];
    value[0] = 0; httpd_query_key_value(buffer, "ota", value, 64); text += url_decode(value); text += '\n';

    int fd = fs_open("ota", "w");
    if (fd >= 0)
    {
        fs_write(text.data(), text.length(), fd);
        fs_close(fd);
    }

    wifi_mode_t mode = WIFI_MODE_NULL;
    esp_wifi_get_mode(&mode);
    if (mode == WIFI_MODE_STA || mode == WIFI_MODE_APSTA)
    {
        fd = fs_open("ota", "r");
        if (fd >= 0)
        {
            if (strcmp(fs_gets(number, 256, fd), "YES") == 0)
            {
                ota_init(8685);
                debug = true;
            }
            fs_close(fd);
        }
    }

    return ESP_OK;
}

esp_err_t web_udp(httpd_req_t* req)
{
    httpd_resp_set_status(req, "302 Found");
    httpd_resp_set_hdr(req, "Location", "/");
    httpd_resp_send(req, NULL, 0);

    size_t len;
    char buffer[(len = httpd_req_get_url_query_len(req) + 1)];
    httpd_req_get_url_query_str(req, buffer, len);

    string text;
    char value[64];
    value[0] = 0; httpd_query_key_value(buffer, "udp", value, 64); text += url_decode(value); text += '\n';

    int fd = fs_open("udp", "w");
    if (fd >= 0)
    {
        fs_write(text.data(), text.length(), fd);
        fs_close(fd);
    }

    wifi_mode_t mode = WIFI_MODE_NULL;
    esp_wifi_get_mode(&mode);
    if (mode == WIFI_MODE_STA || mode == WIFI_MODE_APSTA)
    {
        fd = fs_open("udp", "r");
        if (fd >= 0)
        {
            if (strchr(fs_gets(number, 256, fd), '.') != 0)
            {
                init_udp_console(number);
                debug = true;
            }
            fs_close(fd);
        }
    }

    return ESP_OK;
}

esp_err_t web_mqtt(httpd_req_t* req)
{
    httpd_resp_set_status(req, "302 Found");
    httpd_resp_set_hdr(req, "Location", "/");
    httpd_resp_send(req, NULL, 0);

    size_t len;
    char buffer[(len = httpd_req_get_url_query_len(req) + 1)];
    httpd_req_get_url_query_str(req, buffer, len);

    string text;
    char value[64];
    value[0] = 0; httpd_query_key_value(buffer, "mqtt", value, 64); text += url_decode(value); text += '\n';
    value[0] = 0; httpd_query_key_value(buffer, "port", value, 64); text += url_decode(value); text += '\n';

    int fd = fs_open("mqtt", "w");
    if (fd >= 0)
    {
        fs_write(text.data(), text.length(), fd);
        fs_close(fd);
    }

    wifi_mode_t mode = WIFI_MODE_NULL;
    esp_wifi_get_mode(&mode);
    if (mode == WIFI_MODE_STA || mode == WIFI_MODE_APSTA)
    {
        int fd = fs_open("mqtt", "r");
        if (fd >= 0)
        {
            string mqtt = fs_gets(number, 256, fd);
            string port = fs_gets(number, 256, fd);
            mqtt_setup(mqtt.c_str(), strtol(port.c_str(), nullptr, 10));
            fs_close(fd);
        }
    }

    return ESP_OK;
}

esp_err_t web_ntp(httpd_req_t* req)
{
    httpd_resp_set_status(req, "302 Found");
    httpd_resp_set_hdr(req, "Location", "/");
    httpd_resp_send(req, NULL, 0);

    size_t len;
    char buffer[(len = httpd_req_get_url_query_len(req) + 1)];
    httpd_req_get_url_query_str(req, buffer, len);

    string text;
    char value[64];
    value[0] = 0; httpd_query_key_value(buffer, "name", value, 64); text += url_decode(value); text += '\n';
    value[0] = 0; httpd_query_key_value(buffer, "zone", value, 64); text += url_decode(value); text += '\n';

    int fd = fs_open("ntp", "w");
    if (fd >= 0)
    {
        fs_write(text.data(), text.length(), fd);
        fs_close(fd);
    }

    wifi_mode_t mode = WIFI_MODE_NULL;
    esp_wifi_get_mode(&mode);
    if (mode == WIFI_MODE_STA || mode == WIFI_MODE_APSTA)
    {
        fd = fs_open("ntp", "r");
        if (fd >= 0)
        {
            string ntp = fs_gets(number, 256, fd);
            string zone = fs_gets(number, 256, fd);
            fs_close(fd);
            free((void*)sntp_getservername(0));
            sntp_setoperatingmode(SNTP_OPMODE_POLL);
            sntp_setservername(0, strdup(ntp.c_str()));
            sntp_init();
            setenv("TZ", zone.c_str(), 1);
            tzset();
        }
    }

    return ESP_OK;
}

esp_err_t web_reset(httpd_req_t* req)
{
    httpd_resp_set_status(req, "302 Found");
    httpd_resp_set_hdr(req, "Location", "/");
    httpd_resp_send(req, NULL, 0);

    xTaskCreate([](void*)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        esp_restart();
    }, "Reset Task", 4096, nullptr, 5, nullptr);

    return ESP_OK;
}
