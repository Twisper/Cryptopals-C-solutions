#include "aes_internal.h"

/**
 * @brief This is internal function for encrypting exactly one Block (128 bit) with different amounts of encryption rounds (10, 12 and 14). 
 * The main idea is to store columns as rows - there are more operations with columns than with rows, so it will also improve L cache performance
 * 
 * @param src pointer to 128-bit Block
 * @param dst where encrypted Block will be written
 * @param round_keys pointer to array, which contains all Round Keys
 * @param round_count count of encryption rounds (depends on Key length)
 */
static void __aes_cipher(uint8_t *src, uint8_t *dst, uint8_t *round_keys, size_t round_count) {

    uint32_t state[4];
    memcpy(state, src, 16);

    aes_add_round_key(state, (uint32_t *)round_keys);

    size_t round;
    for (round = 1; round < round_count; round++) {

        aes_sub_bytes((uint8_t *)state);

        aes_shift_rows((uint8_t *)state);

        aes_mix_columns((uint8_t *)state);

        aes_add_round_key((uint32_t *)state, (uint32_t *)round_keys + (round << 2));
    }

    aes_sub_bytes((uint8_t *)state);
    aes_shift_rows((uint8_t *)state);
    aes_add_round_key((uint32_t *)state, (uint32_t *)round_keys + (round << 2));

    memcpy(dst, state, 16);

}

/**
 * @brief This is internal function for decrypting exactly one Block (128 bit) with different amounts of encryption rounds (10, 12 and 14). 
 * The main idea is to store columns as rows - there are more operations with columns than with rows, so it will also improve L cache performance
 * 
 * @param src pointer to 128-bit Block
 * @param dst where decrypted Block will be written
 * @param round_keys pointer to array, which contains all Round Keys
 * @param round_count count of encryption rounds (depends on Key length)
 */
static void __aes_uncipher(uint8_t *src, uint8_t *dst, uint8_t *round_keys, size_t round_count) {

    uint32_t state[4];
    memcpy(state, src, 16);

    aes_add_round_key(state, (uint32_t *)round_keys + (round_count << 2));

    size_t round;
    for (round = round_count-1; round > 0; round--) {

        aes_inv_shift_rows((uint8_t *)state);

        aes_inv_sub_bytes((uint8_t *)state);

        aes_add_round_key((uint32_t *)state, (uint32_t *)round_keys + (round << 2));

        aes_inv_mix_columns((uint8_t *)state);
    }

    aes_inv_shift_rows((uint8_t *)state);
    aes_inv_sub_bytes((uint8_t *)state);
    aes_add_round_key((uint32_t *)state, (uint32_t *)round_keys);

    memcpy(dst, state, 16);

}

/**
 * @brief This is internal function for generating Round Keys for each column for each encryption/decryption round
 * 
 * @param src pointer to AES cryptographic key
 * @param dst pointer to array, where all Round Keys will be written
 * @param keylen length of cryptographic key in bytes
 * @return amount of rounds. 0 if invalid keylen (not 16, 24 or 32)
 */
static size_t __aes_key_expansion(uint8_t *src, uint8_t *dst, size_t keylen) {

    size_t round_count;
    switch (keylen) {
        case 16:
            round_count = 10;
            break;
        case 24:
            round_count = 12;
            break;
        case 32:
            round_count = 14;
            break;
        default:
            return 0;
    }

    for (size_t i = 0; i < keylen >> 2; i++) {
        *((uint32_t *)dst + i) = *src++ | *src++ << 8 | *src++ << 16 | *src++ << 24;
    }

    size_t word_count = keylen >> 2;
    size_t i = word_count;
    uint32_t *round_keys = (uint32_t *)dst;
    uint32_t temp;
    while (i < 4 * (round_count + 1)) {

        temp = round_keys[i-1];
        if (i % word_count == 0) {

            temp = aes_sub_word(__rord(temp, 8)) ^ Rcon[i/word_count];

        } else if (keylen == 32 && i % 8 == 4) {
            temp = aes_sub_word(temp);
        }
        round_keys[i] = round_keys[i-word_count] ^ temp;
        i += 1;
    }

    return round_count;
}

/**
 * @brief This function is a wrapper. It creates simple API for AES encryption, requiring only text and key for encryption without calling other functions and creating mess in code
 * 
 * @param src from where data will be read for encryption
 * @param dst where ciphertext will be written
 * @param srclen how many bytes will be encrypted (should be divisible by 16)
 * @param key pointer to cipher key
 * @param keylen length of key in bytes (Either 16, 24 or 32)
 * @return 0 if success, 1 if error
 */
size_t aes_ecb_cipher(uint8_t *src, uint8_t *dst, size_t srclen, uint8_t *key, size_t keylen) {

    if (srclen % 16 != 0) return 1;

    uint32_t aes_round_keys[60];
    size_t round_count = __aes_key_expansion(key, (uint8_t *)aes_round_keys, keylen);
    if (round_count == 0) return 1;

    for (size_t i = 0; i < srclen; i += 16)
        __aes_cipher(src + i, dst + i, (uint8_t *)aes_round_keys, round_count);

    return 0;

}

/**
 * @brief This function is a wrapper. It creates simple API for AES decryption, requiring only text and key for encryption without calling other functions and creating mess in code
 * 
 * @param src from where ciphertext will be read for encryption
 * @param dst where decrypted data will be written
 * @param srclen how many bytes will be decrypted (should be divisible by 16)
 * @param key pointer to cipher key
 * @param keylen length of cipher key in bytes (Either 16, 24 or 32)
 * @return 0 if success, 1 if error
 */
size_t aes_ecb_uncipher(uint8_t *src, uint8_t *dst, size_t srclen, uint8_t *key, size_t keylen) {

    if (srclen % 16 != 0) return 1;

    uint32_t aes_round_keys[60];
    size_t round_count = __aes_key_expansion(key, (uint8_t *)aes_round_keys, keylen);
    if (round_count == 0) return 1;

    for (size_t i = 0; i < srclen; i += 16)
        __aes_uncipher(src + i, dst + i, (uint8_t *)aes_round_keys, round_count);

    return 0;
}