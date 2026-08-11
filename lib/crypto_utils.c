#include "crypto_utils.h"
/**
 * @brief This function converts array of hex'es to Base64 in ASCII format
 * 
 * @param src array of uint8_t (hex) elements 
 * @param dst array of ascii bytes
 * @param len length of encoded array
 */
void hex2base64(const uint8_t *src, uint8_t *dst, const size_t len) {

    size_t i = 0;
    if (len >= 3) {
        for (i = 0; i < len; i += 3) {
            uint32_t iter_bytes = src[i] << 16 | src[i+1] << 8 | src[i+2];
            *dst++ = base64[iter_bytes >> 18 & 0x3F];
            *dst++ = base64[iter_bytes >> 12 & 0x3F];
            *dst++ = base64[iter_bytes >> 6 & 0x3F];
            *dst++ = base64[iter_bytes & 0x3F];
        }
    }

    if (len - i == 2) {
        uint32_t iter_bytes = src[i] << 16 | src[i+1] << 8;
        *dst++ = base64[iter_bytes >> 18 & 0x3F];
        *dst++ = base64[iter_bytes >> 12 & 0x3F];
        *dst++ = base64[iter_bytes >> 6 & 0x3F];
        *dst++ = '=';
    } else if (len - i == 1) {
        uint32_t iter_bytes = src[i] << 16;
        *dst++ = base64[iter_bytes >> 18 & 0x3F];
        *dst++ = base64[iter_bytes >> 12 & 0x3F];
        *dst++ = '=';
        *dst++ = '=';
    }
    *dst = '\0';

}

//len is size of base64 buffer
void base64_2hex(const uint8_t *src, uint8_t *dst, const size_t len) {

    size_t i = 0;
    while (i < len) {
        uint32_t three_bytes = base64_2_hex_lut[*src++] << 18 | (base64_2_hex_lut[*src++] << 12) | (base64_2_hex_lut[*src++] << 6) | (base64_2_hex_lut[*src++]);
        *dst++ = (three_bytes >> 16) & 0xFF;
        *dst++ = (three_bytes >> 8) & 0xFF;
        *dst++ = (three_bytes) & 0xFF;
        i += 4;
    }

}

/**
 * @brief This function finds result of XOR operation between array and key
 * 
 * @param src1 first operand
 * @param key key for XOR operation
 * @param dst array, where result will be written
 * @param len how many bytes will be XOR'ed
 * @param keylen length of key
 */
void xor_array(const uint8_t *src, uint8_t *key, uint8_t *dst, const size_t len, const size_t keylen) {
    for (size_t i = 0; i < len; i += keylen) {

        int remainder = (len - i > keylen) ? keylen : len-i;

        for (int j = 0; j < remainder; j++) {
            *dst++ = *src++ ^ key[j];
        }
    }
}


/**
 * @brief This function reads one line from specific stream and returns length of this input.
 * Memory used by this function must be freed after use
 * 
 * @param buffer pointer to pointer of string array
 * @param stream from where input will be received
 * @return length of input, -1 if error or -2 if EOF
 */
ssize_t read_line_stream(char **buffer, FILE *stream) {

    size_t size = 0;
    ssize_t len;

    len = getline(buffer, &size, stream);

    if (len != -1) {
        if ((*buffer)[len - 1] == '\n') {
            (*buffer)[len - 1] = '\0';
            len -= 1;
        }
    } else {
        free(*buffer);
        if (!feof(stream)) 
            return -1;
        else
            return -2;
    }

    return len;
}

/**
 * @brief This function reads an entire input to EOF, allocating memory and returning pointer to it with length of input.
 * Memory used by this function must be freed after use
 * 
 * @param buffer pointer to pointer of buffer
 * @param stream from where input will be received
 * @return total length of input
 */
size_t read_input_stream(char **buffer, FILE *stream) {
    size_t curr_input_size;
    size_t curr_buf_size = 1024;
    size_t input_size = 0;

    char *stdin_buf;
    char *new_buf;
    if ((stdin_buf = (char *)malloc(curr_buf_size + 1)) == NULL) {
        return 0;
    }

    while ((curr_input_size = fread(stdin_buf + input_size, sizeof(char), 1024, stream)) > 0) {
        input_size += curr_input_size;
        if (curr_buf_size - input_size < 1024) {
            new_buf = (char *)realloc(stdin_buf, curr_buf_size + 1024 + 1);
            if (new_buf == NULL) {
                free(stdin_buf);
                return 0;
            }
            curr_buf_size += 1024;
            stdin_buf = new_buf;
        }
    }

    if (input_size != 0) {
        *buffer = stdin_buf;
        if (stdin_buf[input_size-1] == '\n') {
            stdin_buf[input_size-1] = '\0';
            input_size -= 1;
        }
        stdin_buf[input_size] = '\0';
    } else {
        free(stdin_buf);
        return 0;
    }
    
    return input_size;
}

/**
 * @brief This function counts logarithmic probability of given array, to be close to normal English text, using unigrams method with Chi-Square test.
 * This is nessesary to check decrypted string automatically in order to find answer (if it is known, that English text was encrypted)
 * 
 * @param src array of ascii symbols
 * @param len length of array
 * @return normalised logarithmic probability of given string of being English
 */
float chi_square_oracle(uint8_t *src, size_t len) {

    if (len == 0) return -999999.0f;

    float score = 0;

    for (size_t i = 0; i < len; i++) {

        int curr_index = ascii2alpha(src[i]);

        if (curr_index == -1) return -999999.0f;

        if (curr_index == 27)
            score += -5.0f;
        else
            score += unigram_log_probs[curr_index];

    }

    return score / len;
}

/**
 * @brief This function counts logarithmic probability of given array, to be close to normal English text, using bigrams method.
 * This is nessesary to check decrypted string automatically in order to find answer (if it is known, that English text was encrypted)
 * 
 * @param src array of ascii symbols
 * @param len length of array
 * @return normalised logarithmic probability of given string of being English
 */
float bigram_text_oracle(uint8_t *src, size_t len) {

    if (len < 2) return -999999.0f;

    float score = 0;

    int first_bigram_letter = ascii2alpha(src[0]);

    if (first_bigram_letter == -1) return -999999.0f;

    size_t i;
    for (i = 1; i < len; i++) {

        int second_bigram_letter = ascii2alpha(src[i]);

        if (second_bigram_letter == -1) return -999999.0f;

        if (first_bigram_letter == 27 || second_bigram_letter == 27) {
            score += -15.0f;
        } else {
            score += bigram_log_probs[first_bigram_letter][second_bigram_letter];
        }

        first_bigram_letter = second_bigram_letter;

    }

    return score / (len - 1);

}

/**
 * @brief this function calculates Hamming distance (how many bits differ) betweem two hex arrays using __builtin_popcount intrinsic
 * 
 * @param src1 first array-operand
 * @param src2 second array-operand
 * @param len1 amount of symbols from first operand
 * @param len2 amount of symbols from second operand
 * @return hamming distance between two hex arrays
 */
int hamming_distance(const uint8_t *src1, const uint8_t *src2, const size_t len) {

    size_t distance = 0;

    for (size_t i = 0; i < len; i++) {
        distance += __builtin_popcount((uint32_t)(src1[i] ^ src2[i]));
        // uint8_t curr_byte = temp_array[i];
        // for (size_t j = 0; j < 8; j++) {
        //     distance += (curr_byte & 1);
        //     curr_byte >>= 1;
        // }
    }

    return distance;

}

void pkcs7_padding(uint8_t *src, size_t len, size_t new_len) {

    uint8_t padding_byte = new_len - len;

    for (size_t i = len; i < new_len; i++) {
        src[i] = padding_byte;
    }

}