CC ?= gcc

CFLAGS := -Wall -Wextra -O3
CFLAGS-DEBUG := $(CC-FLAGS) -g

all:
	@echo "Use specific rule with file to build"

clean:
	rm -f ./build/*.out

%: ./src/%.c ./lib/crypto_utils.c
	$(CC) $< ./lib/crypto_utils.c -o ./build/$@.out $(CFLAGS)

debug-%: ./src/%.c ./lib/crypto_utils.c
	$(CC) $< -o ./build/debug-$@.out $(CFLAGS-DEBUG)

aes-%: ./src/%.c ./lib/aes.c ./lib/crypto_utils.c
	$(CC) $< ./lib/aes.c ./lib/crypto_utils.c -o ./build/$@.out $(CFLAGS)
	