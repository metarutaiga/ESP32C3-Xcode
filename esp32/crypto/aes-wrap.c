/*
 * AES Key Wrap Algorithm (RFC3394)
 *
 * Copyright (c) 2003-2007, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "aes.h"


/**
 * aes_wrap - Wrap keys with AES Key Wrap Algorithm (RFC3394)
 * @kek: Key encryption key (KEK)
 * @kek_len: Length of KEK in octets
 * @n: Length of the plaintext key in 64-bit units; e.g., 2 = 128-bit = 16
 * bytes
 * @plain: Plaintext key to be wrapped, n * 64 bits
 * @cipher: Wrapped key, (n + 1) * 64 bits
 * Returns: 0 on success, -1 on failure
 */
int aes_wrap(const u8 *kek, size_t kek_len, int n, const u8 *plain, u8 *cipher)
{
    u8 *a, *r, b[AES_BLOCK_SIZE];
    int i, j;
    unsigned int t;

    a = cipher;
    r = cipher + 8;

    /* 1) Initialize variables. */
    memset(a, 0xa6, 8);
    memcpy(r, plain, 8 * n);

    esp_aes_acquire_hardware();
    aes_hal_setkey(kek, kek_len, ESP_AES_ENCRYPT);

    /* 2) Calculate intermediate values.
     * For j = 0 to 5
     *     For i=1 to n
     *         B = AES(K, A | R[i])
     *         A = MSB(64, B) ^ t where t = (n*j)+i
     *         R[i] = LSB(64, B)
     */
    for (j = 0; j <= 5; j++) {
        r = cipher + 8;
        for (i = 1; i <= n; i++) {
            memcpy(b, a, 8);
            memcpy(b + 8, r, 8);
            aes_hal_transform_block(b, b);
            memcpy(a, b, 8);
            t = n * j + i;
            a[7] ^= t;
            a[6] ^= t >> 8;
            a[5] ^= t >> 16;
            a[4] ^= t >> 24;
            memcpy(r, b + 8, 8);
            r += 8;
        }
    }
    esp_aes_release_hardware();

    /* 3) Output the results.
     *
     * These are already in @cipher due to the location of temporary
     * variables.
     */

    return 0;
}