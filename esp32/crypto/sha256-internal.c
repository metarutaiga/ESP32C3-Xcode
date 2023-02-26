/*
 * SHA-256 hash implementation and interface functions
 * Copyright (c) 2003-2011, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "sha.h"


/**
 * sha256_vector - SHA256 hash for data vector
 * @num_elem: Number of elements in the data vector
 * @addr: Pointers to the data areas
 * @len: Lengths of the data blocks
 * @mac: Buffer for the hash
 * Returns: 0 on success, -1 of failure
 */
int __wrap_sha256_vector(size_t num_elem, const u8 *addr[], const size_t *len, u8 *mac)
{
    size_t i;

    esp_sha_acquire_hardware();
    for (i = 0; i < num_elem; i++)
        sha_hal_hash_block(SHA2_256, addr[i], len[i] / 4, i == 0);
    sha_hal_read_digest(SHA2_256, mac);
    esp_sha_release_hardware();
    return 0;
}
