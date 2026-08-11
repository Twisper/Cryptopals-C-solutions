#include "crypto_utils.h"
#include "aes.h"

int main() {

    FILE *file = fopen("./assets/7.txt", "r");
    if (!file) {
        printf("Error while opening file\n");
        return 1;
    }

    uint8_t aes_key[16] = "YELLOW SUBMARINE";
    char *input = NULL;
    size_t input_len = read_input_stream(&input, file);
    if (!input_len) {
        printf("Error while reading input\n");
        return 1;
    }

    size_t hex_len = input_len / 4 * 3;
    uint8_t *hex_array = (uint8_t *)malloc(hex_len+1);
    if (!hex_array) {
        printf("Error while allocating memory\n");
        return 1;
    }

    base64_2hex((uint8_t *)input, hex_array, input_len);

    aes_ecb_uncipher(hex_array, hex_array, hex_len, aes_key, 16);

    hex_array[hex_len] = '\0';

    printf("%s\n", hex_array);

    return 0;
}