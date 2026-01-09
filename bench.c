#define UU_DEBUG

#include "ubench.h"
#include "uu.h"

#include <assert.h>
#include <time.h>

const int COUNT = 100000;

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

UBENCH_EX(Dict, at) {
  uu_dict(int, int*) d = uu_dict_init(d, cmp_fn_int);

  int i      = 0;
  int* array = (int*)malloc(COUNT * sizeof(int));

  for (i = 0; i < COUNT; i++) {
    array[i] = i;
  }

  shuffle(array, COUNT);

  UBENCH_DO_BENCHMARK() {
    for (i = 0; i < COUNT; i++) {
      uu_dict_at(d, array[i]);
    }
  }

  assert(i == COUNT);

  UBENCH_DO_NOTHING(&i);

  free(array);

  uu_dict_deinit(d);
}

UBENCH_EX(Dict, insert) {
  uu_dict(int, int*) d = uu_dict_init(d, cmp_fn_int);

  int i      = 0;
  int* array = (int*)malloc(COUNT * sizeof(int));

  for (i = 0; i < COUNT; i++) {
    array[i] = i;
  }

  shuffle(array, COUNT);

  UBENCH_DO_BENCHMARK() {
    for (i = 0; i < COUNT; i++) {
      uu_dict_insert(d, array[i], (void*)(intptr_t)array[i]);
    }
  }

  assert(i == COUNT);

  UBENCH_DO_NOTHING(&i);

  free(array);

  uu_dict_deinit(d);
}

UBENCH_EX(Dict, remove) {
  uu_dict(int, int*) d = uu_dict_init(d, cmp_fn_int);

  int i      = 0;
  int* array = (int*)malloc(COUNT * sizeof(int));

  for (i = 0; i < COUNT; i++) {
    array[i] = i;
  }

  shuffle(array, COUNT);

  for (i = 0; i < COUNT; i++) {
    uu_dict_insert(d, i, (void*)(intptr_t)i);
  }

  shuffle(array, COUNT);

  UBENCH_DO_BENCHMARK() {
    for (i = 0; i < COUNT; i++) {
      uu_dict_remove(d, array[i]);
    }
  }

  assert(i == COUNT);

  free(array);

  uu_dict_deinit(d);
}

UBENCH_MAIN();
