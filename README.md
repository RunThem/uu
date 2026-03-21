# UU — U Utils Library

UU is a C generic container library providing three fundamental data structures:
UU 是一个 C 语言通用容器库, 提供三种基础数据结构:

- **`uu_vec`** - Type-safe dynamic array (vector) / 类型安全的动态数组(向量)
- **`uu_dict`** - Dictionary (key-value map) based on hash table and AVL tree / 基于哈希表和 AVL 树实现的字典(键值对映射)
- **`uu_tree`** - Ordered map implemented with AVL tree / AVL 树实现的有序映射

## Quick Start / 快速开始

### Compiler Requirements / 编译器要求

- Requires GCC extensions: `__typeof__` and statement expressions `({ ... })`
- Recommended: GCC or Clang

### Build & Test / 构建与测试

```bash
make test      # Run all tests / 运行所有测试
make bench     # Run benchmarks / 性能基准测试
make clean     # Clean build artifacts / 清理
```

## API Examples / API 示例

### Vector (uu_vec)

```c
uu_vec(int) v = uu_vec_init(v);

uu_vec_insert_tail(v, 10);
uu_vec_insert_tail(v, 20);

uu_vec_each(v, it) {
  printf("%d\n", it);
}

uu_vec_deinit(v);
```

### Dictionary (uu_dict)

```c
uu_dict(int, char*) d = uu_dict_init(d, uu_cmp_fn_int, NULL);

uu_dict_insert(d, 1, "one");
uu_dict_insert(d, 2, "two");

printf("%s\n", uu_dict_at(d, 1));  // "one"

uu_dict_deinit(d);
```

### Tree (uu_tree)

```c
uu_tree(int, char*) t = uu_tree_init(t, uu_cmp_fn_int);

uu_tree_insert(t, 2, "two");
uu_tree_insert(t, 1, "one");

uu_tree_each(t, k, v) {
  printf("%d -> %s\n", k, v);  // Ordered traversal / 有序遍历
}

uu_tree_deinit(t);
```

## Predefined Functions / 预定义函数

```c
// Comparison functions / 比较函数
uu_cmp_fn_int     // int
uu_cmp_fn_int64   // int64_t
uu_cmp_fn_cstr    // char*

// Hash functions / 哈希函数
uu_hash_fn_cstr   // char*
```

## License / 许可证

See the `LICENSE` file in this repository.
请参阅仓库中的 `LICENSE` 文件.
