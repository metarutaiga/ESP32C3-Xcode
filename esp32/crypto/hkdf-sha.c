#include "esp32.h"
#include <mbedtls/sha512.h>

#define SHA1Context void
#define SHA224Context void
#define SHA256Context void
#define SHA384Context void
#define SHA512Context void

#define SHA1HashSize 20
#define SHA224HashSize 28
#define SHA256HashSize 32
#define SHA384HashSize 48
#define SHA512HashSize 64

/* SHA-1 */
int SHA1Reset(SHA1Context* context) { return -1; }
int SHA1Input(SHA1Context* context, const uint8_t* bytes, unsigned int bytecount) { return -1; }
int SHA1FinalBits(SHA1Context* context, uint8_t bits, unsigned int bit_count) { return -1; }
int SHA1Result(SHA1Context* context, uint8_t Message_Digest[SHA1HashSize]) { return -1; }

/* SHA-224 */
int SHA224Reset(SHA224Context* context) { return -1; }
int SHA224Input(SHA224Context* context, const uint8_t* bytes, unsigned int bytecount) { return -1; }
int SHA224FinalBits(SHA224Context* context, uint8_t bits, unsigned int bit_count) { return -1; }
int SHA224Result(SHA224Context* context, uint8_t Message_Digest[SHA224HashSize]) { return -1; }

/* SHA-256 */
int SHA256Reset(SHA256Context* context) { return -1; }
int SHA256Input(SHA256Context* context, const uint8_t* bytes, unsigned int bytecount) { return -1; }
int SHA256FinalBits(SHA256Context* context, uint8_t bits, unsigned int bit_count) { return -1; }
int SHA256Result(SHA256Context* context, uint8_t Message_Digest[SHA256HashSize]) { return -1; }

/* SHA-384 */
int SHA384Reset(SHA384Context* context) { return -1; }
int SHA384Input(SHA384Context* context, const uint8_t* bytes, unsigned int bytecount) { return -1; }
int SHA384FinalBits(SHA384Context* context, uint8_t bits, unsigned int bit_count) { return -1; }
int SHA384Result(SHA384Context* context, uint8_t Message_Digest[SHA384HashSize]) { return -1; }

/* SHA-512 */
int SHA512Reset(SHA512Context* context)
{
    mbedtls_sha512_starts(context, 0);
    return 0;
}

int SHA512Input(SHA512Context* context, const uint8_t* bytes, unsigned int bytecount)
{
    mbedtls_sha512_update(context, bytes, bytecount);
    return 0;
}

int SHA512FinalBits(SHA512Context* context, uint8_t bits, unsigned int bit_count)
{
    return -1;
}

int SHA512Result(SHA512Context* context, uint8_t Message_Digest[SHA512HashSize])
{
    mbedtls_sha512_finish(context, Message_Digest);
    return 0;
}
