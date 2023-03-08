#include "sc.h"
#include "crypto_int64.h"
#include "crypto_uint32.h"
#include "crypto_uint64.h"

/*
static crypto_uint64 load_3(const unsigned char *in)
{
  crypto_uint64 result;
  result = (crypto_uint64) in[0];
  result |= ((crypto_uint64) in[1]) << 8;
  result |= ((crypto_uint64) in[2]) << 16;
  return result;
}

static crypto_uint64 load_4(const unsigned char *in)
{
  crypto_uint64 result;
  result = (crypto_uint64) in[0];
  result |= ((crypto_uint64) in[1]) << 8;
  result |= ((crypto_uint64) in[2]) << 16;
  result |= ((crypto_uint64) in[3]) << 24;
  return result;
}
*/

/*
Input:
  s[0]+256*s[1]+...+256^63*s[63] = s

Output:
  s[0]+256*s[1]+...+256^31*s[31] = s mod l
  where l = 2^252 + 27742317777372353535851937790883648493.
  Overwrites s in place.
*/

void sc_reduce(unsigned char *s)
{
  crypto_int64 x[24];
  x[0] = 2097151 & load_3(s);
  x[1] = 2097151 & (load_4(s + 2) >> 5);
  x[2] = 2097151 & (load_3(s + 5) >> 2);
  x[3] = 2097151 & (load_4(s + 7) >> 7);
  x[4] = 2097151 & (load_4(s + 10) >> 4);
  x[5] = 2097151 & (load_3(s + 13) >> 1);
  x[6] = 2097151 & (load_4(s + 15) >> 6);
  x[7] = 2097151 & (load_3(s + 18) >> 3);
  x[8] = 2097151 & load_3(s + 21);
  x[9] = 2097151 & (load_4(s + 23) >> 5);
  x[10] = 2097151 & (load_3(s + 26) >> 2);
  x[11] = 2097151 & (load_4(s + 28) >> 7);
  x[12] = 2097151 & (load_4(s + 31) >> 4);
  x[13] = 2097151 & (load_3(s + 34) >> 1);
  x[14] = 2097151 & (load_4(s + 36) >> 6);
  x[15] = 2097151 & (load_3(s + 39) >> 3);
  x[16] = 2097151 & load_3(s + 42);
  x[17] = 2097151 & (load_4(s + 44) >> 5);
  x[18] = 2097151 & (load_3(s + 47) >> 2);
  x[19] = 2097151 & (load_4(s + 49) >> 7);
  x[20] = 2097151 & (load_4(s + 52) >> 4);
  x[21] = 2097151 & (load_3(s + 55) >> 1);
  x[22] = 2097151 & (load_4(s + 57) >> 6);
  x[23] = (load_4(s + 60) >> 3);
  sc_reduce_internal(s, x);
}
