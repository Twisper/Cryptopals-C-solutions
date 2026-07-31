#include "../lib/crypto_utils.h"

int main() {

    char *buffer_first = NULL;
    char *buffer_second = NULL;
    ssize_t len1, len2;

    len1 = read_line_stream(&buffer_first, stdin);
    len2 = read_line_stream(&buffer_second, stdin);

    ascii_array_2_byte((uint16_t *)buffer_first, (uint8_t *)buffer_first, len1 / 2 + (len1 & 1));
    ascii_array_2_byte((uint16_t *)buffer_second, (uint8_t *)buffer_second, len2 / 2 + (len2 & 1));

    xor_array((uint8_t *)buffer_first, (uint8_t *)buffer_second, (uint8_t *)buffer_first, len1 / 2 + (len1 & 1), len2 / 2 + (len2 & 1));

    char *print_array = (char *)malloc(len1);

    byte_array_2_ascii((uint8_t *)buffer_first, (uint16_t *)print_array, len1 / 2 + (len1 & 1));

    printf("%s\n", print_array);

    free(print_array);
    free(buffer_first);
    free(buffer_second);

    return 0;
}