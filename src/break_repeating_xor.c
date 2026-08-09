#include "../lib/crypto_utils.h"

char xor_breaker(uint8_t *buffer, size_t len);

int main() {

    FILE *file = fopen("./assets/6.txt", "r");
    if (!file) {
        printf("Error while reading file\n");
        return 1;
    }

    char *buffer = NULL;
    size_t len;

    len = read_input_stream(&buffer, file);
    if (len == 0) return 1;

    char *hex_buffer;
    size_t hex_len = len / 4 * 3;
    if ((hex_buffer = (char *)malloc(hex_len)) == NULL) return 1;

    base64_2hex((const uint8_t *)buffer, (uint8_t *)hex_buffer, len);

    size_t lowest_keylen = 0;
    float lowest_keylen_score = 999999.0f;

    for (size_t keylen = 2; keylen <= 60; keylen++) {

        size_t curr_step = 4 * keylen;
        size_t hamming_border = hex_len - 2 * keylen;
        float curr_hamming_score = 0;
        size_t pairs_count = 0;

        for (size_t i = 0; i < hamming_border; i += 4 * keylen) {

            for (size_t j = 0; j < curr_step && i + j <= hamming_border; j += keylen) {

                curr_hamming_score += (float)hamming_distance(hex_buffer + i + j, hex_buffer + i + j + keylen, keylen) / keylen;
                pairs_count++;

            }

        }

        curr_hamming_score /= pairs_count;
        if (curr_hamming_score < lowest_keylen_score) {
            lowest_keylen = keylen;
            lowest_keylen_score = curr_hamming_score;
        }

    }

    size_t lines_count = hex_len / lowest_keylen;
    char *temp_buffer = (char *)malloc(lines_count + 1);
    char *xor_key = (char *)malloc(lowest_keylen);

    for (size_t i = 0; i < lowest_keylen; i++) {
        memset(temp_buffer, 0, lines_count);
        size_t curr_lines = 0;
        for (size_t j = 0; j + i < hex_len; j += lowest_keylen) {

            temp_buffer[curr_lines] = hex_buffer[i+j];
            curr_lines += 1;
        }
        xor_key[i] = xor_breaker(temp_buffer, curr_lines);
    }

    xor_array(hex_buffer, xor_key, hex_buffer, hex_len, lowest_keylen);

    printf("%s\n", hex_buffer);

    return 0;

}

char xor_breaker(uint8_t *buffer, size_t len) {

    char *temp_array = (char *)malloc(len+1);
    float curr_score;
    
    float best_score = -999999.0f;
    char best_xor_byte = 0;

    for (uint8_t xor_byte = 1; xor_byte != 0; xor_byte++) {

        xor_array((const uint8_t *)buffer, &xor_byte, (uint8_t *)temp_array, len, 1);

        curr_score = chi_square_oracle((uint8_t *)temp_array, len);

        if (curr_score > best_score) {
            best_score = curr_score;
            best_xor_byte = (char)xor_byte;
        }
    }

    return best_xor_byte;

}