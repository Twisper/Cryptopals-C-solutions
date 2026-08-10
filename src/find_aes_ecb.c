#include "../lib/crypto_utils.h"

int main() {

    FILE *file = fopen("./assets/8.txt", "r");
    if (!file) {
        printf("Error while opening file\n");
        return 1;
    }

    char *buffer;
    uint8_t *hex_buffer, *aes_buffer;

    size_t hex_len;
    ssize_t input_len = read_line_stream(&buffer, file);

    if (input_len > 0) {
        hex_len = input_len / 2;
        hex_buffer = (uint8_t *)malloc(hex_len);
        aes_buffer = (uint8_t *)malloc(hex_len);
    }
    memset(aes_buffer, 0, hex_len);

    size_t same_blocks_count = 0;
    size_t aes_same_blocks = 0;
    size_t aes_line;
    size_t curr_line = 1;
    while (input_len > 0) {

        memset(hex_buffer, 0, hex_len);
        ascii_array_2_byte((uint16_t *)buffer, hex_buffer, hex_len);

        same_blocks_count = 0;

        for (size_t i = 0; i < hex_len - 32; i += 16) {
            for (size_t j = i + 16; j < hex_len - 16; j += 16) {
                same_blocks_count += (strncmp(hex_buffer + i, hex_buffer + j, 16) == 0);
            }
        }
        
        if (same_blocks_count > aes_same_blocks) {
            aes_same_blocks = same_blocks_count;
            memcpy(aes_buffer, hex_buffer, hex_len);
            aes_line = curr_line;
        }

        curr_line += 1;
        fseek(file, 1, SEEK_CUR);
        input_len = read_line_stream(&buffer, file);
    }

    printf("line: %d\n", aes_line);
    for (size_t i = 0; i < hex_len; i++) printf("%x", (uint8_t)aes_buffer[i]);
    printf("\n");

    if (input_len == -1) {
        printf("Error while reading current line\n");
        return 1;
    }

    if (input_len == -2) {
        printf("EOF!\n");
        return 0;
    }

    free(buffer);
    free(hex_buffer);
    free(aes_buffer);

    printf("End of line\n");
    return 1;
}