#include "crypto_utils.h"

int main() {

    char key[3] = "ICE";

    char *buffer = NULL;
    size_t len;

    len = read_input_stream(&buffer, stdin);

    xor_array((uint8_t *)buffer, (uint8_t *)key, (uint8_t *)buffer, len, 3);

    char *print_array1 = (char *)malloc(len << 1);

    byte_array_2_ascii((uint8_t *)buffer, (uint16_t *)print_array1, len);

    printf("%s\n", print_array1);

    free(print_array1);
    free(buffer);
}