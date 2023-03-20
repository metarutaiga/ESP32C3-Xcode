#include <errno.h>
#define INTEGER_ONLY
#define _vfiprintf_r _vfiprintf_r_unused
#include "vfprintf.c"

#undef _vfiprintf_r
int _vfiprintf_r(struct _reent *data, FILE * fp, const char *fmt0, va_list ap)
{
  return _vfprintf_r(data, fp, fmt0, ap);
}
