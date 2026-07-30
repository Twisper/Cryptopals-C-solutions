#ifndef CRYPTO_UTILS_H
#define CRYPTO_UTILS_H

#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>

static const char ascii_lut[] = "0123456789ABCDEF";
static const char base64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

size_t hex2base64(const uint8_t *src, uint8_t *dst, const size_t len);

/*
    There is one simple trick for fast convertation without using if/else or giant lookup tables
    For '0'-'9' (0x30-0x39) 7th bit: 0b0_0_11???? (between underscores) is set to zero. For 'A'-'F' (0x41-0x46)
    or 'a'-'f' (0x61-0x66) 7th bit set to 1. This bit can be used to multiply it by 9 and add to lower 4th bits of character,
    which allows to convert byte to nibble using only maths without branching (This function converts two ASCII bytes into one using same method)
*/
static inline char ascii2byte(uint16_t ascii_byte) {
    return ((ascii_byte & 0xF) + 9 * ((ascii_byte & 0x40) >> 6)) << 4 | 
                (((ascii_byte >> 8) & 0xF) + 9 * (((ascii_byte >> 8) & 0x40) >> 6));
}

static inline uint16_t byte2ascii(char byte) {
    return ((ascii_lut[byte >> 4]) << 8) | (ascii_lut[byte & 0xF]);
}

#endif