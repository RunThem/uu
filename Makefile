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

test: test_vec test_dict
	$(CC) $(CFLAGS) -o test_vec uu.c test_vec.c && ./test_vec
	$(CC) $(CFLAGS) -o test_dict uu.c test_dict.c && ./test_dict

bench: uu.c bench.c
	$(CC) $(CFLAGS) -o $@ $^ -lm
	@./bench

clean:
	rm -rf test_vec test_dict bench *.o *.dSYM

.PHONY: test bench clean
