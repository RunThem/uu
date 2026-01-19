CFLAGS = -Wall -Wextra -Werror -std=gnu99 -O3 -ggdb

test: uu.c test.c
	$(CC) $(CFLAGS) -DUU_DICT_CHECK -o $@ $^
	@./test

bench: uu.c bench.c
	$(CC) $(CFLAGS) -o $@ $^ -lm
	@./bench

clean:
	rm -rf test bench *.o *.dSYM

.PHONY: test bench clean
