#pragma once

#ifdef __cplusplus
typedef int div_t;
typedef long ldiv_t;
typedef long long lldiv_t;
extern "C" void abort();
extern "C" ldiv_t ldiv(long __x, long __y);
extern "C" lldiv_t lldiv(long long __x, long long __y);
#include <malloc.h>
#include <string>
typedef std::string string;
#endif

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

extern char* url_decode(char* param);
extern int setenv(const char *__string, const char *__value, int __overwrite);

extern const char version[];
extern const char build_date[];
extern const char web_css[];
extern char thisname[24];
extern char number[128];
extern bool debug;

#ifdef __cplusplus
};
#endif
