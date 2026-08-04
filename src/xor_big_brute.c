#include "../lib/crypto_utils.h"

int main() {

    FILE *file = fopen("./assets/4.txt", "r");
    if (!file) {
        printf("Error while reading file\n");
        return 1;
    }

    char buffer[61];
    char hex_buffer[31];
    memset(buffer, 0, 61);

    char *temp_array = (char *)malloc(31);
    char *best_array = (char *)malloc(31);
    
    float best_score = -999999.0f;
    char best_xor_byte = 0;

    size_t best_iter = 0;
    size_t curr_iter = 0;

    while (fgets(buffer, 61, file) != NULL) {

        size_t len = strlen(buffer);
        size_t hex_len = len / 2 + (len & 1);

        ascii_array_2_byte((uint16_t *)buffer, (uint8_t *)hex_buffer, hex_len);

        memset(temp_array, 0, 31);
        float curr_score;

        for (uint8_t xor_byte = 1; xor_byte != 0; xor_byte++) {

            xor_array((const uint8_t *)hex_buffer, &xor_byte, (uint8_t *)temp_array, hex_len, 1);

            curr_score = bigram_text_oracle((uint8_t *)temp_array, hex_len);

            if (curr_score > best_score) {
                best_score = curr_score;
                memcpy(best_array, temp_array, hex_len);
                best_xor_byte = (char)xor_byte;
                best_iter = curr_iter;
            }
        }

        curr_iter += 1;
    }

    best_array[30] = '\0';
    printf("Best byte: 0x%x, best score: %.4f, best line: %d\n", best_xor_byte, best_score, best_iter);
    printf("%s", best_array);

    fclose(file);
    return 0;

}