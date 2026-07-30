#include <stdio.h>
#include <stdlib.h>
#include "crypto_utils.h"

int main() {

    char *buffer = NULL;
    size_t size = 0;
    ssize_t len;

    len = getline(&buffer, &size, stdin);

    if (len != -1) {
        if (buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
            len -= 1;
        }
    } else {
        return 1;
    }

    size_t hex_len = len / 2 + len % 2;
    size_t base64_len = (len / 3 + len % 3) * 4;

    char *base64_buffer = (char *)malloc(base64_len);
    char *hex_buffer = (char *)calloc(hex_len, 1);
    
    for (size_t i = 0; i < len; i += 2) {
        hex_buffer[i >> 1] = ascii2byte(*(uint16_t *)(buffer+i));
    }

    hex2base64(hex_buffer, base64_buffer, hex_len);

    printf("Resulting Base64: %s\n", base64_buffer);

    free(buffer);
    free(base64_buffer);

    return 0;

}