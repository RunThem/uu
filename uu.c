#include "uu.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************************************************
 * Macro
 **************************************************************************************************/
#define UU_DICT_NBL_STACK_MAX 64

#if defined(__GNUC__) || defined(__clang__)
#  define likely(x)   __builtin_expect(!!(x), 1)
#  define unlikely(x) __builtin_expect(!!(x), 0)
#else
#  define likely(x)   (x)
#  define unlikely(x) (x)
#endif

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
 * Hash function
 **************************************************************************************************/
inline uint32_t uu_hash_fn_fnv1a(const void* data, uint32_t len, uint32_t seed) {
  const uint8_t* bytes = (const uint8_t*)data;
  uint32_t hash        = 0x811c9dc5;

  hash ^= seed;
  hash *= 0x01000193;

  for (uint32_t i = 0; i < len; i++) {
    hash ^= bytes[i];
    hash *= 0x01000193;
  }

  return hash;
}

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
struct uu_node_t;
typedef struct uu_node_t* uu_node_mut_t;
typedef const struct uu_node_t* uu_node_ref_t;
typedef struct uu_node_t {
  uu_node_mut_t l, r, p;
  uint32_t h;

  uint32_t hash;
  void* uptr;
  uint8_t key[0];
} uu_node_t;

struct uu_tree_t;
typedef struct uu_tree_t* uu_tree_mut_t;
typedef const struct uu_tree_t* uu_tree_ref_t;
typedef struct uu_tree_t {
  uint32_t ksize;
  uint32_t len;

  uu_cmp_fn cmp_fn;

  uu_node_mut_t root;
  uu_node_mut_t iter;
} uu_tree_t;

#define __lh(n) (n->l ? n->l->h : 0)
#define __rh(n) (n->r ? n->r->h : 0)

static inline void __uu_tree_child_replace(uu_node_mut_t* root,
                                           uu_node_mut_t p,
                                           uu_node_mut_t o,
                                           uu_node_mut_t n) {
  if (!p) {
    root[0] = n;
  } else {
    *(p->l == o ? &p->l : &p->r) = n;
  }
}

static inline uu_node_mut_t __uu_tree_rotate_left(uu_node_mut_t* root, uu_node_mut_t n) {
  uint32_t lh     = __lh(n);
  uu_node_mut_t r = n->r, p = n->p, rl = r->l;

  // 交换指针
  n->p = r;
  n->r = rl;
  r->p = p;
  r->l = n;
  __uu_tree_child_replace(root, p, n, r);

  // 更新高度
  if (rl) {
    rl->p = n;
    n->h  = (lh > rl->h ? lh : rl->h) + 1;
  } else {
    n->h = lh + 1;
  }

  lh   = __lh(r);
  r->h = (lh > n->h ? lh : n->h) + 1;

  return r;
}

static inline uu_node_mut_t __uu_tree_rotate_right(uu_node_mut_t* root, uu_node_mut_t n) {
  uint32_t rh     = __rh(n);
  uu_node_mut_t l = n->l, p = n->p, lr = l->r;

  // 交换指针
  n->p = l;
  n->l = lr;
  l->p = p;
  l->r = n;
  __uu_tree_child_replace(root, p, n, l);

  // 更新高度
  if (lr) {
    lr->p = n;
    n->h  = (lr->h > rh ? lr->h : rh) + 1;
  } else {
    n->h = rh + 1;
  }

  rh   = __rh(l);
  l->h = (n->h > rh ? n->h : rh) + 1;

  return l;
}

static inline uu_node_mut_t __uu_tree_fix_left(uu_node_mut_t* root, uu_node_mut_t n) {
  if (__lh(n->r) > __rh(n->r)) {
    __uu_tree_rotate_right(root, n->r);
  }

  return __uu_tree_rotate_left(root, n);
}

static inline uu_node_mut_t __uu_tree_fix_right(uu_node_mut_t* root, uu_node_mut_t n) {
  if (__lh(n->l) < __rh(n->l)) {
    __uu_tree_rotate_left(root, n->l);
  }

  return __uu_tree_rotate_right(root, n);
}

static inline uu_node_mut_t __uu_tree_remove_left_and_right(uu_node_mut_t* root, uu_node_mut_t n) {
  uu_node_mut_t z = n->r, c;

  while (z->l) {
    z = z->l;
  }

  c = z;
  if (z != n->r) {
    // 覆盖返回值
    c = z->p;

    // 更新 Z 节点附件节点的指针
    z->p->l = z->r;
    if (z->r) {
      z->r->p = z->p;
    }

    // 更新 N 节点右子节点的指针
    n->r->p = z;
    z->r    = n->r;
  }

  // 更新余下指针
  z->l    = n->l;
  z->p    = n->p;
  z->h    = n->h;
  n->l->p = z;
  __uu_tree_child_replace(root, n->p, n, z);

  return c;
}

static inline uu_node_mut_t __uu_tree_remove_left_or_right(uu_node_mut_t* root, uu_node_mut_t n) {
  uu_node_mut_t c = n->l ? n->l : n->r, p = n->p;

  if (c) {
    c->p = p;
  }

  __uu_tree_child_replace(root, p, n, c);

  return p;
}

static inline void __uu_tree_remove_rebalance(uu_node_mut_t* root, uu_node_mut_t n) {
  int64_t lh, rh, d, h;

  for (; n; n = n->p) {
    lh   = __lh(n);
    rh   = __rh(n);
    h    = (lh > rh ? lh : rh) + 1;
    d    = lh - rh;
    n->h = h;

    if (d <= -2) {
      n = __uu_tree_fix_left(root, n);
    } else if (d >= 2) {
      n = __uu_tree_fix_right(root, n);
    } else {
      break;
    }
  }
}

static inline void __uu_tree_insert_rebalance(uu_node_mut_t* root, uu_node_mut_t n) {
  int64_t lh, rh, d, h;

  for (n = n->p; n; n = n->p) {
    lh = __lh(n);
    rh = __rh(n);
    h  = (lh > rh ? lh : rh) + 1;
    d  = lh - rh;

    if (n->h == h) {
      break;
    }

    n->h = h;

    if (d <= -2) {
      n = __uu_tree_fix_left(root, n);
    } else if (d >= 2) {
      n = __uu_tree_fix_right(root, n);
    }
  }
}

static inline uu_node_mut_t __uu_tree_first(uu_node_mut_t* root) {
  uu_node_mut_t n = root[0];

  if (!n) {
    return NULL;
  }

  while (n->l) {
    n = n->l;
  }

  return n;
}

static inline uu_node_mut_t __uu_tree_next(uu_node_mut_t n) {
  uu_node_mut_t last;

  if (n) {
    if (n->r) {
      n = n->r;

      while (n->l) {
        n = n->l;
      }
    } else {
      while (!0) {
        last = n;
        n    = n->p;

        if (!n || n->l == last) {
          break;
        }
      }
    }
  }

  return n;
}

static uu_node_mut_t __uu_tree_remove_tear(uu_node_mut_t* root, uu_node_mut_t* next) {
  uu_node_mut_t n = *next, p;

  if (!n) {
    if (!root[0]) {
      return NULL;
    }
    n = root[0];
  }

  while (!0) {
    if (n->l) {
      n = n->l;
    } else if (n->r) {
      n = n->r;
    } else {
      break;
    }
  }

  p = n->p;
  if (!p) {
    next[0] = root[0] = NULL;
    return n;
  }

  if (p->l == n) {
    p->l = NULL;
  } else {
    p->r = NULL;
  }

  n->h  = 0;
  *next = p;

  return n;
}

void* __uu_tree_init(uint32_t ksize, uu_cmp_fn cmp_fn) {
  uu_tree_mut_t self = NULL;

  self = (uu_tree_mut_t)UU_MALLOC(sizeof(uu_tree_t));
  uu_end_if(!self, err0);

  self->cmp_fn = cmp_fn;
  self->len    = 0;
  self->ksize  = ksize;
  self->root   = NULL;
  self->iter   = NULL;

  return self;

err0:
  return NULL;
}

void __uu_tree_clear(void* _self) {
  uu_tree_mut_t self = (uu_tree_mut_t)_self;
  uu_node_mut_t node = NULL;
  uu_node_mut_t next = NULL;

  while ((node = __uu_tree_remove_tear(&self->root, &next))) {
    UU_FREE(node);
  }

  self->len = 0;
}

void __uu_tree_deinit(void* _self) {
  uu_tree_mut_t self = (uu_tree_mut_t)_self;

  __uu_tree_clear(self);

  UU_FREE(self);
}

uint32_t __uu_tree_len(void* _self) {
  uu_tree_mut_t self = (uu_tree_mut_t)_self;

  return self->len;
}

void* __uu_tree_at(void* _self, void* key) {
  uu_tree_mut_t self = (uu_tree_mut_t)_self;
  uu_node_mut_t i    = self->root;
  int result         = 0;

  while (i) {
    result = self->cmp_fn(key, &i->key[0]);
    if (!result) {
      return i->uptr;
    }

    i = (result < 0) ? i->l : i->r;
  }

  return NULL;
}

int __uu_tree_insert(void* _self, void* key, void* uptr) {
  uu_tree_mut_t self  = (uu_tree_mut_t)_self;
  uu_node_mut_t* link = &self->root;
  uu_node_mut_t node  = NULL;
  uu_node_mut_t p     = NULL;
  int result          = 0;

  while (link[0]) {
    p      = link[0];
    result = self->cmp_fn(key, &p->key[0]);
    uu_end_if(!result, err0);

    link = (result < 0) ? &p->l : &p->r;
  }

  node = (uu_node_mut_t)UU_MALLOC(sizeof(uu_node_t));
  uu_end_if(unlikely(!node), err0);

  node->h    = 1;
  node->uptr = uptr;
  node->p    = p;
  node->l    = NULL;
  node->r    = NULL;
  link[0]    = node;

  memcpy(&node->key[0], key, self->ksize);

  __uu_tree_insert_rebalance(&self->root, node);

  self->len++;

  return !0;

err0:
  return !!0;
}

void* __uu_tree_remove(void* _self, void* key) {
  uu_tree_mut_t self           = (uu_tree_mut_t)_self;
  uu_node_mut_t node           = self->root;
  uu_node_mut_t rebalance_node = NULL;
  void* uptr                   = NULL;
  int result                   = 0;

  while (node) {
    result = self->cmp_fn(key, &node->key[0]);
    if (!result) {
      break;
    }

    node = (result < 0) ? node->l : node->r;
  }

  uu_end_if(!node, err0);

  uptr = node->uptr;

  rebalance_node = (node->l && node->r ? __uu_tree_remove_left_and_right :
                                         __uu_tree_remove_left_or_right)(&self->root, node);

  if (rebalance_node) {
    __uu_tree_remove_rebalance(&self->root, rebalance_node);
  }

  UU_FREE(node);

  self->len--;

  return uptr;

err0:
  return NULL;
}

int __uu_tree_each(void* _self, int init, void* out[2]) {
  uu_tree_mut_t self = (uu_tree_mut_t)_self;
  uu_node_mut_t iter = NULL;

  uu_chk_if(self->len == 0, !!0);

  if (init) {
    self->iter = NULL;
    return !0;
  }

  if (self->iter == NULL) {
    iter = __uu_tree_first(&self->root);
  } else {
    iter = __uu_tree_next(self->iter);
  }

  uu_end_if(!iter, err0);

  self->iter = iter;

  out[0] = &iter->key[0];
  out[1] = (void*)iter->uptr;

  return !0;

err0:
  return !!0;
}

/***************************************************************************************************
 * Dict
 **************************************************************************************************/
struct uu_dict_t;
typedef struct uu_dict_t* uu_dict_mut_t;
typedef const struct uu_dict_t* uu_dict_ref_t;
typedef struct uu_dict_t {
  uint32_t ksize;
  uint32_t len;

  uu_cmp_fn cmp_fn;
  uu_hash_fn hash_fn;

  uu_node_mut_t* buckets;
  uu_node_mut_t* obuckets;
  uu_node_mut_t* ibucket;
  uu_node_mut_t inode;
  uint32_t buckets_mask;
  uint32_t obuckets_mask;
  uint32_t obuckets_idx;
  uint32_t seed;
} uu_dict_t;

static inline uu_node_mut_t
    __uu_dict_tree_at(uu_node_mut_t* root, uint32_t hash, void* key, uu_cmp_fn cmp_fn) {
  uu_node_mut_t i = root[0];
  int t;

  while (i) {
    if (likely(hash != i->hash)) {
      i = (hash < i->hash) ? i->l : i->r;
    } else {
      t = cmp_fn(key, &i->key[0]);
      if (likely(t != 0)) {
        i = (t < 0) ? i->l : i->r;
      } else {
        return i;
      }
    }
  }

  return NULL;
}

static inline void __uu_dict_tree_remove(uu_node_mut_t* root, uu_node_mut_t n) {
  if (likely(!root[0]->l && !root[0]->r)) {
    root[0] = NULL;
  } else if ((root[0]->l && !root[0]->r) || (!root[0]->l && root[0]->r)) {
    if (root[0] != n) {
      root[0]->l = root[0]->r = NULL;
    } else {
      root[0]    = (uu_node_mut_t)((uintptr_t)(n->l) | (uintptr_t)(n->r));
      root[0]->p = NULL;
    }
  } else {
    uu_node_mut_t p =
        (n->l && n->r ? __uu_tree_remove_left_and_right : __uu_tree_remove_left_or_right)(root, n);
    if (p) {
      __uu_tree_remove_rebalance(root, p);
    }
  }
}

static inline int __uu_dict_tree_insert(uu_node_mut_t* root, uu_node_mut_t n, uu_cmp_fn cmp_fn) {
  uu_node_mut_t *link = root, p = NULL;
  int t = 0;

  while (link[0]) {
    p = link[0];
    if (likely(n->hash != p->hash)) {
      link = (n->hash < p->hash) ? &p->l : &p->r;
    } else {
      t = cmp_fn(&n->key[0], &p->key[0]);
      if (likely(t != 0)) {
        link = (t < 0) ? &p->l : &p->r;
      } else {
        return !!0;
      }
    }
  }

  n->p    = p;
  link[0] = n;

  if (link != root) {
    if (likely(root[0]->l && !root[0]->r) || (!root[0]->l && root[0]->r)) {
      p->h++;
    } else {
      __uu_tree_insert_rebalance(root, n);
    }
  }

  return !0;
}

static void __uu_dict_resize(uu_dict_mut_t self) {
  uu_node_mut_t* buckets = NULL;
  uint32_t buckets_len   = self->buckets_mask + 1;
  uint32_t limit         = self->len * 6 >> 2;

  uu_chk_if(likely(limit <= buckets_len || self->obuckets));

  while (buckets_len < limit) {
    buckets_len <<= 1;
  }

  buckets = UU_MALLOC(sizeof(uu_node_mut_t) * buckets_len);
  uu_end_if(!buckets, err0);

  bzero(buckets, sizeof(uu_node_mut_t) * buckets_len);

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
  uu_node_mut_t* bucket  = NULL;
  uu_node_mut_t* obucket = NULL;
  uu_node_mut_t node     = NULL;
  uu_node_mut_t next     = NULL;
  int result             = 0;

  uu_chk_if(likely(!self->obuckets));

  while (self->obuckets_idx <= self->obuckets_mask) {
    obucket = &self->obuckets[self->obuckets_idx++];
    if (!obucket[0]) {
      continue;
    }

    while ((node = __uu_tree_remove_tear(obucket, &next))) {
      node->l = node->r = node->p = NULL;
      node->h                     = 1;

      bucket = &self->buckets[node->hash & self->buckets_mask];
      result = __uu_dict_tree_insert(bucket, node, self->cmp_fn);

      assert(result);
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

void* __uu_dict_init(uint32_t ksize, uu_cmp_fn cmp_fn, uu_hash_fn hash_fn) {
  uu_dict_mut_t self = NULL;

  self = (uu_dict_mut_t)UU_MALLOC(sizeof(uu_dict_t));
  uu_end_if(!self, err0);

  self->buckets_mask = 127;

  self->buckets = (uu_node_mut_t*)UU_MALLOC(sizeof(uu_node_mut_t) * (self->buckets_mask + 1));
  uu_end_if(!self->buckets, err1);

  bzero(self->buckets, sizeof(uu_node_mut_t) * (self->buckets_mask + 1));

  self->ksize         = ksize;
  self->cmp_fn        = cmp_fn;
  self->hash_fn       = hash_fn ? hash_fn : uu_hash_fn_fnv1a;
  self->len           = 0;
  self->inode         = NULL;
  self->ibucket       = NULL;
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
  uu_dict_mut_t self    = (uu_dict_mut_t)_self;
  uu_node_mut_t* bucket = NULL;
  uu_node_mut_t* begin  = NULL;
  uu_node_mut_t* end    = NULL;
  uu_node_mut_t node    = NULL;
  uu_node_mut_t next    = NULL;

  begin = &self->buckets[0];
  end   = &self->buckets[self->buckets_mask + 1];
  for (bucket = begin; bucket != end; bucket++) {
    while ((node = __uu_tree_remove_tear(bucket, &next))) {
      UU_FREE(node);
    }
  }

  if (self->obuckets) {
    begin = &self->obuckets[self->obuckets_idx];
    end   = &self->obuckets[self->obuckets_mask + 1];
    for (bucket = begin; bucket != end; bucket++) {
      while ((node = __uu_tree_remove_tear(bucket, &next))) {
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
  uu_dict_mut_t self    = (uu_dict_mut_t)_self;
  uu_node_mut_t* bucket = NULL;
  uu_node_mut_t node    = NULL;
  uint32_t hash         = 0;

  uu_chk_if(self->len == 0, NULL);

  __uu_dict_rehash(self);

  hash   = self->hash_fn(key, self->ksize, self->seed);
  bucket = &self->buckets[hash & self->buckets_mask];
  node   = __uu_dict_tree_at(bucket, hash, key, self->cmp_fn);

  if (unlikely(!node && self->obuckets) && ((hash & self->obuckets_mask) >= self->obuckets_idx)) {
    bucket = &self->obuckets[hash & self->obuckets_mask];
    node   = __uu_dict_tree_at(bucket, hash, key, self->cmp_fn);
  }

  uu_end_if(!node, err0);

  return (void*)node->uptr;

err0:
  return NULL;
}

int __uu_dict_insert(void* _self, void* key, void* uptr) {
  uu_dict_mut_t self    = (uu_dict_mut_t)_self;
  uu_node_mut_t* bucket = NULL;
  uu_node_mut_t node    = NULL;
  uint32_t hash         = 0;
  int result            = 0;

  __uu_dict_rehash(self);

  hash = self->hash_fn(key, self->ksize, self->seed);

  if (unlikely(self->obuckets) && (hash & self->obuckets_mask) >= self->obuckets_idx) {
    bucket = &self->obuckets[hash & self->obuckets_mask];
    node   = __uu_dict_tree_at(bucket, hash, key, self->cmp_fn);
  }

  uu_end_if(node, err0);

  node = (uu_node_mut_t)UU_MALLOC(sizeof(uu_node_t) + self->ksize);
  uu_end_if(unlikely(!node), err0);

  node->l = node->r = node->p = NULL;
  node->h                     = 1;
  node->hash                  = hash;
  node->uptr                  = uptr;

  memcpy(&node->key[0], key, self->ksize);

  bucket = &self->buckets[hash & self->buckets_mask];
  result = __uu_dict_tree_insert(bucket, node, self->cmp_fn);
  uu_end_if(!result, err0);

  self->len++;

  __uu_dict_resize(self);

  return !0;

err0:
  return !!0;
}

void* __uu_dict_remove(void* _self, void* key) {
  uu_dict_mut_t self    = (uu_dict_mut_t)_self;
  uu_node_mut_t* bucket = NULL;
  uu_node_mut_t node    = NULL;
  void* uptr            = NULL;
  uint32_t hash         = 0;

  uu_chk_if(self->len == 0, NULL);

  __uu_dict_rehash(self);

  hash = self->hash_fn(key, self->ksize, self->seed);

  bucket = &self->buckets[hash & self->buckets_mask];
  node   = __uu_dict_tree_at(bucket, hash, key, self->cmp_fn);

  if (unlikely(!node && self->obuckets) && ((hash & self->obuckets_mask) >= self->obuckets_idx)) {
    bucket = &self->obuckets[hash & self->obuckets_mask];
    node   = __uu_dict_tree_at(bucket, hash, key, self->cmp_fn);
  }

  uu_end_if(!node, err0);

  uptr = (void*)node->uptr;

  __uu_dict_tree_remove(bucket, node);

  UU_FREE(node);

  self->len--;

  return uptr;

err0:
  return NULL;
}

int __uu_dict_each(void* _self, int init, void* out[2]) {
  uu_dict_mut_t self = (uu_dict_mut_t)_self;
  uu_node_mut_t iter = NULL;

  uu_chk_if(self->len == 0, !!0);

  if (unlikely(init)) {
    self->inode   = NULL;
    self->ibucket = self->buckets;

    while (self->obuckets) {
      __uu_dict_rehash(self);
    }

    return !!0;
  }

  for (iter = self->inode; self->ibucket != self->buckets + self->buckets_mask + 1;
       self->ibucket++) {
    if (!iter) {
      iter = __uu_tree_first(self->ibucket);
    } else {
      iter = __uu_tree_next(iter);
    }

    if (iter) {
      break;
    }
  }

  uu_end_if(!iter, err0);

  self->inode = iter;

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
