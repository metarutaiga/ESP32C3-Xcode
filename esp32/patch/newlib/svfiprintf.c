#include <errno.h>
#define INTEGER_ONLY
#define STRING_ONLY
#define _svfiprintf_r _svfiprintf_r_unused
#include "vfprintf.c"

#undef _svfiprintf_r
int _svfprintf_r(struct _reent *data, FILE * fp, const char *fmt0, va_list ap);
int _svfiprintf_r(struct _reent *data, FILE * fp, const char *fmt0, va_list ap)
{
  return _svfprintf_r(data, fp, fmt0, ap);
}
