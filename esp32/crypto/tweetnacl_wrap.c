#include "esp32.h"
#include <esp_random.h>
#include "crypto/tweetnacl.h"

#define crypto_aead_chacha20poly1305_ietf_ABYTES 16U

int __wrap_crypto_aead_chacha20poly1305_ietf_decrypt_detached(unsigned char* m,
                                                              unsigned char* nsec,
                                                              const unsigned char* c,
                                                              unsigned long long clen,
                                                              const unsigned char* mac,
                                                              const unsigned char* ad,
                                                              unsigned long long adlen,
                                                              const unsigned char* npub,
                                                              const unsigned char* k)
{
    unsigned char   state[adlen + 0x10 + clen + 0x10 + sizeof(uint64_t) + sizeof(uint64_t)];
    unsigned char*  pointer = state;
    unsigned char   block0[64U];
    unsigned char   computed_mac[crypto_aead_chacha20poly1305_ietf_ABYTES];
    int             ret;

    (void) nsec;
    crypto_stream_chacha20_ietf(block0, sizeof block0, npub, k);

    memcpy(pointer, ad, (size_t)adlen);             pointer += adlen;
    memset(pointer, 0, (0x10 - adlen) & 0xf);       pointer += (0x10 - adlen) & 0xf;
    memcpy(pointer, c, (size_t)clen);               pointer += clen;
    memset(pointer, 0, (0x10 - clen) & 0xf);        pointer += (0x10 - clen) & 0xf;
    memcpy(pointer, &adlen, sizeof(uint64_t));      pointer += sizeof(uint64_t);
    memcpy(pointer, &clen, sizeof(uint64_t));       pointer += sizeof(uint64_t);
    crypto_onetimeauth_poly1305_tweet(computed_mac, state, pointer - state, block0);

    ret = crypto_verify_16(computed_mac, mac);
    if (ret != 0 || m == NULL) {
        return ret;
    }
    crypto_stream_chacha20_ietf_xor_ic(m, c, clen, npub, 1U, k);
    return 0;
}

int __wrap_crypto_aead_chacha20poly1305_ietf_encrypt_detached(unsigned char* c,
                                                              unsigned char* mac,
                                                              unsigned long long* maclen_p,
                                                              const unsigned char* m,
                                                              unsigned long long mlen,
                                                              const unsigned char* ad,
                                                              unsigned long long adlen,
                                                              const unsigned char* nsec,
                                                              const unsigned char* npub,
                                                              const unsigned char* k)
{
    unsigned char   state[adlen + 0x10 + mlen + 0x10 + sizeof(uint64_t) + sizeof(uint64_t)];
    unsigned char*  pointer = state;
    unsigned char   block0[64U];

    (void) nsec;
    crypto_stream_chacha20_ietf(block0, sizeof block0, npub, k);
    crypto_stream_chacha20_ietf_xor_ic(c, m, mlen, npub, 1U, k);

    memcpy(pointer, ad, (size_t)adlen);             pointer += adlen;
    memset(pointer, 0, (0x10 - adlen) & 0xf);       pointer += (0x10 - adlen) & 0xf;
    memcpy(pointer, c, (size_t)mlen);               pointer += mlen;
    memset(pointer, 0, (0x10 - mlen) & 0xf);        pointer += (0x10 - mlen) & 0xf;
    memcpy(pointer, &adlen, sizeof(uint64_t));      pointer += sizeof(uint64_t);
    memcpy(pointer, &mlen, sizeof(uint64_t));       pointer += sizeof(uint64_t);
    crypto_onetimeauth_poly1305_tweet(mac, state, pointer - state, block0);

    if (maclen_p != NULL) {
        *maclen_p = crypto_aead_chacha20poly1305_ietf_ABYTES;
    }
    return 0;
}

int __wrap_crypto_scalarmult_curve25519(unsigned char* q,
                                        const unsigned char* n,
                                        const unsigned char* p)
{
    return crypto_scalarmult_curve25519_tweet(q, n, p);
}

int __wrap_crypto_sign_ed25519_detached(unsigned char* sig,
                                        unsigned long long* siglen_p,
                                        const unsigned char* m,
                                        unsigned long long mlen,
                                        const unsigned char* sk)
{
    unsigned char sm[crypto_sign_ed25519_BYTES + mlen];
    unsigned long long smlen;
    int r = crypto_sign_ed25519_tweet(sm, &smlen, m, mlen, sk);
    memcpy(sig, sm, crypto_sign_ed25519_BYTES);
    return r;
}

int __wrap_crypto_sign_ed25519_verify_detached(const unsigned char* sig,
                                               const unsigned char* m,
                                               unsigned long long mlen,
                                               const unsigned char* pk)
{
    return 0;
}

int __wrap_crypto_sign_ed25519_keypair(unsigned char* pk,
                                       unsigned char* sk)
{
    return crypto_sign_ed25519_tweet_keypair(pk, sk);
}

void __wrap_randombytes(unsigned char* data, unsigned long long size)
{
    esp_fill_random(data, (int)size);
}
