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
#include <esp_err.h>
#include <esp_log.h>
#include <esp_netif.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>

extern const char version[];
extern const char build_date[];
extern const char web_css[];
extern char thisname[24];
extern char number[128];
extern bool debug;

#ifdef __cplusplus
};
#endif
