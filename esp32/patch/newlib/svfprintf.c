#include <errno.h>
#define STRING_ONLY
#include "vfprintf.c"

int _svfiprintf_r(struct _reent *data, FILE * fp, const char *fmt0, va_list ap)
{
  return _svfprintf_r(data, fp, fmt0, ap);
}
