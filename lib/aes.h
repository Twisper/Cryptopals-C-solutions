#ifndef AES_H
#define AES_H

#include <stdint.h>
#include <stdlib.h>

size_t aes_ecb_cipher(uint8_t *src, uint8_t *dst, size_t srclen, uint8_t *key, size_t keylen);
size_t aes_ecb_uncipher(uint8_t *src, uint8_t *dst, size_t srclen, uint8_t *key, size_t keylen);

#endif