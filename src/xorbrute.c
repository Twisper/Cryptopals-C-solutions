#include "../lib/crypto_utils.h"

int main() {

    ssize_t len;
    char *buffer = NULL;

    len = read_stdin(&buffer);

    size_t hex_len = len / 2 + (len & 1);

    ascii_array_2_byte((uint16_t *)buffer, (uint8_t *)buffer, hex_len);

    char *temp_array = (char *)malloc(hex_len+1);
    float curr_score;
    
    char *best_array = (char *)malloc(hex_len+1);
    float best_score = -999999.0f;
    char best_xor_byte = 0;

    for (uint8_t xor_byte = 1; xor_byte != 0; xor_byte++) {

        xor_array((const uint8_t *)buffer, &xor_byte, (uint8_t *)temp_array, hex_len, 1);

        curr_score = english_text_oracle((uint8_t *)temp_array, hex_len);

        if (curr_score > best_score) {
            best_score = curr_score;
            memcpy(best_array, temp_array, hex_len);
            best_xor_byte = (char)xor_byte;
        }
    }

    best_array[hex_len] = '\0';
    printf("Best byte: 0x%x, best score: %.4f\n", best_xor_byte, best_score);
    printf("%s\n", best_array);
}