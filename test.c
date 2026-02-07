#define UU_DEBUG

#include "uu.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const int COUNT = 10000;

typedef struct {
  int age;
} user_t;

uu_cmp_fn_def(int, x, y, x - y);

void shuffle(int* array, int len) {
  srand(time(NULL));

  for (int i = len - 1; i > 0; i--) {
    int j = rand() % (i + 1);

    int temp = array[i];
    array[i] = array[j];
    array[j] = temp;
  }
}

void dump_fn_int(const void* _key, const void* _uptr) {
  int key         = *(int*)_key;
  const int* uptr = _uptr;

  printf("[%d] = 0x%x", key, (int)(intptr_t)uptr);
}

void dict() {
  printf("test(Dict<K, V = void*>) {\n");

  int i                = 0;
  int* array           = calloc(sizeof(int), COUNT);
  user_t** udata       = calloc(sizeof(user_t*), COUNT);
  uu_dict(int, int*) d = uu_dict_init(d, cmp_fn_int);

  assert(d);
  assert(uu_dict_is_empty(d));

  for (i = 0; i < COUNT; i++) {
    array[i] = i;
    udata[i] = calloc(sizeof(user_t), 1);
  }

  printf("  env init ok\n");

  shuffle(array, COUNT);

  for (i = 0; i < COUNT; i++) {
    udata[i]->age = array[i] * 3;
    uu_dict_insert(d, array[i], udata[i]);
  }

  assert(COUNT == i);
  assert(COUNT == uu_dict_len(d));
  assert(!uu_dict_is_empty(d));

  printf("  dict.insert ok\n");

  shuffle(array, COUNT);

  for (i = 0; i < COUNT; i++) {
    user_t* uptr = uu_dict_at(d, array[i]);
    assert(array[i] * 3 == uptr->age);
  }

  assert(COUNT == i);
  assert(COUNT == uu_dict_len(d));
  assert(!uu_dict_is_empty(d));
  assert(uu_dict_any_if(d, key, user_t*, uptr, uptr->age == 0));
  assert(!uu_dict_all_if(d, key, user_t*, uptr, uptr->age == 0));

  printf("  dict.at ok\n");

  int sum = (0 + COUNT - 1) * COUNT / 2;
  i       = 0;
  uu_dict_each(d, key, user_t*, uptr) {
    assert(key * 3 == uptr->age);
    i++;
    sum -= key;
  }

  assert(sum == 0);
  assert(COUNT == i);

  printf("  dict.each ok\n");

  shuffle(array, COUNT);

  for (i = 0; i < COUNT; i++) {
    user_t* uptr = uu_dict_remove(d, array[i]);
    assert((array[i] * 3) == uptr->age);
  }

  assert(COUNT == i);
  assert(0 == uu_dict_len(d));
  assert(uu_dict_is_empty(d));

  printf("  dict.remove ok\n");

  uu_dict_deinit(d);

  free(array);

  printf("}\n");

  assert(!d);
}

int main() {

  dict();

  return 0;
}
