CFLAGS=-std=c99 -g -Wall -Werror
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

test: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^
	./test

clean:
	rm -f test *.o

.PHONY: test clean
