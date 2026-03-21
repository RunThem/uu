CFLAGS = -Wall -Wextra -Werror -std=gnu99 -O3 -ggdb

LIBS_MIMALLOC_PATH =

ifneq ($(LIBS_MIMALLOC_PATH), )
	LIBS_MIMALLOC_CFLAGS += -include $(LIBS_MIMALLOC_PATH)/include/mimalloc.h
	LIBS_MIMALLOC_CFLAGS += -L $(LIBS_MIMALLOC_PATH)/lib -lmimalloc
	LIBS_MIMALLOC_CFLAGS += -DUU_MEMORY
	LIBS_MIMALLOC_CFLAGS += -D 'UU_MALLOC(size)=mi_malloc(size)'
	LIBS_MIMALLOC_CFLAGS += -D 'UU_REALLOC(ptr, size)=mi_realloc(ptr, size)'
	LIBS_MIMALLOC_CFLAGS += -D 'UU_FREE(ptr)=mi_free(ptr)'
endif

test: test_vec test_dict

test_vec: uu.c test_vec.c
	$(CC) $(CFLAGS) -DUU_DICT_CHECK -o $@ $^ $(LIBS_MIMALLOC_CFLAGS)
	@./$@

test_tree: uu.c test_tree.c
	$(CC) $(CFLAGS) -DUU_DICT_CHECK -o $@ $^ $(LIBS_MIMALLOC_CFLAGS)
	@./$@

test_dict: uu.c test_dict.c
	$(CC) $(CFLAGS) -DUU_DICT_CHECK -o $@ $^ $(LIBS_MIMALLOC_CFLAGS)
	@./$@

bench: uu.c bench.c
	$(CC) $(CFLAGS) -o $@ $^ -lm $(LIBS_MIMALLOC_CFLAGS)
	@./$@

clean:
	rm -rf test_vec test_tree test_dict bench *.o *.dSYM

.PHONY: test bench clean
