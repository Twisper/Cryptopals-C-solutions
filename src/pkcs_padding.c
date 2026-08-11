#include "crypto_utils.h"

int main() {

    char test_array[20] = "YELLOW SUBMARINE";

    pkcs7_padding(test_array, 16, 20);

    for (size_t i = 0; i < 16; i++) printf("%c", test_array[i]);
    for (size_t i = 16; i < 20; i++) printf("%2x", test_array[i]);
    printf("\n");

    return 0;
}