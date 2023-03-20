#include <errno.h>
#define INTEGER_ONLY
//#define vfiscanf vfiscanf_unused
//#define _vfiscanf_r _vfiscanf_r_unused
//#define __svfiscanf __svfiscanf_unused
//#define __svfiscanf_r __svfiscanf_r_unused
#include "vfscanf.c"

//#undef vfiscanf
//int vfscanf(register FILE *fp, const char *fmt, va_list ap);
//int vfiscanf(register FILE *fp, const char *fmt, va_list ap)
//{
//  return vfscanf(fp, fmt, ap);
//}

//#undef _vfiscanf_r
//int _vfscanf_r(struct _reent *data, register FILE *fp, const char *fmt, va_list ap);
//int _vfiscanf_r(struct _reent *data, register FILE *fp, const char *fmt, va_list ap)
//{
//  return _vfscanf_r(data, fp, fmt, ap);
//}

//#undef __svfiscanf
//int __svfscanf(register FILE *fp, char const *fmt0, va_list ap);
//int __svfiscanf(register FILE *fp, char const *fmt0, va_list ap)
//{
//  return __svfscanf(fp, fmt0, ap);
//}

//#undef __svfiscanf_r
//int __svfscanf_r(struct _reent *rptr, register FILE *fp, char const *fmt0, va_list ap);
//int __svfiscanf_r(struct _reent *rptr, register FILE *fp, char const *fmt0, va_list ap)
//{
//  return __svfscanf_r(rptr, fp, fmt0, ap);
//}
