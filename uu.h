#ifndef __UU_H__
#define __UU_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>
#include <stdint.h>
#include <string.h>

#ifndef UU_MEMORY
#  include <stdlib.h>
#  define UU_MALLOC(size)       malloc(size)
#  define UU_REALLOC(ptr, size) realloc(ptr, size)
#  define UU_FREE(ptr)          free(ptr)
#endif

/* clang-format off */
typedef int      (*uu_cmp_fn)  (const void* x, const void* y);
typedef uint32_t (*uu_hash_fn) (const void* data, uint32_t len, uint32_t seed);

extern uint32_t uu_hash_fn_fnv1a(const void*, uint32_t, uint32_t);

#define uu_cmp_fn_def(name, type, x, y, ...)                                                       \
  static inline int uu_cmp_fn_##name(const void* _##x, const void* _##y) {                         \
    type x = *(type*)_##x;                                                                         \
    type y = *(type*)_##y;                                                                         \
                                                                                                   \
    return __VA_ARGS__;                                                                            \
  }

#define uu_hash_fn_def(name, type, data, ...)                                                      \
  static inline uint32_t uu_hash_fn_##name(const void* _##data, uint32_t len, uint32_t seed) {     \
    type data = *(type*)_##data;                                                                   \
                                                                                                   \
    (void)len;                                                                                     \
    (void)seed;                                                                                    \
                                                                                                   \
    return __VA_ARGS__;                                                                            \
  }

uu_cmp_fn_def(char,   char,     x, y, x - y);
uu_cmp_fn_def(int,    int,      x, y, x - y);
uu_cmp_fn_def(int8,   int8_t,   x, y, x - y);
uu_cmp_fn_def(uint8,  uint8_t,  x, y, x - y);
uu_cmp_fn_def(int16,  int16_t,  x, y, x - y);
uu_cmp_fn_def(uint16, uint16_t, x, y, x - y);
uu_cmp_fn_def(int32,  int32_t,  x, y, x - y);
uu_cmp_fn_def(uint32, uint32_t, x, y, x - y);
uu_cmp_fn_def(int64,  int64_t,  x, y, x - y);
uu_cmp_fn_def(uint64, uint64_t, x, y, x - y);

uu_cmp_fn_def(cstr,  char*, x, y, strcmp(x, y));
uu_hash_fn_def(cstr, char*, data, uu_hash_fn_fnv1a(data, strlen(data), 0));
/* clang-format on */

/***************************************************************************************************
 * Vec
 **************************************************************************************************/
/**
 * ::Class Vec<T>
 */
#define uu_vec(T) __typeof__(T*)

/**
 * ::Vec<T>::init(self) -> Self
 *
 * ```c
  {
    uu_vec(int) v = uu_vec_init(v);
    assert(v);
  }
  {
    uu_vec(int*) v = NULL;

    v = uu_vec_init(v);
    assert(v);
  }
 * ```
 */
#define uu_vec_init(self)                                                                          \
  ({                                                                                               \
    extern void* __uu_vec_init(uint32_t);                                                          \
                                                                                                   \
    self = (__typeof__(self))__uu_vec_init(sizeof(*self));                                         \
                                                                                                   \
    self;                                                                                          \
  })

/**
 * ::Vec<T>::clear(self) -> !
 * ::Vec<T>::clear(self, ...) -> !
 *
 * ```c
  {
    uu_vec(int) v = uu_vec_init(v);

    uu_vec_insert_tail(v, 0); // {0}
    assert(1 == uu_vec_len(v));

    uu_vec_clear(v);
    assert(v);
    assert(0 == uu_vec_len(v));
  }
  {
    uu_vec(int*) v = uu_vec_init(v);

    uu_vec_insert_tail(v, malloc(sizeof(int))); // {0xptr}
    assert(1 == uu_vec_len(v));

    uu_vec_clear(v, { free(it); });
    assert(v);
    assert(0 == uu_vec_len(v));
  }
 * ```
 */
#define uu_vec_clear(self, ...)                                                                    \
  do {                                                                                             \
    extern void __uu_vec_clear(void*);                                                             \
                                                                                                   \
    {                                                                                              \
      __typeof__(self) Self = self;                                                                \
      void* nil             = NULL;                                                                \
      assert(Self != nil);                                                                         \
    }                                                                                              \
                                                                                                   \
    uu_vec_each(self, it) {                                                                        \
      __VA_ARGS__;                                                                                 \
                                                                                                   \
      (void)it;                                                                                    \
    };                                                                                             \
                                                                                                   \
    __uu_vec_clear((void*)self);                                                                   \
  } while (0)

/**
 * ::Vec<T>::deinit(self) -> !
 * ::Vec<T>::deinit(self, ...) -> !
 *
 * ```c
  {
    uu_vec(int) v = uu_vec_init(v);

    uu_vec_insert_tail(v, 0); // {0}
    assert(1 == uu_vec_len(v));

    uu_vec_deinit(v);
    assert(!v);
  }
  {
    uu_vec(int*) v = uu_vec_init(v);

    uu_vec_insert_tail(v, malloc(sizeof(int))); // {0xptr}
    assert(1 == uu_vec_len(v));

    uu_vec_deinit(v, { free(it); });
    assert(!v);
  }
 * ```
 */
#define uu_vec_deinit(self, ...)                                                                   \
  do {                                                                                             \
    extern void __uu_vec_deinit(void*);                                                            \
                                                                                                   \
    {                                                                                              \
      __typeof__(self) Self = self;                                                                \
      void* nil             = NULL;                                                                \
      assert(Self != nil);                                                                         \
    }                                                                                              \
                                                                                                   \
    uu_vec_each(self, it) {                                                                        \
      __VA_ARGS__;                                                                                 \
                                                                                                   \
      (void)it;                                                                                    \
    };                                                                                             \
                                                                                                   \
    __uu_vec_deinit((void*)self);                                                                  \
                                                                                                   \
    self = NULL;                                                                                   \
  } while (0)

/**
 * ::Vec<T>::len(self) -> uint32_t
 *
 * ```c
  {
    uu_vec(int) v = uu_vec_init(v);
    assert(0 == uu_vec_len(v));

    uu_vec_insert_tail(v, 0); // {0}
    assert(1 == uu_vec_len(v));
  }
 * ```
 */
#define uu_vec_len(self)                                                                           \
  ({                                                                                               \
    extern uint32_t __uu_vec_len(void*);                                                           \
                                                                                                   \
    {                                                                                              \
      __typeof__(self) Self = self;                                                                \
      void* nil             = NULL;                                                                \
      assert(Self != nil);                                                                         \
    }                                                                                              \
                                                                                                   \
    __uu_vec_len((void*)self);                                                                     \
  })

/**
 * ::Vec<T>::is_empty(self) -> bool
 *
 * ```c
  {
    uu_vec(int) v = uu_vec_init(v);
    assert(uu_vec_is_empty(v));

    uu_vec_insert_tail(v, 0); // {0}
    assert(!uu_vec_is_empty(v));
  }
 * ```
 */
#define uu_vec_is_empty(self)                                                                      \
  ({                                                                                               \
    extern uint32_t __uu_vec_len(void*);                                                           \
                                                                                                   \
    {                                                                                              \
      __typeof__(self) Self = self;                                                                \
      void* nil             = NULL;                                                                \
      assert(Self != nil);                                                                         \
    }                                                                                              \
                                                                                                   \
    0 == __uu_vec_len((void*)self);                                                                \
  })

/**
 * ::Vec<T>::swap(self, idx: uint32_t, val: T) -> !
 * ::Vec<T>::swap_head(self, val: T) -> !
 * ::Vec<T>::swap_tail(self, val: T) -> !
 *
 * ```c
  {
    uu_vec(int) v = uu_vec_init(v);

    uu_vec_insert_tail(v, 0); // {0}
    assert(0 == uu_vec_at_head(v));

    uu_vec_swap_head(v, 100);
    assert(100 == uu_vec_at_head(v));
  }
 * ```
 */
#define uu_vec_swap_head(self, _val) uu_vec_swap(self, 0, _val)
#define uu_vec_swap_tail(self, _val) uu_vec_swap(self, uu_vec_len(self) - 1, _val)
#define uu_vec_swap(self, _idx, _val)                                                              \
  do {                                                                                             \
    extern uint32_t __uu_vec_len(void*);                                                           \
    extern void* __uu_vec_at(void*, uint32_t);                                                     \
                                                                                                   \
    {                                                                                              \
      __typeof__(self) Self = self;                                                                \
      void* nil             = NULL;                                                                \
      assert(Self != nil);                                                                         \
                                                                                                   \
      uint32_t Len = __uu_vec_len((void*)self);                                                    \
      uint32_t Idx = _idx;                                                                         \
      assert(Idx < Len);                                                                           \
    }                                                                                              \
                                                                                                   \
    *(__typeof__(self))__uu_vec_at((void*)self, _idx) = _val;                                      \
  } while (0)

/**
 * ::Vec<T>::at(self, idx: uint32_t) -> T
 * ::Vec<T>::at_head(self) -> T
 * ::Vec<T>::at_tail(self) -> T
 *
 * ```c
  {
    uu_vec(int) v = uu_vec_init(v);

    uu_vec_insert_tail(v, 0); // {0}
    uu_vec_insert_tail(v, 1); // {0, 1}
    uu_vec_insert_tail(v, 2); // {0, 1, 2}
    assert(3 == uu_vec_len(v));

    assert(0 == uu_vec_at_head(v));
    assert(1 == uu_vec_at(v, 1));
    assert(2 == uu_vec_at_tail(v));
  }
 * ```
 */
#define uu_vec_at_head(self) uu_vec_at(self, 0)
#define uu_vec_at_tail(self) uu_vec_at(self, uu_vec_len(self) - 1)
#define uu_vec_at(self, _idx)                                                                      \
  ({                                                                                               \
    extern uint32_t __uu_vec_len(void*);                                                           \
    extern void* __uu_vec_at(void*, uint32_t);                                                     \
                                                                                                   \
    {                                                                                              \
      __typeof__(self) Self = self;                                                                \
      void* nil             = NULL;                                                                \
      assert(Self != nil);                                                                         \
                                                                                                   \
      uint32_t Len = __uu_vec_len((void*)self);                                                    \
      uint32_t Idx = _idx;                                                                         \
      assert(Idx < Len);                                                                           \
    }                                                                                              \
                                                                                                   \
    *(__typeof__(self))__uu_vec_at((void*)self, _idx);                                             \
  })

/**
 * ::Vec<T>::insert(self, idx: uint32_t, val: T) -> !
 * ::Vec<T>::insert_head(self, val: T) -> !
 * ::Vec<T>::insert_tail(self, val: T) -> !
 *
 * ```c
  {
    uu_vec(int) v = uu_vec_init(v);

    uu_vec_insert_tail(v, 0);  // {0}
    uu_vec_insert_tail(v, 1);  // {0, 1}
    uu_vec_insert_tail(v, 2);  // {0, 1, 2}
    uu_vec_insert_head(v, 3); // {3, 0, 1, 2}
    uu_vec_insert(v, 1, 4);    // {3, 4, 0, 1, 2}
    uint32_t len = uu_vec_len(v);
    uu_vec_insert(v, len, 5);  // {3, 4, 0, 1, 2, 5}

    assert(3 == uu_vec_at(v, 0));
    assert(4 == uu_vec_at(v, 1));
    assert(0 == uu_vec_at(v, 2));
    assert(1 == uu_vec_at(v, 3));
    assert(2 == uu_vec_at(v, 4));
    assert(5 == uu_vec_at(v, 5));
  }
 * ```
 */
#define uu_vec_insert_head(self, _val) uu_vec_insert(self, 0, _val)
#define uu_vec_insert_tail(self, _val) uu_vec_insert(self, uu_vec_len(self), _val)
#define uu_vec_insert(self, _idx, _val)                                                            \
  do {                                                                                             \
    extern uint32_t __uu_vec_len(void*);                                                           \
    extern void* __uu_vec_insert(void*, uint32_t);                                                 \
                                                                                                   \
    {                                                                                              \
      __typeof__(self) Self = self;                                                                \
      void* nil             = NULL;                                                                \
      assert(Self != nil);                                                                         \
                                                                                                   \
      uint32_t Len = __uu_vec_len((void*)self);                                                    \
      uint32_t Idx = _idx;                                                                         \
      assert(Idx <= Len);                                                                          \
    }                                                                                              \
                                                                                                   \
    *(__typeof__(self))__uu_vec_insert((void*)self, _idx) = _val;                                  \
  } while (0)

/**
 * ::Vec<T>::remove(self, idx: uint32_t) -> T
 * ::Vec<T>::remove_head(self) -> T
 * ::Vec<T>::remove_tail(self) -> T
 *
 * ```c
  {
    uu_vec(int) v = uu_vec_init(v);

    uu_vec_insert_tail(v, 0);  // {0}
    uu_vec_insert_tail(v, 1);  // {0, 1}
    uu_vec_insert_tail(v, 2);  // {0, 1, 2}
    assert(3 == uu_vec_len(v));

    assert(1 == uu_vec_remove(v, 1));    // {0, 2}
    assert(2 == uu_vec_remove_tail(v));  // {0}
    assert(0 == uu_vec_remove_head(v));  // {}
    assert(0 == uu_vec_len(v));
  }
 * ```
 */
#define uu_vec_remove_head(self) uu_vec_remove(self, 0)
#define uu_vec_remove_tail(self) uu_vec_remove(self, uu_vec_len(self) - 1)
#define uu_vec_remove(self, _idx)                                                                  \
  ({                                                                                               \
    extern uint32_t __uu_vec_len(void*);                                                           \
    extern void* __uu_vec_at(void*, uint32_t);                                                     \
    extern void __uu_vec_remove(void*, uint32_t);                                                  \
                                                                                                   \
    {                                                                                              \
      __typeof__(self) Self = self;                                                                \
      void* nil             = NULL;                                                                \
      assert(Self != nil);                                                                         \
                                                                                                   \
      uint32_t Len = __uu_vec_len((void*)self);                                                    \
      uint32_t Idx = _idx;                                                                         \
      assert(Idx < Len);                                                                           \
    }                                                                                              \
                                                                                                   \
    __typeof__(*self) __val__ = *(__typeof__(self))__uu_vec_at((void*)self, _idx);                 \
                                                                                                   \
    __uu_vec_remove((void*)self, _idx);                                                            \
                                                                                                   \
    __val__;                                                                                       \
  })

/**
 * ::Vec<T>::each(self, it) -> Iter<T>
 * ::Vec<T>::each_if(self, it, cond) -> Iter<T>
 *
 * ```c
  {
    uu_vec(int) v = uu_vec_init(v);

    uu_vec_insert_tail(v, 0);  // {0}
    uu_vec_insert_tail(v, 1);  // {0, 1}
    uu_vec_insert_tail(v, 2);  // {0, 1, 2}
    assert(3 == uu_vec_len(v));

    int i = 0;
    uu_vec_each(v, it) {
      assert(i == it);
      i++;
    }

    assert(i == uu_vec_len(v));
  }
 * ```
 */
#define uu_vec_each_if(self, it, cond) uu_vec_each(self, it) if (cond)
#define uu_vec_each(self, it)                                                                      \
  {                                                                                                \
    extern void* __uu_vec_each(void*, int);                                                        \
                                                                                                   \
    {                                                                                              \
      __typeof__(self) Self = self;                                                                \
      void* nil             = NULL;                                                                \
      assert(Self != nil);                                                                         \
    }                                                                                              \
                                                                                                   \
    (void)__uu_vec_each((void*)self, 1);                                                           \
  };                                                                                               \
                                                                                                   \
  for (__typeof__(*self) it = {0}; ({                                                              \
         extern void* __uu_vec_each(void*, int);                                                   \
                                                                                                   \
         __typeof__(self) __ref__ = (__typeof__(self))__uu_vec_each((void*)self, 2);               \
                                                                                                   \
         it = __ref__ ? *__ref__ : it;                                                             \
                                                                                                   \
         __ref__;                                                                                  \
       });)

/**
 * ::Vec<T>::reach(self, it) -> Iter<T>
 * ::Vec<T>::reach_if(self, it, cond) -> Iter<T>
 *
 * ```c
  {
    uu_vec(int) v = uu_vec_init(v);

    uu_vec_insert_tail(v, 0);  // {0}
    uu_vec_insert_tail(v, 1);  // {0, 1}
    uu_vec_insert_tail(v, 2);  // {0, 1, 2}
    assert(3 == uu_vec_len(v));

    int i = uu_vec_len(v) - 1;
    uu_vec_reach(v, it) {
      assert(i == it);
      i--;
    }

    assert(-1 == i);
  }
 * ```
 */
#define uu_vec_reach_if(self, it, cond) uu_vec_reach(self, it) if (cond)
#define uu_vec_reach(self, it)                                                                     \
  {                                                                                                \
    extern void* __uu_vec_each(void*, int);                                                        \
                                                                                                   \
    {                                                                                              \
      __typeof__(self) Self = self;                                                                \
      void* nil             = NULL;                                                                \
      assert(Self != nil);                                                                         \
    }                                                                                              \
                                                                                                   \
    (void)__uu_vec_each(self, 3);                                                                  \
  };                                                                                               \
                                                                                                   \
  for (__typeof__(*self) it = {0}; ({                                                              \
         extern void* __uu_vec_each(void*, int);                                                   \
                                                                                                   \
         __typeof__(self) __ref__ = (__typeof__(self))__uu_vec_each((void*)self, 4);               \
                                                                                                   \
         it = __ref__ ? *__ref__ : it;                                                             \
                                                                                                   \
         __ref__;                                                                                  \
       });)

/**
 * ::Vec<T>::count_if(self, it, cond) -> uint32_t
 *
 * ```c
  {
    uu_vec(int) v = uu_vec_init(v);

    uu_vec_insert_tail(v, 0);  // {0}
    uu_vec_insert_tail(v, 1);  // {0, 1}
    uu_vec_insert_tail(v, 2);  // {0, 1, 2}
    assert(3 == uu_vec_len(v));

    uint32_t count = uu_vec_count_if(v, it, it % 2 == 0);
    assert(2 == count);

    count = uu_vec_count_if(v, it, it % 2 != 0);
    assert(1 == count);
  }
 * ```
 */
#define uu_vec_count_if(self, it, cond)                                                            \
  ({                                                                                               \
    uint32_t __cnt__ = 0;                                                                          \
                                                                                                   \
    uu_vec_each_if(self, it, cond) __cnt__++;                                                      \
                                                                                                   \
    __cnt__;                                                                                       \
  })

/**
 * ::Vec<T>::find_if(self, it, cond) -> uint32_t | -1
 *
 * ```c
  {
    uu_vec(int) v = uu_vec_init(v);

    uu_vec_insert_tail(v, 0);  // {0}
    uu_vec_insert_tail(v, 1);  // {0, 1}
    uu_vec_insert_tail(v, 2);  // {0, 1, 2}
    uu_vec_insert_tail(v, 1);  // {0, 1, 2, 1}
    assert(4 == uu_vec_len(v));

    uint32_t idx = uu_vec_find_if(v, it, it == 1);
    assert(1 == idx);

    idx = uu_vec_find_if(v, it, it == 3);
    assert(-1 == idx);
  }
 * ```
 */
#define uu_vec_find_if(self, it, cond)                                                             \
  ({                                                                                               \
    int64_t __idx__ = 0;                                                                           \
                                                                                                   \
    uu_vec_each(self, it) {                                                                        \
      if (cond)                                                                                    \
        break;                                                                                     \
                                                                                                   \
      __idx__++;                                                                                   \
    }                                                                                              \
                                                                                                   \
    (__idx__ == uu_vec_len(self)) ? -1 : __idx__;                                                  \
  })

/**
 * ::Vec<T>::rfind_if(self, it, cond) -> uint32_t | -1
 *
 * ```c
  {
    uu_vec(int) v = uu_vec_init(v);

    uu_vec_insert_tail(v, 0);  // {0}
    uu_vec_insert_tail(v, 1);  // {0, 1}
    uu_vec_insert_tail(v, 2);  // {0, 1, 2}
    uu_vec_insert_tail(v, 1);  // {0, 1, 2, 1}
    assert(4 == uu_vec_len(v));

    uint32_t idx = uu_vec_rfind_if(v, it, it == 1);
    assert(3 == idx);

    idx = uu_vec_rfind_if(v, it, it == 3);
    assert(-1 == idx);
  }
 * ```
 */
#define uu_vec_rfind_if(self, it, cond)                                                            \
  ({                                                                                               \
    int64_t __idx__ = uu_vec_len(self);                                                            \
                                                                                                   \
    uu_vec_reach(self, it) {                                                                       \
      if (cond)                                                                                    \
        break;                                                                                     \
                                                                                                   \
      __idx__--;                                                                                   \
    }                                                                                              \
                                                                                                   \
    (__idx__ == 0) ? -1 : __idx__ - 1;                                                             \
  })

/**
 * ::Vec<T>::map_by(self, it, ...) -> !
 *
 * ```c
  {
    uu_vec(int) v = uu_vec_init(v);

    uu_vec_insert_tail(v, 0);  // {0}
    uu_vec_insert_tail(v, 1);  // {0, 1}
    uu_vec_insert_tail(v, 2);  // {0, 1, 2}
    assert(3 == uu_vec_len(v));

    uu_vec_map_by(v, it, { it = it * 2; });

    assert(0 == uu_vec_at(v, 0));
    assert(2 == uu_vec_at(v, 1));
    assert(4 == uu_vec_at(v, 2));
  }
 * ```
 */
#define uu_vec_map_by(self, it, ...)                                                               \
  do {                                                                                             \
    extern void* __uu_vec_each(void*, int);                                                        \
                                                                                                   \
    {                                                                                              \
      __typeof__(self) Self = self;                                                                \
      void* nil             = NULL;                                                                \
      assert(Self != nil);                                                                         \
    }                                                                                              \
                                                                                                   \
    (void)__uu_vec_each((void*)self, 1);                                                           \
                                                                                                   \
    __typeof__(self) __mut__ = NULL;                                                               \
    for (__typeof__(*self) it = {0};                                                               \
         (__mut__ = (__typeof__(self))__uu_vec_each((void*)self, 2));) {                           \
      it = *__mut__;                                                                               \
                                                                                                   \
      __VA_ARGS__                                                                                  \
                                                                                                   \
      *__mut__ = it;                                                                               \
    }                                                                                              \
  } while (0)

/**
 * ::Vec<T>::any_if(self, it, cond) -> bool
 *
 * ```c
  {
    uu_vec(int) v = uu_vec_init(v);

    uu_vec_insert_tail(v, 0);  // {0}
    uu_vec_insert_tail(v, 1);  // {0, 1}
    uu_vec_insert_tail(v, 2);  // {0, 1, 2}
    assert(3 == uu_vec_len(v));

    int result = uu_vec_any_if(v, it, it >= 2);
    assert(result);

    result = uu_vec_any_if(v, it, it > 2);
    assert(!result);
  }
 * ```
 */
#define uu_vec_any_if(self, it, cond)                                                              \
  ({                                                                                               \
    int __result__ = !!0;                                                                          \
                                                                                                   \
    uu_vec_each_if(self, it, cond) {                                                               \
      __result__ = !0;                                                                             \
      break;                                                                                       \
    }                                                                                              \
                                                                                                   \
    __result__;                                                                                    \
  })

/**
 * ::Vec<T>::all_if(self, it, cond) -> bool
 *
 * ```c
  {
    uu_vec(int) v = uu_vec_init(v);

    uu_vec_insert_tail(v, 0);  // {0}
    uu_vec_insert_tail(v, 1);  // {0, 1}
    uu_vec_insert_tail(v, 2);  // {0, 1, 2}
    assert(3 == uu_vec_len(v));

    int result = uu_vec_all_if(v, it, it >= 0);
    assert(result);

    result = uu_vec_all_if(v, it, it > 0);
    assert(!result);
  }
 * ```
 */
#define uu_vec_all_if(self, it, cond)                                                              \
  ({                                                                                               \
    int __result__ = !0;                                                                           \
                                                                                                   \
    uu_vec_each_if(self, it, !(cond)) {                                                            \
      __result__ = !!0;                                                                            \
      break;                                                                                       \
    }                                                                                              \
                                                                                                   \
    __result__;                                                                                    \
  })

/**
 * ::Vec<T>::sort(self, cmp_fn: uu_cmp_fn) -> !
 *
 * ```c
  {
    uu_vec(int) v = uu_vec_init(v);

    uu_vec_insert_tail(v, 2);  // {2}
    uu_vec_insert_tail(v, 1);  // {2, 1}
    uu_vec_insert_tail(v, 4);  // {2, 1, 4}
    uu_vec_insert_tail(v, 3);  // {2, 1, 4, 3}
    assert(4 == uu_vec_len(v));

    uu_vec_sort(v, uu_cmp_fn_int);
    assert(4 == uu_vec_len(v));

    assert(1 == uu_vec_at(v, 0));
    assert(2 == uu_vec_at(v, 1));
    assert(3 == uu_vec_at(v, 2));
    assert(4 == uu_vec_at(v, 3));
  }

 * ```
 */
#define uu_vec_sort(self, cmp_fn)                                                                  \
  do {                                                                                             \
    extern void* __uu_vec_sort(void*, uu_cmp_fn);                                                  \
                                                                                                   \
    {                                                                                              \
      __typeof__(self) Self = self;                                                                \
      void* nil             = NULL;                                                                \
      assert(Self != nil);                                                                         \
    }                                                                                              \
                                                                                                   \
    __uu_vec_sort((void*)self, cmp_fn);                                                            \
  } while (0)

/***************************************************************************************************
 * Tree
 **************************************************************************************************/
/**
 * ::Class Tree<K: fn(K*, K*) -> int, V = void*>
 */
#define uu_tree(K, V) __typeof__(K*)

/**
 * ::Tree<K, V = void*>::init(self, cmp_fn: uu_cmp_fn) -> Self
 *
 * ```c
  {
    uu_tree(int, int*) t = uu_tree_init(t, uu_cmp_fn_int);
    assert(t);
  }
  {
    uu_tree(int, int*) t = NULL;

    t = uu_tree_init(t, uu_cmp_fn_int);
    assert(t);
  }
 * ```
 */
#define uu_tree_init(self, cmp_fn)                                                                 \
  ({                                                                                               \
    extern void* __uu_tree_init(uint32_t, uu_cmp_fn);                                              \
                                                                                                   \
    self = (__typeof__(self))__uu_tree_init(sizeof(*self), cmp_fn);                                \
                                                                                                   \
    self;                                                                                          \
  })

/**
 * ::Tree<K, V = void*>::clear(self) -> !
 * ::Tree<K, V = void*>::clear(self, ...) -> !
 *
 * ```c
  {
    uu_tree(int, int*) t = uu_tree_init(t, uu_cmp_fn_int);

    uu_tree_insert(t, 1, (void*)0x11);
    assert(1 == uu_tree_len(t));

    uu_tree_clear(t);
    assert(t);
    assert(0 == uu_tree_len(t));
  }
 * ```
 */
#define uu_tree_clear(self, ...)                                                                   \
  do {                                                                                             \
    extern void __uu_tree_clear(void*);                                                            \
                                                                                                   \
    {                                                                                              \
      __typeof__(self) Self = self;                                                                \
      void* nil             = NULL;                                                                \
      assert(Self != nil);                                                                         \
    }                                                                                              \
                                                                                                   \
    uu_tree_each(self, key, void*, uptr) {                                                         \
      __VA_ARGS__;                                                                                 \
                                                                                                   \
      (void)key;                                                                                   \
      (void)uptr;                                                                                  \
    };                                                                                             \
                                                                                                   \
    __uu_tree_clear((void*)self);                                                                  \
  } while (0)

/**
 * ::Tree<K, V = void*>::deinit(self) -> !
 * ::Tree<K, V = void*>::deinit(self, ...) -> !
 *
 * ```c
  {
    uu_tree(int, int*) t = uu_tree_init(t, uu_cmp_fn_int);

    uu_tree_deinit(t);
    assert(!t);
  }
 * ```
 */
#define uu_tree_deinit(self, ...)                                                                  \
  do {                                                                                             \
    extern void __uu_tree_deinit(void*);                                                           \
                                                                                                   \
    {                                                                                              \
      __typeof__(self) Self = self;                                                                \
      void* nil             = NULL;                                                                \
      assert(Self != nil);                                                                         \
    }                                                                                              \
                                                                                                   \
    uu_tree_each(self, key, void*, uptr) {                                                         \
      __VA_ARGS__;                                                                                 \
                                                                                                   \
      (void)key;                                                                                   \
      (void)uptr;                                                                                  \
    };                                                                                             \
                                                                                                   \
    __uu_tree_deinit((void*)self);                                                                 \
                                                                                                   \
    self = NULL;                                                                                   \
  } while (0)

/**
 * ::Tree<K, V = void*>::len(self) -> uint32_t
 *
 * ```c
  {
    uu_tree(int, int*) t = uu_tree_init(t, uu_cmp_fn_int);
    assert(0 == uu_tree_len(t));

    uu_tree_insert(t, 1, (void*)0x11);
    assert(1 == uu_tree_len(t));

    uu_tree_insert(t, 2, (void*)0x22);
    assert(2 == uu_tree_len(t));
  }
 * ```
 */
#define uu_tree_len(self)                                                                          \
  ({                                                                                               \
    extern uint32_t __uu_tree_len(void*);                                                          \
                                                                                                   \
    {                                                                                              \
      __typeof__(self) Self = self;                                                                \
      void* nil             = NULL;                                                                \
      assert(Self != nil);                                                                         \
    }                                                                                              \
    __uu_tree_len((void*)self);                                                                    \
  })

/**
 * ::Tree<K, V = void*>::is_empty(self) -> bool
 *
 * ```c
  {
    uu_tree(int, int*) t = uu_tree_init(t, uu_cmp_fn_int);
    assert(uu_tree_is_empty(t));

    uu_tree_insert(t, 1, (void*)0x11);
    assert(!uu_tree_is_empty(t));
  }
 * ```
 */
#define uu_tree_is_empty(self)                                                                     \
  ({                                                                                               \
    extern uint32_t __uu_tree_len(void*);                                                          \
                                                                                                   \
    {                                                                                              \
      __typeof__(self) Self = self;                                                                \
      void* nil             = NULL;                                                                \
      assert(Self != nil);                                                                         \
    }                                                                                              \
                                                                                                   \
    0 == __uu_tree_len((void*)self);                                                               \
  })

/**
 * ::Tree<K, V = void*>::at(self, key: K) -> V
 *
 * ```c
  {
    uu_tree(int, int*) t = uu_tree_init(t, uu_cmp_fn_int);

    uu_tree_insert(t, 1, (void*)0x11);
    uu_tree_insert(t, 2, (void*)0x22);
    uu_tree_insert(t, 3, (void*)0x33);

    assert((void*)0x11 == uu_tree_at(t, 1));
    assert((void*)0x22 == uu_tree_at(t, 2));
    assert((void*)0x33 == uu_tree_at(t, 3));
    assert(NULL == uu_tree_at(t, 4));
  }
 * ```
 */
#define uu_tree_at(self, _key)                                                                     \
  ({                                                                                               \
    extern void* __uu_tree_at(void*, void*);                                                       \
                                                                                                   \
    {                                                                                              \
      __typeof__(self) Self = self;                                                                \
      void* nil             = NULL;                                                                \
      assert(Self != nil);                                                                         \
    }                                                                                              \
                                                                                                   \
    __typeof__(*self) __key__ = _key;                                                              \
                                                                                                   \
    __uu_tree_at((void*)self, (void*)&__key__);                                                    \
  })

/**
 * ::Tree<K, V = void*>::insert(self, key: K, uptr: V) -> bool
 *
 * ```c
  {
    uu_tree(int, int*) t = uu_tree_init(t, uu_cmp_fn_int);

    int result = uu_tree_insert(t, 1, (void*)0x11);
    assert(result);

    result = uu_tree_insert(t, 2, (void*)0x22);
    assert(result);

    result = uu_tree_insert(t, 1, (void*)0x11);
    assert(!result);  // key already exists

    assert(2 == uu_tree_len(t));
  }
 * ```
 */
#define uu_tree_insert(self, _key, _uptr)                                                          \
  ({                                                                                               \
    extern int __uu_tree_insert(void*, void*, void*);                                              \
                                                                                                   \
    {                                                                                              \
      __typeof__(self) Self = self;                                                                \
      void* nil             = NULL;                                                                \
      assert(Self != nil);                                                                         \
    }                                                                                              \
                                                                                                   \
    void* __uptr__            = _uptr;                                                             \
    __typeof__(*self) __key__ = _key;                                                              \
                                                                                                   \
    __uu_tree_insert((void*)self, (void*)&__key__, __uptr__);                                      \
  })

/**
 * ::Tree<K, V = void*>::remove(self, key: K) -> V
 *
 * ```c
  {
    uu_tree(int, int*) t = uu_tree_init(t, uu_cmp_fn_int);

    uu_tree_insert(t, 1, (void*)0x11);
    uu_tree_insert(t, 2, (void*)0x22);
    uu_tree_insert(t, 3, (void*)0x33);

    void* result = uu_tree_remove(t, 2);
    assert(result == (void*)0x22);

    result = uu_tree_remove(t, 2);
    assert(result == NULL);  // key not found

    assert(2 == uu_tree_len(t));
  }
 * ```
 */
#define uu_tree_remove(self, _key)                                                                 \
  ({                                                                                               \
    extern void* __uu_tree_remove(void*, void*);                                                   \
                                                                                                   \
    {                                                                                              \
      __typeof__(self) Self = self;                                                                \
      void* nil             = NULL;                                                                \
      assert(Self != nil);                                                                         \
    }                                                                                              \
                                                                                                   \
    __typeof__(*self) __key__ = _key;                                                              \
                                                                                                   \
    __uu_tree_remove((void*)self, (void*)&__key__);                                                \
  })

/**
 * ::Tree<K, V = void*, U: V>::each(self, key, U, uptr) -> Iter<K, U>
 * ::Tree<K, V = void*, U: V>::each_if(self, key, U, uptr, cond) -> Iter<K, U>
 *
 * ```c
  {
    uu_tree(int, int*) t = uu_tree_init(t, uu_cmp_fn_int);

    uu_tree_insert(t, 1, (void*)0x11);
    uu_tree_insert(t, 2, (void*)0x22);
    uu_tree_insert(t, 3, (void*)0x33);

    int cnt = 0;
    uu_tree_each(t, key, int*, uptr) {
      cnt++;
    }

    assert(cnt == uu_tree_len(t));
  }
 * ```
 */
#define uu_tree_each_if(self, key, type, uptr, cond) uu_tree_each(self, key, type, uptr) if (cond)
#define uu_tree_each(self, key, type, uptr)                                                        \
  {                                                                                                \
    extern int __uu_tree_each(void*, int, void* [2]);                                              \
                                                                                                   \
    {                                                                                              \
      __typeof__(self) Self = self;                                                                \
      void* nil             = NULL;                                                                \
      assert(Self != nil);                                                                         \
    }                                                                                              \
                                                                                                   \
    (void)__uu_tree_each((void*)self, !0, NULL);                                                   \
  }                                                                                                \
                                                                                                   \
  for (__typeof__(*self) key = {0}, *__key__ = &key; __key__; __key__ = NULL)                      \
    for (type uptr = NULL; ({                                                                      \
           extern int __uu_tree_each(void*, int, void* [2]);                                       \
                                                                                                   \
           __typeof__(self) __out__[2] = {0};                                                      \
           int __result__              = __uu_tree_each((void*)self, !!0, (void**)&__out__);       \
                                                                                                   \
           if (__result__) {                                                                       \
             key  = *__out__[0];                                                                   \
             uptr = (type)(void*)__out__[1];                                                       \
           }                                                                                       \
                                                                                                   \
           __result__;                                                                             \
         });)

/**
 * ::Tree<K, V = void*, U: V>::find_if(self, key, U, uptr, cond) -> U
 *
 * ```c
  {
    uu_tree(int, int*) t = uu_tree_init(t, uu_cmp_fn_int);

    uu_tree_insert(t, 1, (void*)0x11);
    uu_tree_insert(t, 2, (void*)0x22);
    uu_tree_insert(t, 3, (void*)0x33);

    int* uptr = uu_tree_find_if(t, key, int*, uptr, uptr == (int*)(uintptr_t)0x22);
    assert(uptr == (int*)(uintptr_t)0x22);

    uptr = uu_tree_find_if(t, key, int*, uptr, uptr == (int*)(uintptr_t)0x44);
    assert(uptr == NULL);
  }
 * ```
 */
#define uu_tree_find_if(self, key, type, uptr, cond)                                               \
  ({                                                                                               \
    type __uptr__ = NULL;                                                                          \
                                                                                                   \
    uu_tree_each_if(self, key, type, uptr, cond) {                                                 \
      __uptr__ = uptr;                                                                             \
      break;                                                                                       \
    }                                                                                              \
                                                                                                   \
    __uptr__;                                                                                      \
  })

/**
 * ::Tree<K, V = void*, U: V>::any_if(self, key, U, uptr, cond) -> bool
 *
 * ```c
  {
    uu_tree(int, int*) t = uu_tree_init(t, uu_cmp_fn_int);

    uu_tree_insert(t, 1, (void*)0x11);
    uu_tree_insert(t, 2, (void*)0x22);
    uu_tree_insert(t, 3, (void*)0x33);

    int result = uu_tree_any_if(t, key, int*, uptr, uptr == (int*)(uintptr_t)0x22);
    assert(result);

    result = uu_tree_any_if(t, key, int*, uptr, uptr == (int*)(uintptr_t)0x44);
    assert(!result);
  }
 * ```
 */
#define uu_tree_any_if(self, key, type, uptr, cond)                                                \
  ({                                                                                               \
    int __result__ = !!0;                                                                          \
                                                                                                   \
    uu_tree_each_if(self, key, type, uptr, cond) {                                                 \
      __result__ = !0;                                                                             \
      break;                                                                                       \
    }                                                                                              \
                                                                                                   \
    __result__;                                                                                    \
  })

/**
 * ::Tree<K, V = void*, U: V>::all_if(self, key, U, uptr, cond) -> bool
 *
 * ```c
  {
    uu_tree(int, int*) t = uu_tree_init(t, uu_cmp_fn_int);

    uu_tree_insert(t, 1, (void*)0x11);
    uu_tree_insert(t, 2, (void*)0x22);
    uu_tree_insert(t, 3, (void*)0x33);

    int result = uu_tree_all_if(t, key, int*, uptr, uptr >= (int*)(intptr_t)0x11);
    assert(result);

    result = uu_tree_all_if(t, key, int*, uptr, uptr > (int*)(intptr_t)0x11);
    assert(!result);
  }
 * ```
 */
#define uu_tree_all_if(self, key, type, uptr, cond)                                                \
  ({                                                                                               \
    int __result__ = !0;                                                                           \
                                                                                                   \
    uu_tree_each_if(self, key, type, uptr, !(cond)) {                                              \
      __result__ = !!0;                                                                            \
      break;                                                                                       \
    }                                                                                              \
                                                                                                   \
    __result__;                                                                                    \
  })

/***************************************************************************************************
 * Dict
 **************************************************************************************************/

typedef void (*uu_dict_dump_fn)(const void* key, const void* uptr);

#ifdef UU_DEBUG
#  define uu_dict_dump(self, dump_fn)                                                              \
    do {                                                                                           \
      extern void __uu_dict_dump(void*, uu_dict_dump_fn);                                          \
                                                                                                   \
      __uu_dict_dump(self, dump_fn);                                                               \
    } while (0)
#endif /* !UU_DEBUG */

/**
 * ::Class Dict<K: fn(K*, K*) -> int, V = void*>
 */
#define uu_dict(K, V) __typeof__(K*)

/**
 * ::Dict<K, V = void*>::init(self, cmp_fn: uu_cmp_fn, hash_fn: uu_hash_fn) -> Self
 *
 * ```c
  {
    uu_dict(int, int*) d = uu_dict_init(d, uu_cmp_fn_int, NULL);
    assert(d);
  }
  {
    uu_dict(int, int*) d = NULL;

    d = uu_dict_init(d, uu_cmp_fn_int, NULL);
    assert(d);
  }
  {
    uu_dict(char*, int*) d = uu_dict_init(d, uu_cmp_fn_cstr, uu_hash_fn_cstr);
    assert(d);
  }
 * ```
 */
#define uu_dict_init(self, cmp_fn, hash_fn)                                                        \
  ({                                                                                               \
    extern void* __uu_dict_init(uint32_t, uu_cmp_fn, uu_hash_fn);                                  \
                                                                                                   \
    {                                                                                              \
      uu_cmp_fn CmpFn = cmp_fn;                                                                    \
      void* nil       = NULL;                                                                      \
      assert(CmpFn != nil);                                                                        \
    }                                                                                              \
                                                                                                   \
    self = (__typeof__(self))__uu_dict_init(sizeof(*self), cmp_fn, hash_fn);                       \
                                                                                                   \
    self;                                                                                          \
  })

/**
 * ::Dict<K, V = void*>::clear(self) -> !
 * ::Dict<K, V = void*>::clear(self, ...) -> !
 *
 * ```c
  {
    uu_dict(int, int*) d = uu_dict_init(d, uu_cmp_fn_int, NULL);
    assert(d);

    uu_dict_insert(d, 1, (void*)0x11);
    assert(1 == uu_dict_len(d));

    uu_dict_clear(d);
    assert(d);
    assert(0 == uu_dict_len(d));
  }
 * ```
 */
#define uu_dict_clear(self, ...)                                                                   \
  do {                                                                                             \
    extern void __uu_dict_clear(void*);                                                            \
                                                                                                   \
    {                                                                                              \
      __typeof__(self) Self = self;                                                                \
      void* nil             = NULL;                                                                \
      assert(Self != nil);                                                                         \
    }                                                                                              \
                                                                                                   \
    uu_dict_each(self, key, void*, uptr) {                                                         \
      __VA_ARGS__;                                                                                 \
                                                                                                   \
      (void)key;                                                                                   \
      (void)uptr;                                                                                  \
    };                                                                                             \
                                                                                                   \
    __uu_dict_clear((void*)self);                                                                  \
  } while (0)

/**
 * ::Dict<K, V = void*>::deinit(self) -> !
 * ::Dict<K, V = void*>::deinit(self, ...) -> !
 *
 * ```c
  {
    uu_dict(int, int*) d = uu_dict_init(d, uu_cmp_fn_int, NULL);
    assert(d);

    uu_dict_deinit(d);
    assert(!d);
  }
 * ```
 */
#define uu_dict_deinit(self, ...)                                                                  \
  do {                                                                                             \
    extern void __uu_dict_deinit(void*);                                                           \
                                                                                                   \
    {                                                                                              \
      __typeof__(self) Self = self;                                                                \
      void* nil             = NULL;                                                                \
      assert(Self != nil);                                                                         \
    }                                                                                              \
                                                                                                   \
    uu_dict_each(self, key, void*, uptr) {                                                         \
      __VA_ARGS__;                                                                                 \
                                                                                                   \
      (void)key;                                                                                   \
      (void)uptr;                                                                                  \
    };                                                                                             \
                                                                                                   \
    __uu_dict_deinit((void*)self);                                                                 \
                                                                                                   \
    self = NULL;                                                                                   \
  } while (0)

/**
 * ::Dict<K, V = void*>::len(self) -> uint32_t
 *
 * ```c
  {
    uu_dict(int, int*) d = uu_dict_init(d, uu_cmp_fn_int, NULL);

    uu_dict_insert(d, 1, (void*)0x11);
    uu_dict_insert(d, 2, (void*)0x22);
    uu_dict_insert(d, 3, (void*)0x33);
    uu_dict_insert(d, 4, (void*)0x44);
    uu_dict_insert(d, 5, (void*)0x55);

    assert(5 == uu_dict_len(d));
  }
 * ```
 */
#define uu_dict_len(self)                                                                          \
  ({                                                                                               \
    extern uint32_t __uu_dict_len(void*);                                                          \
                                                                                                   \
    {                                                                                              \
      __typeof__(self) Self = self;                                                                \
      void* nil             = NULL;                                                                \
      assert(Self != nil);                                                                         \
    }                                                                                              \
    __uu_dict_len((void*)self);                                                                    \
  })

/**
 * ::Dict<K, V = void*>::is_empty(self) -> bool
 *
 * ```c
  {
    uu_dict(int, int*) d = uu_dict_init(d, uu_cmp_fn_int, NULL);

    assert(uu_dict_is_empty(d));

    uu_dict_insert(d, 1, (void*)0x11);
    uu_dict_insert(d, 2, (void*)0x22);
    uu_dict_insert(d, 3, (void*)0x33);
    uu_dict_insert(d, 4, (void*)0x44);
    uu_dict_insert(d, 5, (void*)0x55);

    assert(!uu_dict_is_empty(d));
  }
 * ```
 */
#define uu_dict_is_empty(self)                                                                     \
  ({                                                                                               \
    extern uint32_t __uu_dict_len(void*);                                                          \
                                                                                                   \
    {                                                                                              \
      __typeof__(self) Self = self;                                                                \
      void* nil             = NULL;                                                                \
      assert(Self != nil);                                                                         \
    }                                                                                              \
                                                                                                   \
    0 == __uu_dict_len((void*)self);                                                               \
  })

/**
 * ::Dict<K, V = void*>::at(self, key: K) -> V
 *
 * ```c
  {
    uu_dict(int, int*) d = uu_dict_init(d, uu_cmp_fn_int, NULL);

    uu_dict_insert(d, 1, (void*)0x11);
    uu_dict_insert(d, 2, (void*)0x22);
    uu_dict_insert(d, 3, (void*)0x33);
    uu_dict_insert(d, 4, (void*)0x44);
    uu_dict_insert(d, 5, (void*)0x55);

    assert((void*)0x11 == uu_dict_at(d, 1));
    assert((void*)0x22 == uu_dict_at(d, 2));
    assert((void*)0x33 == uu_dict_at(d, 3));
    assert((void*)0x44 == uu_dict_at(d, 4));
    assert((void*)0x55 == uu_dict_at(d, 5));
  }
 * ```
 */
#define uu_dict_at(self, _key)                                                                     \
  ({                                                                                               \
    extern void* __uu_dict_at(void*, void*);                                                       \
                                                                                                   \
    {                                                                                              \
      __typeof__(self) Self = self;                                                                \
      void* nil             = NULL;                                                                \
      assert(Self != nil);                                                                         \
    }                                                                                              \
                                                                                                   \
    __typeof__(*self) __key__ = _key;                                                              \
                                                                                                   \
    __uu_dict_at((void*)self, (void*)&__key__);                                                    \
  })

/**
 * ::Dict<K, V = void*>::insert(self, key: K, uptr: V) -> bool
 *
 * ```c
  {
    uu_dict(int, int*) d = uu_dict_init(d, uu_cmp_fn_int, NULL);

    int result = 0;

    result = uu_dict_insert(d, 1, (void*)0x11);
    assert(result);
    result = uu_dict_insert(d, 2, (void*)0x22);
    assert(result);
    result = uu_dict_insert(d, 3, (void*)0x33);
    assert(result);
    result = uu_dict_insert(d, 4, (void*)0x44);
    assert(result);
    result = uu_dict_insert(d, 5, (void*)0x55);
    assert(result);

    result = uu_dict_insert(d, 3, (void*)0x33);
    assert(!result);

    assert((void*)0x11 == uu_dict_at(d, 1));
    assert((void*)0x22 == uu_dict_at(d, 2));
    assert((void*)0x33 == uu_dict_at(d, 3));
    assert((void*)0x44 == uu_dict_at(d, 4));
    assert((void*)0x55 == uu_dict_at(d, 5));

    assert(5 == uu_dict_len(d));
  }
 * ```
 */
#define uu_dict_insert(self, _key, _uptr)                                                          \
  ({                                                                                               \
    extern int __uu_dict_insert(void*, void*, void*);                                              \
                                                                                                   \
    {                                                                                              \
      __typeof__(self) Self = self;                                                                \
      void* nil             = NULL;                                                                \
      assert(Self != nil);                                                                         \
    }                                                                                              \
                                                                                                   \
    void* __uptr__            = _uptr;                                                             \
    __typeof__(*self) __key__ = _key;                                                              \
                                                                                                   \
    __uu_dict_insert((void*)self, (void*)&__key__, __uptr__);                                      \
  })

/**
 * ::Dict<K, V = void*>::remove(self, key: K) -> V
 *
 * ```c
  {
    uu_dict(int, int*) d = uu_dict_init(d, uu_cmp_fn_int, NULL);

    uu_dict_insert(d, 1, (void*)0x11);
    uu_dict_insert(d, 2, (void*)0x22);
    uu_dict_insert(d, 3, (void*)0x33);
    uu_dict_insert(d, 4, (void*)0x44);
    uu_dict_insert(d, 5, (void*)0x55);

    void* result = uu_dict_remove(d, 3);
    assert(result);

    result = uu_dict_remove(d, 3);
    assert(!result);

    assert((void*)0x11 == uu_dict_at(d, 1));
    assert((void*)0x22 == uu_dict_at(d, 2));
    assert((void*)0x44 == uu_dict_at(d, 4));
    assert((void*)0x55 == uu_dict_at(d, 5));

    assert((void*)NULL == uu_dict_at(d, 3));

    assert(4 == uu_dict_len(d));
  }
 * ```
 */
#define uu_dict_remove(self, _key)                                                                 \
  ({                                                                                               \
    extern void* __uu_dict_remove(void*, void*);                                                   \
                                                                                                   \
    {                                                                                              \
      __typeof__(self) Self = self;                                                                \
      void* nil             = NULL;                                                                \
      assert(Self != nil);                                                                         \
    }                                                                                              \
                                                                                                   \
    __typeof__(*self) __key__ = _key;                                                              \
                                                                                                   \
    __uu_dict_remove((void*)self, (void*)&__key__);                                                \
  })

/**
 * ::Dict<K, V = void*, U: V>::each(self, key, U, uptr) -> Iter<K, U>
 * ::Dict<K, V = void*, U: V>::each_if(self, key, U, uptr) -> Iter<K, U>
 *
 * ```c
  {
    uu_dict(int, int*) d = uu_dict_init(d, uu_cmp_fn_int, NULL);

    uu_dict_insert(d, 1, (void*)0x11);
    uu_dict_insert(d, 2, (void*)0x22);
    uu_dict_insert(d, 3, (void*)0x33);
    uu_dict_insert(d, 4, (void*)0x44);
    uu_dict_insert(d, 5, (void*)0x55);

    int cnt = 0;
    uu_dict_each(d, key, int*, uptr) {
      assert((void*)((uintptr_t)key * 0x11) == uptr);

      cnt++;
    }

    assert(cnt == uu_dict_len(d));
  }
 * ```
 */
#define uu_dict_each_if(self, key, type, uptr, cond) uu_dict_each(self, key, type, uptr) if (cond)
#define uu_dict_each(self, key, type, uptr)                                                        \
  {                                                                                                \
    extern int __uu_dict_each(void*, int, void* [2]);                                              \
                                                                                                   \
    {                                                                                              \
      __typeof__(self) Self = self;                                                                \
      void* nil             = NULL;                                                                \
      assert(Self != nil);                                                                         \
    }                                                                                              \
                                                                                                   \
    (void)__uu_dict_each((void*)self, !0, NULL);                                                   \
  }                                                                                                \
                                                                                                   \
  for (__typeof__(*self) key = {0}, *__key__ = &key; __key__; __key__ = NULL)                      \
    for (type uptr = NULL; ({                                                                      \
           extern int __uu_dict_each(void*, int, void* [2]);                                       \
                                                                                                   \
           __typeof__(self) __out__[2] = {0};                                                      \
           int __result__              = __uu_dict_each((void*)self, !!0, (void**)&__out__);       \
                                                                                                   \
           if (__result__) {                                                                       \
             key  = *__out__[0];                                                                   \
             uptr = (type)(void*)__out__[1];                                                       \
           }                                                                                       \
                                                                                                   \
           __result__;                                                                             \
         });)

/**
 * ::Dict<K, V = void*, U: V>::find_if(self, key, U, uptr, cond) -> U
 *
 * ```c
  {
    uu_dict(int, int*) d = uu_dict_init(d, uu_cmp_fn_int, NULL);

    uu_dict_insert(d, 1, (void*)0x11);
    uu_dict_insert(d, 2, (void*)0x22);
    uu_dict_insert(d, 3, (void*)0x33);
    uu_dict_insert(d, 4, (void*)0x44);
    uu_dict_insert(d, 5, (void*)0x55);

    int* uptr = uu_dict_find_if(d, key, int*, uptr, uptr == (int*)(uintptr_t)0x33);
    assert(uptr == (int*)(uintptr_t)0x33);

    uptr = uu_dict_find_if(d, key, int*, uptr, uptr == (int*)(uintptr_t)0x66);
    assert(uptr == NULL);
  }
 * ```
 */
#define uu_dict_find_if(self, key, type, uptr, cond)                                               \
  ({                                                                                               \
    type __uptr__ = NULL;                                                                          \
                                                                                                   \
    uu_dict_each_if(self, key, type, uptr, cond) {                                                 \
      __uptr__ = uptr;                                                                             \
      break;                                                                                       \
    }                                                                                              \
                                                                                                   \
    __uptr__;                                                                                      \
  })

/**
 * ::Dict<K, V = void*, U: V>::any_if(self, key, U, uptr, cond) -> bool
 *
 * ```c
  {
    uu_dict(int, int*) d = uu_dict_init(d, uu_cmp_fn_int, NULL);

    uu_dict_insert(d, 1, (void*)0x11);
    uu_dict_insert(d, 2, (void*)0x22);
    uu_dict_insert(d, 3, (void*)0x33);
    uu_dict_insert(d, 4, (void*)0x44);
    uu_dict_insert(d, 5, (void*)0x55);

    int result = uu_dict_any_if(d, key, int*, uptr, uptr == (int*)(uintptr_t)0x33);
    assert(result);

    result = uu_dict_any_if(d, key, int*, uptr, uptr == (int*)(uintptr_t)0x66);
    assert(!result);
  }
 * ```
 */
#define uu_dict_any_if(self, key, type, uptr, cond)                                                \
  ({                                                                                               \
    int __result__ = !!0;                                                                          \
                                                                                                   \
    uu_dict_each_if(self, key, type, uptr, cond) {                                                 \
      __result__ = !0;                                                                             \
      break;                                                                                       \
    }                                                                                              \
                                                                                                   \
    __result__;                                                                                    \
  })

/**
 * ::Dict<K, V = void*, U: V>::all_if(self, key, U, uptr, cond) -> bool
 *
 * ```c
  {
    uu_dict(int, int*) d = uu_dict_init(d, uu_cmp_fn_int, NULL);

    uu_dict_insert(d, 1, (void*)0x11);
    uu_dict_insert(d, 2, (void*)0x22);
    uu_dict_insert(d, 3, (void*)0x33);
    uu_dict_insert(d, 4, (void*)0x44);
    uu_dict_insert(d, 5, (void*)0x55);

    int result = uu_dict_all_if(d, key, int*, uptr, uptr >= (int*)(intptr_t)0x11);
    assert(result);

    result = uu_dict_all_if(d, key, int*, uptr, uptr > (int*)(intptr_t)0x11);
    assert(!result);
  }
 * ```
 */
#define uu_dict_all_if(self, key, type, uptr, cond)                                                \
  ({                                                                                               \
    int __result__ = !0;                                                                           \
                                                                                                   \
    uu_dict_each_if(self, key, type, uptr, !(cond)) {                                              \
      __result__ = !!0;                                                                            \
      break;                                                                                       \
    }                                                                                              \
                                                                                                   \
    __result__;                                                                                    \
  })

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* !__UU_H__ */
