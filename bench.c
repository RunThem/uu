#include "ubench.h"
#include "uu.h"

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

int* make_array(int n) {
  int* array = (int*)malloc(n * sizeof(int));

  for (int i = 0; i < n; i++) {
    array[i] = i;
  }

  shuffle(array, n);

  return array;
}

UBENCH_EX(Dict, at) {
  uu_dict(int, int*) d = uu_dict_init(d, cmp_fn_int);

  size_t i   = 0;
  int* array = make_array(COUNT);

  for (i = 0; i < COUNT; i++) {
    uu_dict_insert(d, array[i], (void*)(intptr_t)array[i]);
  }

  UBENCH_DO_BENCHMARK() {
    shuffle(array, COUNT);

    UBENCH_DO_BENCHMARK_BLOCK({
      for (i = 0; i < COUNT; i++) {
        uu_dict_at(d, array[i]);
      }
    });
  }

  UBENCH_DO_NOTHING(&i);

  free(array);

  uu_dict_deinit(d);
}

UBENCH_EX(Dict, insert) {
  uu_dict(int, int*) d = uu_dict_init(d, cmp_fn_int);

  size_t i   = 0;
  int* array = make_array(COUNT);

  UBENCH_DO_BENCHMARK() {
    shuffle(array, COUNT);

    UBENCH_DO_BENCHMARK_BLOCK({
      for (i = 0; i < COUNT; i++) {
        uu_dict_insert(d, array[i], (void*)(intptr_t)array[i]);
      }
    });

    uu_dict_clear(d);
  }

  UBENCH_DO_NOTHING(&i);

  free(array);

  uu_dict_deinit(d);
}

UBENCH_EX(Dict, remove) {
  uu_dict(int, int*) d = uu_dict_init(d, cmp_fn_int);

  size_t i   = 0;
  int* array = make_array(COUNT);

  UBENCH_DO_BENCHMARK() {
    for (i = 0; i < COUNT; i++) {
      uu_dict_insert(d, i, (void*)(intptr_t)i);
    }

    shuffle(array, COUNT);

    UBENCH_DO_BENCHMARK_BLOCK({
      for (i = 0; i < COUNT; i++) {
        uu_dict_remove(d, array[i]);
      }
    });
  }

  UBENCH_DO_NOTHING(&i);

  free(array);

  uu_dict_deinit(d);
}

UBENCH_EX(Dict, each) {
  uu_dict(int, int*) d = uu_dict_init(d, cmp_fn_int);

  size_t i   = 0;
  int* array = make_array(COUNT);

  for (i = 0; i < COUNT; i++) {
    uu_dict_insert(d, array[i], (void*)(intptr_t)array[i]);
  }

  UBENCH_DO_BENCHMARK() {
    i = 0;

    UBENCH_DO_BENCHMARK_BLOCK({
      uu_dict_each(d, key, int*, val) {
        (void)key;
        (void)val;
        i++;
      };
    });

    assert(i == COUNT);
  }

  UBENCH_DO_NOTHING(&i);

  free(array);

  uu_dict_deinit(d);
}

UBENCH_MAIN();
