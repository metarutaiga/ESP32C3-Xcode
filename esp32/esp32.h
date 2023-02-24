#pragma once

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <esp_attr.h>
//#include <esp_clk.h>
#include <esp_err.h>
#include <esp_log.h>
#include <esp_netif.h>
#include <esp_system.h>
//#include <esp_timer.h>
#include <esp_wifi.h>
//#include <freertos/FreeRTOS.h>
//#include <freertos/task.h>
//#include <freertos/timers.h>

//extern char* url_decode(char* param);
//extern uint32_t lfs_crc(uint32_t crc, const void* buffer, size_t size);
//void esp_reset(esp_reset_reason_t hint);
//inline uint32_t IRAM_ATTR esp_get_cycle_count()
//{
//    extern uint32_t g_esp_os_cpu_clk;
//    return g_esp_os_cpu_clk + soc_get_ccount();
//}
//inline int32_t esp_timer_get_time_ms()
//{
//    return (int32_t)(esp_timer_get_time() / 1000);
//}

extern const char version[];
extern const char build_date[];
extern const char web_css[];
extern char thisname[24];
extern char number[128];
extern bool debug;

#ifdef __cplusplus
};
#endif
