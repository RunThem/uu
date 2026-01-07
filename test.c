#define UU_DEBUG

#include "uu.h"

#include <assert.h>
#include <stdio.h>

int cmp_fn_int(const void* x, const void* y) {
  int a = *(int*)x;
  int b = *(int*)y;

  return a - b;
}

void dump_fn_int(const void* _key, const void* _uptr) {
  int key         = *(int*)_key;
  const int* uptr = _uptr;

  printf("[%d] = 0x%x", key, (int)(intptr_t)uptr);
}

int main() {
  {
    uu_vec(int) v = uu_vec_init(v);

    assert(uu_vec_is_empty(v));

    uu_vec_insert_tail(v, 1);
    uu_vec_insert_tail(v, 2);
    uu_vec_insert_tail(v, 3);
    uu_vec_insert_tail(v, 4);
    uu_vec_insert_tail(v, 5);
    uu_vec_insert_tail(v, 6);
    uu_vec_insert_tail(v, 7);

    assert(7 == uu_dict_len(v));
    assert(!uu_dict_is_empty(v));

    uu_vec_remove_head(v);
    uu_vec_remove_tail(v);
    uu_vec_remove(v, 2);

    uu_vec_deinit(v, { printf("[%d]\n", it); });
  }

  {
    uu_dict(int, int*) d = uu_dict_init(d, cmp_fn_int);

    assert(uu_dict_is_empty(d));

    uu_dict_insert(d, 1, (void*)(intptr_t)0x1);
    uu_dict_insert(d, 2, (void*)(intptr_t)0x2);
    uu_dict_insert(d, 3, (void*)(intptr_t)0x3);
    uu_dict_insert(d, 4, (void*)(intptr_t)0x4);
    uu_dict_insert(d, 5, (void*)(intptr_t)0x5);
    uu_dict_insert(d, 6, (void*)(intptr_t)0x6);
    uu_dict_insert(d, 7, (void*)(intptr_t)0x7);

    assert(7 == uu_dict_len(d));
    assert(!uu_dict_is_empty(d));

    uu_dict_dump(d, dump_fn_int);

    uu_dict_remove(d, 3);

    uu_dict_deinit(d, { printf("[%d] = 0x%x\n", key, (int)(intptr_t)uptr); });
  }

  return 0;
}
