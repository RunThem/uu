CFLAGS = -Wall -Wextra -Werror -std=gnu99 -O3 -ggdb

MIMALLOC = 

ifneq ($(MIMALLOC), )
	CFLAGS += -include $(MIMALLOC)/include/mimalloc.h
	CFLAGS += -L $(MIMALLOC)/lib -lmimalloc
	CFLAGS += -DUU_MEMORY
	CFLAGS += -D 'UU_MALLOC(size)=mi_malloc(size)'
	CFLAGS += -D 'UU_REALLOC(ptr, size)=mi_realloc(ptr, size)'
	CFLAGS += -D 'UU_FREE(ptr)=mi_free(ptr)'
endif

test: uu.c test.c
	$(CC) $(CFLAGS) -DUU_DICT_CHECK -o $@ $^
	@./test

bench: uu.c bench.c
	$(CC) $(CFLAGS) -o $@ $^ -lm
	@./bench

clean:
	rm -rf test bench *.o *.dSYM

.PHONY: test bench clean
