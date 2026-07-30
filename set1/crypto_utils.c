#include "crypto_utils.h"
/**
 * @brief This function converts array of hex'es to Base64 in ASCII format
 * 
 * @param src array of uint8_t (hex) elements 
 * @param dst array of ascii bytes
 * @param len length of encoded array
 * @return length of Base64 encoded array
 */
size_t hex2base64(const uint8_t *src, uint8_t *dst, const size_t len) {

    size_t i = 0;
    size_t base64_index = 0;
    if (len >= 3) {
        for (i = 0; i < len; i += 3) {
            uint32_t iter_bytes = src[i] << 16 | src[i+1] << 8 | src[i+2];
            *dst++ = base64[iter_bytes >> 18 & 0x3F];
            *dst++ = base64[iter_bytes >> 12 & 0x3F];
            *dst++ = base64[iter_bytes >> 6 & 0x3F];
            *dst++ = base64[iter_bytes & 0x3F];
            base64_index += 4;
        }
    }

    if (len - i == 2) {
        uint32_t iter_bytes = src[i] << 16 | src[i+1] << 8;
        *dst++ = base64[iter_bytes >> 18 & 0x3F];
        *dst++ = base64[iter_bytes >> 12 & 0x3F];
        *dst++ = base64[iter_bytes >> 6 & 0x3F];
        *dst++ = '=';
        base64_index += 4;
    } else if (len - i == 1) {
        uint32_t iter_bytes = src[i] << 16;
        *dst++ = base64[iter_bytes >> 18 & 0x3F];
        *dst++ = base64[iter_bytes >> 12 & 0x3F];
        *dst++ = '=';
        *dst++ = '=';
        base64_index += 4;
    }
    *dst = '\0';

    return base64_index;

}

/**
 * @brief This function finds result of XOR operation between two arrays
 * 
 * @param src1 first operand
 * @param src2 second operand
 * @param dst array, where result will be written
 * @param len how many bytes will be XOR'ed
 */
void xor2arrays(const uint8_t *src1, const uint8_t *src2, uint8_t *dst, const size_t len) {
    for (size_t i = 0; i < len; i++) {
        *dst++ = *src1++ ^ *src2++;
    }
}


/**
 * @brief This function reads input from stdin and returns length of this input.
 * Memory used by this function must be freed after use
 * 
 * @param buffer pointer to pointer of string array
 * @return length of input string
 */
ssize_t read_stdin(char **buffer) {

    size_t size = 0;
    ssize_t len;

    len = getline(buffer, &size, stdin);

    if (len != -1) {
        if ((*buffer)[len - 1] == '\n') {
            (*buffer)[len - 1] = '\0';
            len -= 1;
        }
    } else {
        free(*buffer);
        return -1;
    }

    return len;
}