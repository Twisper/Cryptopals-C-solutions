#include "crypto_utils.h"

int main() {

    ssize_t len;
    char *buffer = NULL;

    if ((len = read_stdin(&buffer)) == -1) {
        return 1;
    }

    size_t hex_len = len / 2 + len % 2;
    size_t base64_len = (len / 3 + len % 3) * 4;

    char *base64_buffer = (char *)malloc(base64_len);
    char *hex_buffer = (char *)calloc(hex_len, 1);

    ascii_array_2_byte((uint16_t *)buffer, hex_buffer, hex_len);

    hex2base64(hex_buffer, base64_buffer, hex_len);

    printf("Resulting Base64: %s\n", base64_buffer);

    free(buffer);
    free(base64_buffer);

    return 0;

}