#include "ge.h"
#include "crypto_uint32.h"

/* Packed coordinates of the base point */
static const unsigned char ge25519_base_x[32] = {0x1A, 0xD5, 0x25, 0x8F, 0x60, 0x2D, 0x56, 0xC9, 0xB2, 0xA7, 0x25, 0x95, 0x60, 0xC7, 0x2C, 0x69,
                                                 0x5C, 0xDC, 0xD6, 0xFD, 0x31, 0xE2, 0xA4, 0xC0, 0xFE, 0x53, 0x6E, 0xCD, 0xD3, 0x36, 0x69, 0x21};
static const unsigned char ge25519_base_y[32] = {0x58, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
                                                 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66};
static const unsigned char ge25519_base_z[32] = {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static const unsigned char ge25519_base_t[32] = {0xA3, 0xDD, 0xB7, 0xA5, 0xB3, 0x8A, 0xDE, 0x6D, 0xF5, 0x52, 0x51, 0x77, 0x80, 0x9F, 0xF0, 0x20,
                                                 0x7D, 0xE3, 0xAB, 0x64, 0x8E, 0x4E, 0xEA, 0x66, 0x65, 0x76, 0x8B, 0xD7, 0x0F, 0x5F, 0x87, 0x67};

/* Constant-time version of: r = b ? p : q */
static void cmov_p3(ge_p3 *r,const ge_p3 *p,const ge_p3 *q,unsigned char b)
{
  fe_copy(r->X,b?p->X:q->X);
  fe_copy(r->Y,b?p->Y:q->Y);
  fe_copy(r->Z,b?p->Z:q->Z);
  fe_copy(r->T,b?p->T:q->T);
}

static void ge_scalarmult(ge_p3 *r,const ge_p3 *p,const unsigned char *s)
{
  int i;

  ge_p3 neutral;
  ge_p3 t;
  ge_cached tc;
  ge_p1p1 tp1p1;

  // Precomputation
  ge_p3_0(&neutral);
  *r = neutral;

  // Scalar multiplication
  for(i=255;i>=0;--i)
  {
    ge_p2_dbl(&tp1p1,(ge_p2 *)r);
    ge_p1p1_to_p3(r,&tp1p1);
    cmov_p3(&t,p,&neutral,(s[i/8]>>(i&7))&1);
    ge_p3_to_cached(&tc,&t);
    ge_add(&tp1p1,r,&tc);
    if((i&7)!=0) ge_p1p1_to_p2((ge_p2 *)r,&tp1p1);
    else ge_p1p1_to_p3(r,&tp1p1); /* convert to p3 representation at the end */
  }
}

/*
h = a * B
where a = a[0]+256*a[1]+...+256^31 a[31]
B is the Ed25519 base point (x,4/5) with x positive.

Preconditions:
  a[31] <= 127
*/

void ge_scalarmult_base(ge_p3 *h,const unsigned char *a)
{
  /* XXX: Better algorithm for known-base-point scalar multiplication */
  ge_p3 base;
  fe_frombytes(base.X, ge25519_base_x);
  fe_frombytes(base.Y, ge25519_base_y);
  fe_frombytes(base.Z, ge25519_base_z);
  fe_frombytes(base.T, ge25519_base_t);
  ge_scalarmult(h,&base,a);
}
