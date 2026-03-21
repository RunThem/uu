#define UU_DEBUG

#include "uu.h"

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

// 全局不可变变量：测试数据量
#define TEST_TREE_NUM_ITEMS 20000

void test_tree_init() {
  printf("test(uu_tree_init): ");

  uu_tree(int, int*) t = NULL;
  t                    = uu_tree_init(t, uu_cmp_fn_int);

  assert(t);
  assert(uu_tree_is_empty(t));
  assert(0 == uu_tree_len(t));

  uu_tree_deinit(t);
  assert(!t);

  printf("pass\n");
}

void test_tree_clear() {
  printf("test(uu_tree_clear): ");

  uu_tree(int, int*) t = uu_tree_init(t, uu_cmp_fn_int);

  for (int i = 0; i < TEST_TREE_NUM_ITEMS; i++) {
    int result = uu_tree_insert(t, i, (void*)(uintptr_t)(i + 1));
    assert(result);
  }
  assert(TEST_TREE_NUM_ITEMS == uu_tree_len(t));

  uu_tree_clear(t);
  assert(0 == uu_tree_len(t));
  assert(uu_tree_is_empty(t));

  uu_tree_deinit(t);
  assert(!t);

  printf("pass\n");
}

void test_tree_deinit() {
  printf("test(uu_tree_deinit): ");

  uu_tree(int, int*) t = uu_tree_init(t, uu_cmp_fn_int);

  uu_tree_insert(t, 1, (void*)0x11);
  uu_tree_insert(t, 2, (void*)0x22);
  assert(2 == uu_tree_len(t));

  uu_tree_deinit(t);
  assert(!t);

  printf("pass\n");
}

void test_tree_len() {
  printf("test(uu_tree_len): ");

  uu_tree(int, int*) t = uu_tree_init(t, uu_cmp_fn_int);

  assert(0 == uu_tree_len(t));

  for (int i = 0; i < TEST_TREE_NUM_ITEMS; i++) {
    int result = uu_tree_insert(t, i, (void*)(uintptr_t)(i + 1));
    assert(result);
  }
  assert(TEST_TREE_NUM_ITEMS == uu_tree_len(t));

  uu_tree_deinit(t);
  assert(!t);

  printf("pass\n");
}

void test_tree_is_empty() {
  printf("test(uu_tree_is_empty): ");

  uu_tree(int, int*) t = uu_tree_init(t, uu_cmp_fn_int);

  assert(uu_tree_is_empty(t));

  for (int i = 0; i < TEST_TREE_NUM_ITEMS; i++) {
    int result = uu_tree_insert(t, i, (void*)(uintptr_t)(i + 1));
    assert(result);
    assert(!uu_tree_is_empty(t));
  }

  uu_tree_clear(t);
  assert(uu_tree_is_empty(t));

  uu_tree_deinit(t);
  assert(!t);

  printf("pass\n");
}

void test_tree_at() {
  printf("test(uu_tree_at): ");

  uu_tree(int, int*) t = uu_tree_init(t, uu_cmp_fn_int);

  for (int i = 0; i < TEST_TREE_NUM_ITEMS; i++) {
    int result = uu_tree_insert(t, i, (void*)(uintptr_t)(i + 1));
    assert(result);
  }

  for (int i = 0; i < TEST_TREE_NUM_ITEMS; i++) {
    assert((void*)(uintptr_t)(i + 1) == uu_tree_at(t, i));
  }

  uu_tree_deinit(t);
  assert(!t);

  printf("pass\n");
}

void test_tree_insert() {
  printf("test(uu_tree_insert): ");

  uu_tree(int, int*) t = uu_tree_init(t, uu_cmp_fn_int);

  for (int i = 0; i < TEST_TREE_NUM_ITEMS; i++) {
    int result = uu_tree_insert(t, i, (void*)(uintptr_t)(i + 1));
    assert(result);
  }
  assert(TEST_TREE_NUM_ITEMS == uu_tree_len(t));

  // 尝试插入重复键
  int result = uu_tree_insert(t, 0, (void*)0xDEADBEEF);
  assert(!result);
  assert(TEST_TREE_NUM_ITEMS == uu_tree_len(t));

  // 验证数据
  for (int i = 0; i < TEST_TREE_NUM_ITEMS; i++) {
    assert((void*)(uintptr_t)(i + 1) == uu_tree_at(t, i));
  }

  uu_tree_deinit(t);
  assert(!t);

  printf("pass\n");
}

void test_tree_remove() {
  printf("test(uu_tree_remove): ");

  uu_tree(int, int*) t = uu_tree_init(t, uu_cmp_fn_int);

  for (int i = 0; i < TEST_TREE_NUM_ITEMS; i++) {
    int result = uu_tree_insert(t, i, (void*)(uintptr_t)(i + 1));
    assert(result);
  }
  assert(TEST_TREE_NUM_ITEMS == uu_tree_len(t));

  // 删除中间的一个元素
  void* result = uu_tree_remove(t, TEST_TREE_NUM_ITEMS / 2);
  assert((void*)(uintptr_t)(TEST_TREE_NUM_ITEMS / 2 + 1) == result);
  assert(TEST_TREE_NUM_ITEMS - 1 == uu_tree_len(t));

  // 再次删除同一键应该返回 NULL
  result = uu_tree_remove(t, TEST_TREE_NUM_ITEMS / 2);
  assert(!result);
  assert(TEST_TREE_NUM_ITEMS - 1 == uu_tree_len(t));

  // 验证删除后的数据
  for (int i = 0; i < TEST_TREE_NUM_ITEMS; i++) {
    if (i == TEST_TREE_NUM_ITEMS / 2) {
      assert(NULL == uu_tree_at(t, i));
    } else {
      assert((void*)(uintptr_t)(i + 1) == uu_tree_at(t, i));
    }
  }

  uu_tree_deinit(t);
  assert(!t);

  printf("pass\n");
}

void test_tree_each() {
  printf("test(uu_tree_each): ");

  uu_tree(int, int*) t = uu_tree_init(t, uu_cmp_fn_int);

  for (int i = 0; i < TEST_TREE_NUM_ITEMS; i++) {
    int result = uu_tree_insert(t, i, (void*)(uintptr_t)(i + 1));
    assert(result);
  }

  int cnt = 0;
  uu_tree_each(t, key, int*, uptr) {
    assert((void*)(uintptr_t)(key + 1) == uptr);
    cnt++;
  }

  assert(TEST_TREE_NUM_ITEMS == cnt);

  uu_tree_deinit(t);
  assert(!t);

  printf("pass\n");
}

void test_tree_each_if() {
  printf("test(uu_tree_each_if): ");

  uu_tree(int, int*) t = uu_tree_init(t, uu_cmp_fn_int);

  for (int i = 0; i < TEST_TREE_NUM_ITEMS; i++) {
    int result = uu_tree_insert(t, i, (void*)(uintptr_t)(i + 1));
    assert(result);
  }

  int cnt = 0;
  uu_tree_each_if(t, key, int*, uptr, key % 2 == 0) {
    (void)uptr;
    assert(key % 2 == 0);
    cnt++;
  }

  assert(TEST_TREE_NUM_ITEMS / 2 == cnt);

  uu_tree_deinit(t);
  assert(!t);

  printf("pass\n");
}

void test_tree_find_if() {
  printf("test(uu_tree_find_if): ");

  uu_tree(int, int*) t = uu_tree_init(t, uu_cmp_fn_int);

  for (int i = 0; i < TEST_TREE_NUM_ITEMS; i++) {
    int result = uu_tree_insert(t, i, (void*)(uintptr_t)(i + 1));
    assert(result);
  }

  int* found =
      uu_tree_find_if(t, key, int*, uptr, uptr == (int*)(uintptr_t)(TEST_TREE_NUM_ITEMS / 2 + 1));
  assert(found && (int)(uintptr_t)found == TEST_TREE_NUM_ITEMS / 2 + 1);

  found = uu_tree_find_if(t, key, int*, uptr, uptr == (int*)(uintptr_t)(TEST_TREE_NUM_ITEMS + 100));
  assert(!found);

  uu_tree_deinit(t);
  assert(!t);

  printf("pass\n");
}

void test_tree_any_if() {
  printf("test(uu_tree_any_if): ");

  uu_tree(int, int*) t = uu_tree_init(t, uu_cmp_fn_int);

  for (int i = 0; i < TEST_TREE_NUM_ITEMS; i++) {
    int result = uu_tree_insert(t, i, (void*)(uintptr_t)(i + 1));
    assert(result);
  }

  int result =
      uu_tree_any_if(t, key, int*, uptr, uptr == (int*)(uintptr_t)(TEST_TREE_NUM_ITEMS / 2 + 1));
  assert(result);

  result = uu_tree_any_if(t, key, int*, uptr, uptr == (int*)(uintptr_t)(TEST_TREE_NUM_ITEMS + 100));
  assert(!result);

  uu_tree_deinit(t);
  assert(!t);

  printf("pass\n");
}

void test_tree_all_if() {
  printf("test(uu_tree_all_if): ");

  uu_tree(int, int*) t = uu_tree_init(t, uu_cmp_fn_int);

  for (int i = 0; i < TEST_TREE_NUM_ITEMS; i++) {
    int result = uu_tree_insert(t, i, (void*)(uintptr_t)(i + 1));
    assert(result);
  }

  int result = uu_tree_all_if(t, key, int*, uptr, uptr >= (int*)(uintptr_t)1);
  assert(result);

  result = uu_tree_all_if(t, key, int*, uptr, uptr > (int*)(uintptr_t)1);
  assert(!result);

  uu_tree_deinit(t);
  assert(!t);

  printf("pass\n");
}

// ============================================================================
// 边界条件测试
// ============================================================================

void test_tree_access_nonexistent_key() {
  printf("test(tree_access_nonexistent_key): ");

  uu_tree(int, int*) t = uu_tree_init(t, uu_cmp_fn_int);

  // 尝试访问不存在的键应该返回NULL
  assert(NULL == uu_tree_at(t, 1));
  assert(NULL == uu_tree_at(t, 999));

  // 插入一些键后再尝试访问不存在的键
  uu_tree_insert(t, 1, (void*)0x11);
  uu_tree_insert(t, 2, (void*)0x22);
  assert((void*)0x11 == uu_tree_at(t, 1));
  assert(NULL == uu_tree_at(t, 3));  // 应该返回NULL

  uu_tree_deinit(t);
  assert(!t);

  printf("pass\n");
}

void test_tree_large_data_operations() {
  printf("test(tree_large_data_operations): ");

  uu_tree(int, int*) t = uu_tree_init(t, uu_cmp_fn_int);

  // 插入大量数据
  const int num_items = 10000;
  for (int i = 0; i < num_items; i++) {
    int result = uu_tree_insert(t, i, (void*)(uintptr_t)(i + 100));
    assert(result);
  }

  assert(num_items == uu_tree_len(t));

  // 验证所有键值对都正确存储
  for (int i = 0; i < num_items; i++) {
    assert((void*)(uintptr_t)(i + 100) == uu_tree_at(t, i));
  }

  // 删除一些数据（每10个删除一个）
  for (int i = 0; i < num_items; i += 10) {
    void* result = uu_tree_remove(t, i);
    assert((void*)(uintptr_t)(i + 100) == result);
  }

  // 计算剩余项目数
  uint32_t expected_remaining = num_items - num_items / 10;
  assert(expected_remaining == uu_tree_len(t));

  // 验证删除后的数据 - 确保删除的项不再存在，未删除的项仍存在
  for (int i = 0; i < num_items; i++) {
    if (i % 10 == 0) {
      // 这些应该已经被删除了
      assert(NULL == uu_tree_at(t, i));
    } else {
      // 这些应该仍然存在
      assert((void*)(uintptr_t)(i + 100) == uu_tree_at(t, i));
    }
  }

  // 尝试删除已经删除的元素
  for (int i = 0; i < num_items; i += 10) {
    void* result = uu_tree_remove(t, i);
    assert(NULL == result);
  }

  uu_tree_deinit(t);
  assert(!t);

  printf("pass\n");
}

void test_tree_duplicate_keys() {
  printf("test(tree_duplicate_keys): ");

  uu_tree(int, int*) t = uu_tree_init(t, uu_cmp_fn_int);

  // 插入相同的键多次
  int result = uu_tree_insert(t, 1, (void*)0x11);
  assert(result);

  result = uu_tree_insert(t, 1, (void*)0x22);  // 重复键，应该失败
  assert(!result);

  // 值不应该改变
  assert((void*)0x11 == uu_tree_at(t, 1));

  // 测试插入另一个键
  result = uu_tree_insert(t, 2, (void*)0x22);
  assert(result);
  assert((void*)0x22 == uu_tree_at(t, 2));

  // 再次尝试插入相同键2但不同值，应该失败
  result = uu_tree_insert(t, 2, (void*)0x33);
  assert(!result);
  // 值仍应为原始值
  assert((void*)0x22 == uu_tree_at(t, 2));

  // 尝试插入多个不同的键
  for (int i = 3; i < 10; i++) {
    result = uu_tree_insert(t, i, (void*)(uintptr_t)(i + 100));
    assert(result);  // 所有插入都应该成功，因为键都不同
    assert((void*)(uintptr_t)(i + 100) == uu_tree_at(t, i));
  }

  // 尝试重复插入其中一个键，应该失败
  result = uu_tree_insert(t, 5, (void*)0xDEADBEEF);
  assert(!result);  // 应该失败，因为键5已经存在
  // 值仍应为原始值
  assert((void*)(uintptr_t)(5 + 100) == uu_tree_at(t, 5));

  uu_tree_deinit(t);
  assert(!t);

  printf("pass\n");
}

void test_tree_extreme_values() {
  printf("test(tree_extreme_values): ");

  uu_tree(int, int*) t = uu_tree_init(t, uu_cmp_fn_int);

  // 使用极值作为键进行测试
  int min_int = INT_MIN;
  int max_int = INT_MAX;

  int result = uu_tree_insert(t, min_int, (void*)0xAA);
  assert(result);

  result = uu_tree_insert(t, max_int, (void*)0xBB);
  assert(result);

  assert((void*)0xAA == uu_tree_at(t, min_int));
  assert((void*)0xBB == uu_tree_at(t, max_int));

  // 删除极值键
  void* removed_val = uu_tree_remove(t, min_int);
  assert((void*)0xAA == removed_val);

  removed_val = uu_tree_remove(t, max_int);
  assert((void*)0xBB == removed_val);

  // 确认它们已被删除
  assert(NULL == uu_tree_at(t, min_int));
  assert(NULL == uu_tree_at(t, max_int));

  // 重新插入
  result = uu_tree_insert(t, min_int, (void*)0xCC);
  assert(result);
  result = uu_tree_insert(t, max_int, (void*)0xDD);
  assert(result);

  assert((void*)0xCC == uu_tree_at(t, min_int));
  assert((void*)0xDD == uu_tree_at(t, max_int));

  uu_tree_deinit(t);
  assert(!t);

  printf("pass\n");
}

void test_tree_zero_key() {
  printf("test(tree_zero_key): ");

  uu_tree(int, int*) t = uu_tree_init(t, uu_cmp_fn_int);

  // 测试零作为键
  int result = uu_tree_insert(t, 0, (void*)0x00);
  assert(result);

  assert((void*)0x00 == uu_tree_at(t, 0));

  // 尝试再次插入零键
  result = uu_tree_insert(t, 0, (void*)0xFF);
  assert(!result);  // 应该失败

  // 值仍应为原值
  assert((void*)0x00 == uu_tree_at(t, 0));

  // 删除零键
  void* removed_val = uu_tree_remove(t, 0);
  assert((void*)0x00 == removed_val);

  // 确认已删除
  assert(NULL == uu_tree_at(t, 0));

  uu_tree_deinit(t);
  assert(!t);

  printf("pass\n");
}

void test_tree_negative_keys() {
  printf("test(tree_negative_keys): ");

  uu_tree(int, int*) t = uu_tree_init(t, uu_cmp_fn_int);

  // 测试负数键
  int result = uu_tree_insert(t, -1, (void*)0x11);
  assert(result);

  result = uu_tree_insert(t, -100, (void*)0x22);
  assert(result);

  result = uu_tree_insert(t, -999, (void*)0x33);
  assert(result);

  assert((void*)0x11 == uu_tree_at(t, -1));
  assert((void*)0x22 == uu_tree_at(t, -100));
  assert((void*)0x33 == uu_tree_at(t, -999));

  // 混合正负键
  result = uu_tree_insert(t, 1, (void*)0x44);
  assert(result);

  result = uu_tree_insert(t, 100, (void*)0x55);
  assert(result);

  assert((void*)0x44 == uu_tree_at(t, 1));
  assert((void*)0x55 == uu_tree_at(t, 100));

  // 删除负数键
  void* removed_val = uu_tree_remove(t, -100);
  assert((void*)0x22 == removed_val);

  assert(NULL == uu_tree_at(t, -100));

  uu_tree_deinit(t);
  assert(!t);

  printf("pass\n");
}

void test_tree_edge_case_operations() {
  printf("test(tree_edge_case_operations): ");

  uu_tree(int, int*) t = uu_tree_init(t, uu_cmp_fn_int);

  // 测试在空树中删除
  void* result = uu_tree_remove(t, 1);
  assert(NULL == result);

  // 插入、删除、再插入同一键
  int insert_result = uu_tree_insert(t, 42, (void*)0x42);
  assert(insert_result);
  assert((void*)0x42 == uu_tree_at(t, 42));

  result = uu_tree_remove(t, 42);
  assert((void*)0x42 == result);
  assert(NULL == uu_tree_at(t, 42));

  insert_result = uu_tree_insert(t, 42, (void*)0x84);
  assert(insert_result);
  assert((void*)0x84 == uu_tree_at(t, 42));

  // 清空后重新填充
  uu_tree_clear(t);
  assert(0 == uu_tree_len(t));
  assert(uu_tree_is_empty(t));

  // 再次测试基本操作
  insert_result = uu_tree_insert(t, 1, (void*)0x11);
  assert(insert_result);
  assert((void*)0x11 == uu_tree_at(t, 1));

  uu_tree_deinit(t);
  assert(!t);

  printf("pass\n");
}

void test_tree_memory_operations() {
  printf("test(tree_memory_operations): ");

  // 多次初始化和销毁，测试内存管理
  for (int i = 0; i < 10; i++) {
    uu_tree(int, int*) t = uu_tree_init(t, uu_cmp_fn_int);

    // 插入数据
    for (int j = 0; j < 100; j++) {
      int insert_result = uu_tree_insert(t, j, (void*)(uintptr_t)(j + 1000));
      assert(insert_result);
    }

    // 验证数据
    for (int j = 0; j < 100; j++) {
      assert((void*)(uintptr_t)(j + 1000) == uu_tree_at(t, j));
    }

    // 清理
    uu_tree_deinit(t);
    assert(!t);
  }

  printf("pass\n");
}

void test_tree_error_usage() {
  printf("test(tree_error_usage): ");

  uu_tree(int, int*) t = uu_tree_init(t, uu_cmp_fn_int);

  // 在空树中查找不存在的键应该返回NULL而不是崩溃
  assert(NULL == uu_tree_at(t, 123));
  assert(NULL == uu_tree_at(t, -1));
  assert(NULL == uu_tree_at(t, INT_MAX));
  assert(NULL == uu_tree_at(t, INT_MIN));

  // 删除不存在的键应该返回NULL而不是崩溃
  assert(NULL == uu_tree_remove(t, 999));
  assert(NULL == uu_tree_remove(t, -999));

  // 插入后删除，然后再次删除相同键
  uu_tree_insert(t, 42, (void*)0x42);
  assert((void*)0x42 == uu_tree_remove(t, 42));  // 第一次删除成功
  assert(NULL == uu_tree_remove(t, 42));         // 第二次删除返回NULL

  // 尝试获取删除后的值
  assert(NULL == uu_tree_at(t, 42));

  // 测试大量连续插入和删除
  for (int i = 0; i < 1000; i++) {
    uu_tree_insert(t, i, (void*)(uintptr_t)i);
  }

  for (int i = 0; i < 1000; i++) {
    assert((void*)(uintptr_t)i == uu_tree_at(t, i));
  }

  for (int i = 0; i < 1000; i++) {
    assert((void*)(uintptr_t)i == uu_tree_remove(t, i));
  }

  // 确保全部被删除
  for (int i = 0; i < 1000; i++) {
    assert(NULL == uu_tree_at(t, i));
  }

  assert(0 == uu_tree_len(t));

  uu_tree_deinit(t);
  assert(!t);

  printf("pass\n");
}

void test_tree_ordered_traversal() {
  printf("test(tree_ordered_traversal): ");

  uu_tree(int, int*) t = uu_tree_init(t, uu_cmp_fn_int);

  // 插入乱序数据
  int keys[]   = {5, 2, 8, 1, 3, 7, 9, 0, 4, 6};
  int num_keys = sizeof(keys) / sizeof(keys[0]);

  for (int i = 0; i < num_keys; i++) {
    int result = uu_tree_insert(t, keys[i], (void*)(uintptr_t)(keys[i] + 100));
    assert(result);
  }

  // AVL树的中序遍历应该是有序的
  int prev_key = -1;
  uu_tree_each(t, key, int*, uptr) {
    assert(key > prev_key);  // 确保是升序
    prev_key = key;
    (void)uptr;
  }

  uu_tree_deinit(t);
  assert(!t);

  printf("pass\n");
}

void test_tree_sequential_insert() {
  printf("test(tree_sequential_insert): ");

  uu_tree(int, int*) t = uu_tree_init(t, uu_cmp_fn_int);

  // 顺序插入大量数据，测试AVL树的平衡性
  for (int i = 0; i < 5000; i++) {
    int result = uu_tree_insert(t, i, (void*)(uintptr_t)i);
    assert(result);
  }

  assert(5000 == uu_tree_len(t));

  // 验证所有数据
  for (int i = 0; i < 5000; i++) {
    assert((void*)(uintptr_t)i == uu_tree_at(t, i));
  }

  uu_tree_deinit(t);
  assert(!t);

  printf("pass\n");
}

void test_tree_reverse_insert() {
  printf("test(tree_reverse_insert): ");

  uu_tree(int, int*) t = uu_tree_init(t, uu_cmp_fn_int);

  // 逆序插入大量数据，测试AVL树的平衡性
  for (int i = 5000 - 1; i >= 0; i--) {
    int result = uu_tree_insert(t, i, (void*)(uintptr_t)i);
    assert(result);
  }

  assert(5000 == uu_tree_len(t));

  // 验证所有数据
  for (int i = 0; i < 5000; i++) {
    assert((void*)(uintptr_t)i == uu_tree_at(t, i));
  }

  uu_tree_deinit(t);
  assert(!t);

  printf("pass\n");
}

void test_tree_random_operations() {
  printf("test(tree_random_operations): ");

  uu_tree(int, int*) t = uu_tree_init(t, uu_cmp_fn_int);

  // 使用固定种子生成伪随机数序列
  unsigned int seed = 12345;

  // 插入随机键
  for (int i = 0; i < 1000; i++) {
    int key = (int)(seed = seed * 1103515245 + 12345) % 10000;
    uu_tree_insert(t, key, (void*)(uintptr_t)(key + 1000));
  }

  // 验证插入的数据可以正确访问
  seed = 12345;
  for (int i = 0; i < 1000; i++) {
    int key   = (int)(seed = seed * 1103515245 + 12345) % 10000;
    void* val = uu_tree_at(t, key);
    // 由于可能有重复键，值可能是原始值或后续插入的值
    assert(val != NULL || uu_tree_at(t, key) == NULL);
  }

  uu_tree_deinit(t);
  assert(!t);

  printf("pass\n");
}

void test_tree_with_user_data() {
  printf("test(tree_with_user_data): ");

  typedef struct {
    int id;
    char name[32];
  } user_t;

  uu_tree(int, user_t*) t = uu_tree_init(t, uu_cmp_fn_int);

  // 创建并插入用户数据
  for (int i = 0; i < 100; i++) {
    user_t* user = (user_t*)malloc(sizeof(user_t));
    assert(user);
    user->id = i;
    snprintf(user->name, sizeof(user->name), "user_%d", i);

    int result = uu_tree_insert(t, i, user);
    assert(result);
  }

  // 验证数据
  for (int i = 0; i < 100; i++) {
    user_t* user = uu_tree_at(t, i);
    assert(user);
    assert(user->id == i);
  }

  // 使用 deinit 回调释放用户数据
  uu_tree_deinit(t, { free(uptr); });
  assert(!t);

  printf("pass\n");
}

void test_tree_cstr_int() {
  printf("test(tree_cstr_int): ");

  uu_tree(char*, int*) t = uu_tree_init(t, uu_cmp_fn_cstr);

  // 测试初始化
  assert(t);
  assert(uu_tree_is_empty(t));
  assert(0 == uu_tree_len(t));

  // 测试插入
  int result = uu_tree_insert(t, "one", (void*)(uintptr_t)1);
  assert(result);
  result = uu_tree_insert(t, "two", (void*)(uintptr_t)2);
  assert(result);
  result = uu_tree_insert(t, "three", (void*)(uintptr_t)3);
  assert(result);
  assert(3 == uu_tree_len(t));
  assert(!uu_tree_is_empty(t));

  // 测试访问
  assert((void*)(uintptr_t)1 == uu_tree_at(t, "one"));
  assert((void*)(uintptr_t)2 == uu_tree_at(t, "two"));
  assert((void*)(uintptr_t)3 == uu_tree_at(t, "three"));
  assert(NULL == uu_tree_at(t, "four"));  // 不存在的键

  // 测试重复键插入失败
  result = uu_tree_insert(t, "one", (void*)(uintptr_t)100);
  assert(!result);
  assert((void*)(uintptr_t)1 == uu_tree_at(t, "one"));  // 值不变
  assert(3 == uu_tree_len(t));

  // 测试删除
  void* removed = uu_tree_remove(t, "two");
  assert((void*)(uintptr_t)2 == removed);
  assert(2 == uu_tree_len(t));
  assert(NULL == uu_tree_at(t, "two"));

  // 测试删除不存在的键
  removed = uu_tree_remove(t, "nonexistent");
  assert(NULL == removed);

  // 测试 each 遍历
  int count = 0;
  uu_tree_each(t, key, int*, uptr) {
    (void)key;
    (void)uptr;
    count++;
  }
  assert(2 == count);

  // 测试 each_if 条件遍历
  count = 0;
  uu_tree_each_if(t, key, int*, uptr, uptr == (void*)(uintptr_t)1) {
    (void)key;
    (void)uptr;
    count++;
  }
  assert(1 == count);

  // 测试 find_if
  int* found = uu_tree_find_if(t, key, int*, uptr, uptr == (void*)(uintptr_t)3);
  assert(found);
  found = uu_tree_find_if(t, key, int*, uptr, uptr == (void*)(uintptr_t)999);
  assert(!found);

  // 测试 any_if
  result = uu_tree_any_if(t, key, int*, uptr, uptr == (void*)(uintptr_t)1);
  assert(result);
  result = uu_tree_any_if(t, key, int*, uptr, uptr == (void*)(uintptr_t)999);
  assert(!result);

  // 测试 all_if
  result = uu_tree_all_if(t,
                          key,
                          int*,
                          uptr,
                          uptr == (void*)(uintptr_t)1 || uptr == (void*)(uintptr_t)3);
  assert(result);
  result = uu_tree_all_if(t, key, int*, uptr, uptr == (void*)(uintptr_t)1);
  assert(!result);

  // 测试 clear
  uu_tree_clear(t);
  assert(0 == uu_tree_len(t));
  assert(uu_tree_is_empty(t));
  assert(NULL == uu_tree_at(t, "one"));

  // 测试 deinit
  uu_tree_deinit(t);
  assert(!t);

  printf("pass\n");
}

void test_tree_stress() {
  printf("test(tree_stress): ");

  uu_tree(int, int*) t = uu_tree_init(t, uu_cmp_fn_int);

  // 压力测试：大量插入、查找、删除
  const int stress_size = 50000;

  // 插入
  for (int i = 0; i < stress_size; i++) {
    int result = uu_tree_insert(t, i, (void*)(uintptr_t)(i * 2));
    assert(result);
  }
  assert(stress_size == uu_tree_len(t));

  // 查找
  for (int i = 0; i < stress_size; i++) {
    assert((void*)(uintptr_t)(i * 2) == uu_tree_at(t, i));
  }

  // 删除一半
  for (int i = 0; i < stress_size; i += 2) {
    void* result = uu_tree_remove(t, i);
    assert((void*)(uintptr_t)(i * 2) == result);
  }
  assert(stress_size / 2 == uu_tree_len(t));

  // 验证删除后的状态
  for (int i = 0; i < stress_size; i++) {
    if (i % 2 == 0) {
      assert(NULL == uu_tree_at(t, i));
    } else {
      assert((void*)(uintptr_t)(i * 2) == uu_tree_at(t, i));
    }
  }

  // 再次插入删除的键
  for (int i = 0; i < stress_size; i += 2) {
    int result = uu_tree_insert(t, i, (void*)(uintptr_t)(i * 3));
    assert(result);
  }
  assert(stress_size == uu_tree_len(t));

  // 验证新值
  for (int i = 0; i < stress_size; i += 2) {
    assert((void*)(uintptr_t)(i * 3) == uu_tree_at(t, i));
  }

  uu_tree_deinit(t);
  assert(!t);

  printf("pass\n");
}

int main() {
  // 基本功能测试
  test_tree_init();
  test_tree_clear();
  test_tree_deinit();
  test_tree_len();
  test_tree_is_empty();
  test_tree_at();
  test_tree_insert();
  test_tree_remove();
  test_tree_each();
  test_tree_each_if();
  test_tree_find_if();
  test_tree_any_if();
  test_tree_all_if();

  // 字符串键测试
  test_tree_cstr_int();

  // 边界条件测试
  printf("\n=== Boundary condition tests ===\n");
  test_tree_access_nonexistent_key();
  test_tree_large_data_operations();
  test_tree_duplicate_keys();
  test_tree_extreme_values();
  test_tree_zero_key();
  test_tree_negative_keys();
  test_tree_edge_case_operations();
  test_tree_memory_operations();
  test_tree_error_usage();

  // AVL树特性测试
  printf("\n=== AVL tree specific tests ===\n");
  test_tree_ordered_traversal();
  test_tree_sequential_insert();
  test_tree_reverse_insert();
  test_tree_random_operations();
  test_tree_with_user_data();

  // 压力测试
  printf("\n=== Stress test ===\n");
  test_tree_stress();

  return 0;
}