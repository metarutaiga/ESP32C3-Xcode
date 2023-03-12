/*
chacha-ref.c version 20080118
D. J. Bernstein
Public domain.
*/

typedef unsigned char u8;
typedef unsigned long u32;
typedef unsigned long long u64;

#define U8V(v) ((u8)(v))
#define U32V(v) ((u32)(v))
#define ROTL32(v, n) (U32V((v) << (n)) | ((v) >> (32 - (n))))

#define U8TO32_LITTLE(p) \
  (((u32)((p)[0])      ) | \
   ((u32)((p)[1]) <<  8) | \
   ((u32)((p)[2]) << 16) | \
   ((u32)((p)[3]) << 24))

#define U32TO8_LITTLE(p, v) \
  do { \
    (p)[0] = U8V((v)      ); \
    (p)[1] = U8V((v) >>  8); \
    (p)[2] = U8V((v) >> 16); \
    (p)[3] = U8V((v) >> 24); \
  } while (0)

#define ROTATE(v,c) (ROTL32(v,c))
#define XOR(v,w) ((v) ^ (w))
#define PLUS(v,w) (U32V((v) + (w)))
#define PLUSONE(v) (PLUS((v),1))

#define QUARTERROUND(a,b,c,d) \
  x[a] = PLUS(x[a],x[b]); x[d] = ROTATE(XOR(x[d],x[a]),16); \
  x[c] = PLUS(x[c],x[d]); x[b] = ROTATE(XOR(x[b],x[c]),12); \
  x[a] = PLUS(x[a],x[b]); x[d] = ROTATE(XOR(x[d],x[a]), 8); \
  x[c] = PLUS(x[c],x[d]); x[b] = ROTATE(XOR(x[b],x[c]), 7);

static void chacha20_wordtobyte(u8 output[64],const u32 input[16])
{
  u32 x[16];
  int i;

  for (i = 0;i < 16;++i) x[i] = input[i];
  for (i = 20;i > 0;i -= 2) {
    QUARTERROUND( 0, 4, 8,12)
    QUARTERROUND( 1, 5, 9,13)
    QUARTERROUND( 2, 6,10,14)
    QUARTERROUND( 3, 7,11,15)
    QUARTERROUND( 0, 5,10,15)
    QUARTERROUND( 1, 6,11,12)
    QUARTERROUND( 2, 7, 8,13)
    QUARTERROUND( 3, 4, 9,14)
  }
  for (i = 0;i < 16;++i) x[i] = PLUS(x[i],input[i]);
  for (i = 0;i < 16;++i) U32TO8_LITTLE(output + 4 * i,x[i]);
}

static const char sigma[16] = "expand 32-byte k";

static void chacha20_keysetup(u32 *x,const u8 *k)
{
  x[0] = U8TO32_LITTLE(sigma + 0);
  x[1] = U8TO32_LITTLE(sigma + 4);
  x[2] = U8TO32_LITTLE(sigma + 8);
  x[3] = U8TO32_LITTLE(sigma + 12);
  x[4] = U8TO32_LITTLE(k + 0);
  x[5] = U8TO32_LITTLE(k + 4);
  x[6] = U8TO32_LITTLE(k + 8);
  x[7] = U8TO32_LITTLE(k + 12);
  x[8] = U8TO32_LITTLE(k + 16);
  x[9] = U8TO32_LITTLE(k + 20);
  x[10] = U8TO32_LITTLE(k + 24);
  x[11] = U8TO32_LITTLE(k + 28);
}

static void chacha20_ivsetup(u32 *x,const u8 *iv,u32 ivbits,const u8 *counter)
{
  x[12] = counter ? U8TO32_LITTLE(counter) : 0;
  x[13] = U8TO32_LITTLE(iv + 0);
  x[14] = U8TO32_LITTLE(iv + 4);
  x[15] = U8TO32_LITTLE(iv + 8);
}

static void chacha20_encrypt_bytes(u32 *x,const u8 *m,u8 *c,u64 bytes)
{
  u8 output[64];
  u64 i;

  if (!bytes) return;
  for (;;) {
    chacha20_wordtobyte(output,x);
    x[12] = PLUSONE(x[12]);
    if (!x[12]) {
      x[13] = PLUSONE(x[13]);
      /* stopping at 2^70 bytes per nonce is user's responsibility */
    }
    if (bytes <= 64) {
      for (i = 0;i < bytes;++i) c[i] = m ? m[i] ^ output[i] : output[i];
      return;
    }
    for (i = 0;i < 64;++i) c[i] = m ? m[i] ^ output[i] : output[i];
    bytes -= 64;
    c += 64;
    m += 64;
  }
}

int crypto_stream_chacha20_ietf(u8 *c,const u8 *m,u64 bytes,const u8 *n,u32 ic,const u8 *k)
{
  u32 x[16];
  u8 s[4];

  U32TO8_LITTLE(s,ic);
  chacha20_keysetup(x,k);
  chacha20_ivsetup(x,n,96,s);
  chacha20_encrypt_bytes(x,m,c,bytes);

  return 0;
}
