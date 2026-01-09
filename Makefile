CFLAGS = -Wall -Wextra -Werror -std=gnu99 -O3 -ggdb
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

test: uu.o test.o
	$(CC) $(CFLAGS) -DUU_DICT_CHECK -o $@ $^
	@./test

bench: uu.o bench.o
	$(CC) $(CFLAGS) -lm -o $@ $^
	@./bench

clean:
	rm -rf test bench *.o *.dSYM

.PHONY: test bench clean
