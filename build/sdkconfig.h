#pragma once

#pragma clang diagnostic ignored "-Wmacro-redefined"
#pragma clang diagnostic ignored "-Wsection"

#define __ESP_FILE__ __FILE_NAME__

#define LFS_NAME_MAX 32
#define LFS_NO_ASSERT
#define LFS_NO_DEBUG
#define LFS_NO_ERROR
#define LFS_NO_WARN

#include <esp_attr.h>

#ifndef __Espressif__
#undef _SECTION_ATTR_IMPL
#define _SECTION_ATTR_IMPL(SECTION, COUNTER)
#define __XSTRING(x) __STRING(x)
#define _RETARGETABLE_LOCKING
#endif
