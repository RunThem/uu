#include "uu.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define UU_DICT_NBL_STACK_MAX 64

#if defined(__GNUC__) || defined(__clang__)
#  define likely(x)   __builtin_expect(!!(x), 1)
#  define unlikely(x) __builtin_expect(!!(x), 0)
#else
#  define likely(x)   (x)
#  define unlikely(x) (x)
#endif

/***************************************************************************************************
 * Macro
 **************************************************************************************************/
#define uu_chk_if(expr, ...)                                                                       \
  if (expr) {                                                                                      \
    return __VA_ARGS__;                                                                            \
  }                                                                                                \
  do {                                                                                             \
  } while (0)

#define uu_end_if(expr, ...)                                                                       \
  if (expr) {                                                                                      \
    goto __VA_ARGS__;                                                                              \
  }                                                                                                \
  do {                                                                                             \
  } while (0)

/***************************************************************************************************
 * Vec
 **************************************************************************************************/
#undef at
#define at(idx) (self->items + self->itsize * (idx))

struct uu_vec_t;
typedef struct uu_vec_t* uu_vec_mut_t;
typedef const struct uu_vec_t* uu_vec_ref_t;
typedef struct uu_vec_t {
  uint32_t itsize;
  uint32_t len;
  uint32_t cap;
  uint32_t idx;
  uint8_t* items;
} uu_vec_t;

void* __uu_vec_init(uint32_t itsize) {
  uu_vec_mut_t self = NULL;

  self = (uu_vec_mut_t)UU_MALLOC(sizeof(uu_vec_t));
  uu_end_if(!self, err0);

  self->items = (uint8_t*)UU_MALLOC(itsize * 16);
  uu_end_if(!self->items, err1);

  self->itsize = itsize;
  self->cap    = 16;
  self->len    = 0;
  self->idx    = 0;

  return (void*)self;

err1:
  UU_FREE(self);

err0:
  return NULL;
}

void __uu_vec_clear(void* _self) {
  uu_vec_mut_t self = (uu_vec_mut_t)_self;

  self->len = 0;
}

void __uu_vec_deinit(void* _self) {
  uu_vec_mut_t self = (uu_vec_mut_t)_self;

  UU_FREE(self->items);
  UU_FREE(self);
}

uint32_t __uu_vec_len(void* _self) {
  uu_vec_mut_t self = (uu_vec_mut_t)_self;

  return self->len;
}

void* __uu_vec_at(void* _self, uint32_t idx) {
  uu_vec_mut_t self = (uu_vec_mut_t)_self;

  return at(idx);
}

void* __uu_vec_insert(void* _self, uint32_t idx) {
  uu_vec_mut_t self = (uu_vec_mut_t)_self;

  /* resize */
  if (self->len == self->cap) {
    uint8_t* items = (uint8_t*)UU_REALLOC(self->items, 2 * self->cap * self->itsize);
    uu_end_if(!items, err0);

    self->cap *= 2;
    self->items = items;
  }

  if (idx != self->len) {
    memmove(at(idx + 1), at(idx), (self->len - idx) * self->itsize);
  }

  self->len++;

  return at(idx);

err0:
  return NULL;
}

void __uu_vec_remove(void* _self, uint32_t idx) {
  uu_vec_mut_t self = (uu_vec_mut_t)_self;

  if (idx != self->len - 1) {
    memmove(at(idx), at(idx + 1), (self->len - idx - 1) * self->itsize);
  }

  self->len--;
}

void* __uu_vec_each(void* _self, int ev) {
  uu_vec_mut_t self = (uu_vec_mut_t)_self;

  switch (ev) {
    case 1: self->idx = 0; return NULL;
    case 3: self->idx = self->len; return NULL;

    case 2: return (self->idx == self->len) ? NULL : at(self->idx++);
    case 4: return (self->idx == 0) ? NULL : at(--self->idx);

    default: assert(0);
  }

  return NULL;
}

void __uu_vec_sort(void* _self, uu_cmp_fn cmp_fn) {
  uu_vec_mut_t self = (uu_vec_mut_t)_self;

  qsort(self->items, self->len, self->itsize, cmp_fn);
}

/***************************************************************************************************
 * Dict
 **************************************************************************************************/

/* clang-format off */
struct mnode_t;
typedef struct mnode_t* mnode_mut_t;
typedef const struct mnode_t* mnode_ref_t;
typedef struct mnode_t {
  mnode_mut_t l, r, p;
  uint32_t h;
  uint32_t hash;
  void* uptr;
  uint8_t key[0];
} mnode_t;

struct mtree_t;
typedef struct mtree_t* mtree_mut_t;
typedef const struct mtree_t* mtree_ref_t;
typedef struct mtree_t {
  int len;
  mnode_mut_t root;
} mtree_t;

static inline int __lh(mnode_mut_t n) { return n->l ? n->l->h : 0; }
static inline int __rh(mnode_mut_t n) { return n->r ? n->r->h : 0; }
static inline void __hu(mnode_mut_t n) { int lh = __lh(n), rh = __rh(n); n->h = (lh > rh ? lh : rh) + 1; }
static inline void __mtree_child_replace(mtree_mut_t T, mnode_mut_t p, mnode_mut_t o, mnode_mut_t n) {
  if (!p) { T->root = n; } else { *(p->l == o ? &p->l : &p->r) = n; }
}
static inline mnode_mut_t __mtree_rotate_left(mtree_mut_t T, mnode_mut_t n) {
  mnode_mut_t r = n->r, p = n->p;
  n->r = r->l;
  if (r->l) r->l->p = n;
  r->l = n;
  r->p = p;
  __mtree_child_replace(T, p, n, r);
  n->p = r;
  return r;
}
static inline mnode_mut_t __mtree_rotate_right(mtree_mut_t T, mnode_mut_t n) {
  mnode_mut_t l = n->l, p = n->p;
  n->l = l->r;
  if (l->r) l->r->p = n;
  l->r = n;
  l->p = p;
  __mtree_child_replace(T, p, n, l);
  n->p = l;
  return l;
}
static inline mnode_mut_t __mtree_fix_left(mtree_mut_t T, mnode_mut_t n) {
  mnode_mut_t r = n->r;
  int lh = __lh(r), rh = __rh(r);
  if (lh > rh) {
    r = __mtree_rotate_right(T, r);
    __hu(r->r);
    __hu(r);
  }
  n = __mtree_rotate_left(T, n);
  __hu(n->l);
  __hu(n);
  return n;
}
static inline mnode_mut_t __mtree_fix_right(mtree_mut_t T, mnode_mut_t n) {
  mnode_mut_t l = n->l;
  int lh = __lh(l), rh = __rh(l);
  if (lh < rh) {
    l = __mtree_rotate_left(T, l);
    __hu(l->l);
    __hu(l);
  }
  n = __mtree_rotate_right(T, n);
  __hu(n->r);
  __hu(n);
  return n;
}
static inline mnode_mut_t __mtree_pop_left_and_right(mtree_mut_t T, mnode_mut_t n) {
  mnode_mut_t o = n, p, l, c;
  n             = n->r;
  while ((l = n->l)) n = l;
  c = n->r;
  p = n->p;
  if (c) c->p = p;
  __mtree_child_replace(T, p, n, c);
  if (n->p == o) p = n;
  n->l = o->l;
  n->r = o->r;
  n->p = o->p;
  n->h = o->h;
  __mtree_child_replace(T, o->p, o, n);
  o->l->p = n;
  if (o->r) o->r->p = n;
  return p;
}
static inline mnode_mut_t __mtree_pop_left_or_right(mtree_mut_t T, mnode_mut_t n) {
  mnode_mut_t c = n->l, p;
  if (!c) c = n->r;
  p = n->p;
  __mtree_child_replace(T, p, n, c);
  if (c) c->p = p;
  return p;
}
static inline void __mtree_pop_rebalance(mtree_mut_t T, mnode_mut_t n) {
  int64_t lh, rh, d, h;
  while (n) {
    lh = __lh(n);
    rh = __rh(n);
    h  = (lh > rh ? lh : rh) + 1;
    d  = lh - rh;
    if (n->h != h) n->h = h; else if (d >= -1 && d <= 1) break;
    if (d <= -2) {
      n = __mtree_fix_left(T, n);
    } else if (d >= 2) {
      n = __mtree_fix_right(T, n);
    }
    n = n->p;
  }
}
static inline void __mtree_put_rebalance(mtree_mut_t T, mnode_mut_t n) {
  int64_t lh, rh, d, h;
  for (n = n->p; n; n = n->p) {
    lh = __lh(n);
    rh = __rh(n);
    h  = (lh > rh ? lh : rh) + 1;
    d  = lh - rh;
    if (n->h == h) break;
    n->h = h;
    if (d <= -2) {
      n = __mtree_fix_left(T, n);
    } else if (d >= 2) {
      n = __mtree_fix_right(T, n);
    }
  }
}
static inline mnode_mut_t mtree_tear(mtree_mut_t T, mnode_mut_t* next) {
  mnode_mut_t n = *next, p;
  if (!n) {
    if (!T->root) return NULL;
    n = T->root;
  }
  while (!0) { if (n->l) n = n->l; else if (n->r) n = n->r; else break; }
  p = n->p;
  if (!p) {
    *next   = NULL;
    T->root = NULL;
    T->len = 0;
    return n;
  }
  if (p->l == n) p->l = NULL; else p->r = NULL;
  n->h  = 0;
  *next = p;
  return n;
}
static inline mnode_mut_t mtree_first(mtree_mut_t T) {
  mnode_mut_t n = T->root;
  if (!n) return NULL;
  while (n->l) n = n->l;
  return n;
}
static inline mnode_mut_t mtree_next(mnode_mut_t n) {
  mnode_mut_t last;
  if (n) {
    if (n->r) {
      n = n->r;
      while (n->l) n = n->l;
    } else {
      while (!0) {
        last = n;
        n    = n->p;
        if (!n || n->l == last) break;
      }
    }
  }
  return n;
}
static inline mnode_mut_t mtree_at(mtree_mut_t T, uint32_t hash, void* key, uu_cmp_fn cmp_fn) {
  mnode_mut_t l = T->root;
  int t;
  while (l) {
    if (likely(hash != l->hash)) l = (hash < l->hash) ? l->l : l->r; else {
      t = cmp_fn(key, &l->key[0]);
      if (likely(t != 0)) l = (t < 0) ? l->l : l->r; else return l;
    }
  }
  return NULL;
}
static inline void mtree_del(mtree_mut_t T, mnode_mut_t n) {
  if (likely(T->len == 1)) T->root = NULL;
  else if (T->len == 2) {
    if (T->root != n) T->root->l = T->root->r = NULL; else {
      T->root = (mnode_mut_t)((uintptr_t)(n->l) | (uintptr_t)(n->r));
      T->root->p = NULL;
    }
  } else {
    mnode_mut_t p = (n->l && n->r ? __mtree_pop_left_and_right : __mtree_pop_left_or_right)(T, n);
    if (p) __mtree_pop_rebalance(T, p);
  }
  T->len--;
}
static inline int mtree_add(mtree_mut_t T, mnode_mut_t n, uu_cmp_fn cmp_fn) {
  mnode_mut_t *l = &T->root, p = NULL;
  int t = 0;
  while (l[0]) {
    p = l[0];
    if (likely(n->hash != p->hash)) l = (n->hash < p->hash) ? &p->l : &p->r; else {
      t = cmp_fn(&n->key[0], &p->key[0]);
      if (likely(t != 0)) l = (t < 0) ? &p->l : &p->r; else return !!0;
    }
  }
  n->p = p;
  l[0] = n;
  if (likely(T->len == 1)) p->h++; else if (T->len > 1) __mtree_put_rebalance(T, n);
  T->len++;
  return !0;
}
/* clang-format on */

struct uu_dict_t;
typedef struct uu_dict_t* uu_dict_mut_t;
typedef const struct uu_dict_t* uu_dict_ref_t;
typedef struct uu_dict_t {
  uint32_t ksize;
  uint32_t len;

  uu_cmp_fn cmp_fn;

  mtree_mut_t buckets;
  mtree_mut_t obuckets;
  uint32_t buckets_mask;
  uint32_t obuckets_mask;
  uint32_t obuckets_idx;
  uint32_t seed;

  mtree_mut_t iter_bucket;
  mnode_mut_t iter_node;
} uu_dict_t;

static inline uint32_t MurmurHash3_32(const void* data, int len, uint32_t seed) {
  const int nblocks      = len / 4;
  uint32_t h1            = seed;
  uint32_t k1            = 0;
  uint32_t c1            = 0xcc9e2d51;
  uint32_t c2            = 0x1b873593;
  const uint32_t* blocks = (const uint32_t*)((const uint8_t*)data + nblocks * 4);
  const uint8_t* tail    = (const uint8_t*)((const uint8_t*)data + nblocks * 4);
  int i                  = 0;

  //----------
  // body

  for (i = -nblocks; i; i++) {
    k1 = blocks[i];
    k1 *= c1;
    k1 = (k1 << 15) | (k1 >> 17);
    k1 *= c2;

    h1 ^= k1;
    h1 = (h1 << 13) | (h1 >> 19);
    h1 = h1 * 5 + 0xe6546b64;
  }

  //----------
  // tail

  k1 = 0;
  switch (len & 3) {
    case 3: k1 ^= tail[2] << 16; /* fallthrough */
    case 2: k1 ^= tail[1] << 8;  /* fallthrough */
    case 1:
      k1 ^= tail[0];
      k1 *= c1;
      k1 = (k1 << 15) | (k1 >> 17);
      k1 *= c2;
      h1 ^= k1;

    default: break;
  };

  //----------
  // finalization

  h1 ^= len;
  h1 ^= h1 >> 16;
  h1 *= 0x85ebca6b;
  h1 ^= h1 >> 13;
  h1 *= 0xc2b2ae35;
  h1 ^= h1 >> 16;

  return h1;
}

static void __uu_dict_resize(uu_dict_mut_t self) {
  mtree_mut_t buckets  = NULL;
  uint32_t buckets_len = self->buckets_mask + 1;
  uint32_t limit       = self->len * 6 >> 2;

  uu_chk_if(likely(limit <= buckets_len));

  while (buckets_len < limit) {
    buckets_len <<= 1;
  }

  buckets = UU_MALLOC(sizeof(mtree_t) * buckets_len);
  uu_end_if(!buckets, err0);

  bzero(buckets, sizeof(mtree_t) * buckets_len);

  self->obuckets_idx  = 0;
  self->obuckets      = self->buckets;
  self->obuckets_mask = self->buckets_mask;
  self->buckets       = buckets;
  self->buckets_mask  = buckets_len - 1;

  return;

err0:
  return;
}

static void __uu_dict_rehash(uu_dict_mut_t self) {
  mtree_mut_t bucket  = NULL;
  mtree_mut_t obucket = NULL;
  mnode_mut_t node    = NULL;
  mnode_mut_t next    = NULL;

  uu_chk_if(likely(!self->obuckets));

  while (self->obuckets_idx <= self->obuckets_mask) {
    obucket = &self->obuckets[self->obuckets_idx++];
    if (!obucket->root) {
      continue;
    }

    while ((node = mtree_tear(obucket, &next))) {
      node->l = node->r = node->p = NULL;
      node->h                     = 1;

      bucket = &self->buckets[node->hash & self->buckets_mask];
      mtree_add(bucket, node, self->cmp_fn);
    }

    break;
  }

  if (self->obuckets_idx > self->obuckets_mask) {
    UU_FREE(self->obuckets);

    self->obuckets      = NULL;
    self->obuckets_mask = 0;
    self->obuckets_idx  = 0;
  }
}

void* __uu_dict_init(uint32_t ksize, uu_cmp_fn cmp_fn) {
  uu_dict_mut_t self = NULL;

  self = (uu_dict_mut_t)UU_MALLOC(sizeof(uu_dict_t));
  uu_end_if(!self, err0);

  self->buckets_mask = 7;

  self->buckets = (mtree_mut_t)UU_MALLOC(sizeof(mtree_t) * 8);
  uu_end_if(!self->buckets, err1);

  bzero(self->buckets, sizeof(mtree_t) * 8);

  self->ksize         = ksize;
  self->cmp_fn        = cmp_fn;
  self->len           = 0;
  self->iter_node     = NULL;
  self->iter_bucket   = NULL;
  self->obuckets      = NULL;
  self->obuckets_mask = 0;
  self->obuckets_idx  = 0;
  self->seed          = (uint32_t)(uintptr_t)self;

  return self;

err1:
  UU_FREE(self);
err0:
  return NULL;
}

void __uu_dict_clear(void* _self) {
  uu_dict_mut_t self = (uu_dict_mut_t)_self;
  mtree_mut_t bucket = NULL;
  mtree_mut_t begin  = NULL;
  mtree_mut_t end    = NULL;
  mnode_mut_t node   = NULL;
  mnode_mut_t next   = NULL;

  begin = &self->buckets[0];
  end   = &self->buckets[self->buckets_mask + 1];
  for (bucket = begin; bucket != end; bucket++) {
    while ((node = mtree_tear(bucket, &next))) {
      UU_FREE(node);
    }
  }

  if (self->obuckets) {
    begin = &self->obuckets[self->obuckets_idx];
    end   = &self->obuckets[self->obuckets_mask + 1];
    for (bucket = begin; bucket != end; bucket++) {
      while ((node = mtree_tear(bucket, &next))) {
        UU_FREE(node);
      }
    }

    UU_FREE(self->obuckets);

    self->obuckets      = NULL;
    self->obuckets_idx  = 0;
    self->obuckets_mask = 0;
  }

  self->len = 0;
}

void __uu_dict_deinit(void* _self) {
  uu_dict_mut_t self = (uu_dict_mut_t)_self;

  __uu_dict_clear(self);

  UU_FREE(self->buckets);
  UU_FREE(self);
}

uint32_t __uu_dict_len(void* _self) {
  uu_dict_mut_t self = (uu_dict_mut_t)_self;

  return self->len;
}

void* __uu_dict_at(void* _self, void* key) {
  uu_dict_mut_t self = (uu_dict_mut_t)_self;
  mtree_mut_t bucket = NULL;
  mnode_mut_t node   = NULL;
  uint32_t hash      = 0;

  uu_chk_if(self->len == 0, NULL);

  __uu_dict_rehash(self);

  hash   = MurmurHash3_32(key, self->ksize, self->seed);
  bucket = &self->buckets[hash & self->buckets_mask];
  node   = mtree_at(bucket, hash, key, self->cmp_fn);

  if (unlikely(!node && self->obuckets) && ((hash & self->obuckets_mask) >= self->obuckets_idx)) {
    bucket = &self->obuckets[hash & self->obuckets_mask];
    node   = mtree_at(bucket, hash, key, self->cmp_fn);
  }

  uu_end_if(unlikely(!node), err0);

  return (void*)node->uptr;

err0:
  return NULL;
}

int __uu_dict_insert(void* _self, void* key, void* uptr) {
  uu_dict_mut_t self = (uu_dict_mut_t)_self;
  mtree_mut_t bucket = NULL;
  mnode_mut_t node   = NULL;
  uint32_t hash      = 0;
  int result         = 0;

  __uu_dict_rehash(self);

  hash = MurmurHash3_32(key, self->ksize, self->seed);

  if (unlikely(self->obuckets) && (hash & self->obuckets_mask) >= self->obuckets_idx) {
    bucket = &self->obuckets[hash & self->obuckets_mask];
    node   = mtree_at(bucket, hash, key, self->cmp_fn);
  }

  uu_end_if(unlikely(node), err0);

  node = (mnode_mut_t)UU_MALLOC(sizeof(mnode_t) + self->ksize);
  uu_end_if(unlikely(!node), err0);

  node->l = node->r = node->p = NULL;
  node->h                     = 1;
  node->hash                  = hash;
  node->uptr                  = uptr;

  memcpy(&node->key[0], key, self->ksize);

  bucket = &self->buckets[hash & self->buckets_mask];
  result = mtree_add(bucket, node, self->cmp_fn);
  uu_end_if(unlikely(!result), err0);

  self->len++;

  __uu_dict_resize(self);

  return !0;

err0:
  return !!0;
}

void* __uu_dict_remove(void* _self, void* key) {
  uu_dict_mut_t self = (uu_dict_mut_t)_self;
  mtree_mut_t bucket = NULL;
  mnode_mut_t node   = NULL;
  void* uptr         = NULL;
  uint32_t hash      = 0;

  uu_chk_if(self->len == 0, NULL);

  __uu_dict_rehash(self);

  hash = MurmurHash3_32(key, self->ksize, self->seed);

  bucket = &self->buckets[hash & self->buckets_mask];
  node   = mtree_at(bucket, hash, key, self->cmp_fn);

  if (unlikely(!node && self->obuckets) && ((hash & self->obuckets_mask) >= self->obuckets_idx)) {
    bucket = &self->obuckets[hash & self->obuckets_mask];
    node   = mtree_at(bucket, hash, key, self->cmp_fn);
  }

  uu_end_if(!node, err0);

  mtree_del(bucket, node);

  uptr = (void*)node->uptr;

  UU_FREE(node);

  self->len--;

  return uptr;

err0:
  return NULL;
}

int __uu_dict_each(void* _self, int init, void* out[2]) {
  uu_dict_mut_t self = (uu_dict_mut_t)_self;
  mnode_mut_t iter   = NULL;

  uu_chk_if(self->len == 0, !!0);

  if (init) {
    self->iter_node   = NULL;
    self->iter_bucket = self->buckets;
    return !!0;
  }

  iter = self->iter_node;
  do {
    if (!iter) {
      iter = mtree_first(self->iter_bucket);
    } else {
      iter = mtree_next(iter);
    }

    if (iter) {
      break;
    }

    self->iter_bucket++;

    if (self->iter_bucket == self->buckets + self->buckets_mask + 1) {
      if (!self->obuckets) {
        break;
      }
      self->iter_bucket = self->obuckets;
    }
  } while (self->iter_bucket != (self->obuckets ? self->obuckets + self->obuckets_idx : NULL));

  uu_end_if(!iter, err0);

  self->iter_node = iter;

  out[0] = &iter->key[0];
  out[1] = (void*)iter->uptr;

  return !0;

err0:
  return !!0;
}

#if 0
#  define NODE_LEFT  0
#  define NODE_RIGHT 1

struct uu_nbl_t;
typedef struct uu_nbl_t* uu_nbl_mut_t;
typedef const struct uu_nbl_t* uu_nbl_ref_t;
typedef struct uu_nbl_t {
  uu_node_ref_t node;
  int i; /* backtracking point */
} uu_nbl_t;

void __uu_dict_node_dump(uu_node_ref_t node, uu_dict_dump_fn dump_fn) {
  uu_node_ref_t parent = node->parent;
  if (parent) {
    printf("%c(%d): ", node == parent->left ? 'L' : 'R', node->height);
  }

  dump_fn(node->key, node->uptr);

  printf("\n");
}

void __uu_dict_dump(void* _self, uu_dict_dump_fn dump_fn) {
  uu_dict_mut_t self = (uu_dict_mut_t)_self;
  uu_node_ref_t node = self->root;
  uu_nbl_t nbl;

  uu_vec(uu_nbl_t) stack = uu_vec_init(stack);
  uu_nbl_t sentinel      = {NULL, -1};

  int level = 0, sub_index;

  assert(self);

  printf("Dict = {len = %d, height = %d}\n", self->len, self->root->height);

  if (!dump_fn) {
    return;
  }

  while (1) {
    if (node) {
      /* 获取回溯点 */
      sub_index  = sentinel.i > -1 ? sentinel.i : NODE_LEFT;
      sentinel.i = -1;

      if ((!node->left && !node->right) || sub_index == NODE_RIGHT) {
        /* 叶子节点压栈空节点, 或者回溯到右子节点 */
        nbl = (uu_nbl_t){NULL, NODE_LEFT};
        uu_vec_insert_tail(stack, nbl);
      } else {
        /* 非叶子节点, 压栈当前节点, 同时记录下一个回溯点位置, 其实就是右子节点 */
        nbl = (uu_nbl_t){node, NODE_RIGHT};
        uu_vec_insert_tail(stack, nbl);
      }

      level++; /* 缩进层级 */

      /* 如果回溯位置索引为 0 */
      if (sub_index == 0) {
        for (int i = 1; i < level; i++) {
          if (i == level - 1) {
            printf("%-4s", "+---");
          } else {
            printf("%-4s", uu_vec_at(stack, i - 1).node ? "|" : " ");
          }
        }

        __uu_dict_node_dump(node, dump_fn);
      }

      node = sub_index == 0 ? node->left : node->right;
    } else {
      /* 栈为空, 回溯结束 */
      if (uu_vec_is_empty(stack)) {
        break;
      }

      /* 回溯节点 */
      sentinel = uu_vec_remove_tail(stack);
      node     = sentinel.node;
      level--;
    }
  }

  uu_vec_deinit(stack);
};
#endif
