#include <errno.h>
#define INTEGER_ONLY
#define STRING_ONLY
//#define __ssvfiscanf_r __ssvfiscanf_r_unused
#include "vfscanf.c"

//#undef __ssvfiscanf_r
//int __ssvfscanf_r(struct _reent *rptr, register FILE *fp, char const *fmt0, va_list ap);
//int __ssvfiscanf_r(struct _reent *rptr, register FILE *fp, char const *fmt0, va_list ap)
//{
//  return __ssvfscanf_r(rptr, fp, fmt0, ap);
//}
