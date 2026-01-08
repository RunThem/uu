test: uu.c uu.h test.c
	$(CC) -o $@ $^ -Wall -Wextra -Werror -std=c99 -g
	@./test

bench: uu.c uu.h ubench.h bench.c
	$(CC) -o $@ $^ -Wall -Wextra -Werror -std=gnu99 -O3 -lm
	@./bench

clean:
	rm -f test bench *.o

.PHONY: test bench clean
