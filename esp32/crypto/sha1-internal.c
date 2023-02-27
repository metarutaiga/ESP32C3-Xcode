/*
 * SHA1 hash implementation and interface functions
 * Copyright (c) 2003-2005, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "sha.h"


/**
 * sha1_vector - SHA-1 hash for data vector
 * @num_elem: Number of elements in the data vector
 * @addr: Pointers to the data areas
 * @len: Lengths of the data blocks
 * @mac: Buffer for the hash
 * Returns: 0 on success, -1 of failure
 */
int __wrap_sha1_vector(size_t num_elem, const u8 *addr[], const size_t *len, u8 *mac)
{
    u8 buffer[64];
    size_t i, total = 0;
    bool first_block = true;

    esp_sha_acquire_hardware();
    for (i = 0; i <= num_elem; i++) {
        size_t left = total & 0x3F;
        size_t fill = 64 - left;

        if (i == num_elem) {
            u32 high = (total >> 29);
            u32 low = (total << 3);
            memset(buffer + left, 0, fill);
            buffer[left] = 0x80;
            if (left >= 56) {
                sha_hal_hash_block(SHA1, buffer, 64 / 4, first_block);
                first_block = false;
                memset(buffer, 0, 56);
            }
            buffer[56] = (high >> 24);
            buffer[57] = (high >> 16);
            buffer[58] = (high >>  8);
            buffer[59] = (high      );
            buffer[60] = ( low >> 24);
            buffer[61] = ( low >> 16);
            buffer[62] = ( low >>  8);
            buffer[63] = ( low      );
            sha_hal_hash_block(SHA1, buffer, 64 / 4, first_block);
            first_block = false;
            break;
        }

        const u8* data = addr[i];
        size_t length = len[i];
        total += length;

        if (left && length >= fill) {
            memcpy(buffer + left, data, fill);
            data += fill;
            length -= fill;
            left = 0;
            sha_hal_hash_block(SHA1, buffer, 64 / 4, first_block);
            first_block = false;
        }

        while (length >= 64) {
            sha_hal_hash_block(SHA1, data, 64 / 4, first_block);
            data += 64;
            length -= 64;
            first_block = false;
        }

        if (length > 0) {
            memcpy(buffer + left, data, length);
        }
    }
    sha_hal_read_digest(SHA1, mac);
    esp_sha_release_hardware();
    return 0;
}
