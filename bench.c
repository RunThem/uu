#define UU_DEBUG

#include "ubench.h"
#include "uu.h"

uu_cmp_fn_def(int, x, y, x - y);

UBENCH_EX(Dict, insert) {
  uu_dict(int, int*) d = uu_dict_init(d, cmp_fn_int);

  int i = 0, max = 100000;

  UBENCH_DO_BENCHMARK() {
    for (i = 0; i < max; i++) {
      uu_dict_insert(d, i, (void*)(intptr_t)i);
    }
  }

  UBENCH_DO_NOTHING(&i);

  uu_dict_dump(d, NULL);

  uu_dict_deinit(d);
}

UBENCH_MAIN();
