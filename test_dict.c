#define UU_DEBUG

#include "uu.h"

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

uu_cmp_fn_def(int, x, y, x - y);

void test_dict_init() {
  printf("test(uu_dict_init): ");

  uu_dict(int, int*) d = NULL;
  d                    = uu_dict_init(d, cmp_fn_int);

  assert(d);
  assert(uu_dict_is_empty(d));
  assert(0 == uu_dict_len(d));

  uu_dict_deinit(d);
  assert(!d);

  printf("pass\n");
}

void test_dict_clear() {
  printf("test(uu_dict_clear): ");

  uu_dict(int, int*) d = uu_dict_init(d, cmp_fn_int);

  uu_dict_insert(d, 1, (void*)0x11);
  uu_dict_insert(d, 2, (void*)0x22);
  assert(2 == uu_dict_len(d));

  uu_dict_clear(d);
  assert(0 == uu_dict_len(d));
  assert(uu_dict_is_empty(d));

  uu_dict_deinit(d);
  assert(!d);

  printf("pass\n");
}

void test_dict_deinit() {
  printf("test(uu_dict_deinit): ");

  uu_dict(int, int*) d = uu_dict_init(d, cmp_fn_int);

  uu_dict_insert(d, 1, (void*)0x11);
  uu_dict_insert(d, 2, (void*)0x22);
  assert(2 == uu_dict_len(d));

  uu_dict_deinit(d);
  assert(!d);

  printf("pass\n");
}

void test_dict_len() {
  printf("test(uu_dict_len): ");

  uu_dict(int, int*) d = uu_dict_init(d, cmp_fn_int);

  assert(0 == uu_dict_len(d));

  uu_dict_insert(d, 1, (void*)0x11);
  assert(1 == uu_dict_len(d));

  uu_dict_insert(d, 2, (void*)0x22);
  assert(2 == uu_dict_len(d));

  uu_dict_insert(d, 3, (void*)0x33);
  assert(3 == uu_dict_len(d));

  uu_dict_deinit(d);
  assert(!d);

  printf("pass\n");
}

void test_dict_is_empty() {
  printf("test(uu_dict_is_empty): ");

  uu_dict(int, int*) d = uu_dict_init(d, cmp_fn_int);

  assert(uu_dict_is_empty(d));

  uu_dict_insert(d, 1, (void*)0x11);
  assert(!uu_dict_is_empty(d));

  uu_dict_insert(d, 2, (void*)0x22);
  assert(!uu_dict_is_empty(d));

  uu_dict_clear(d);
  assert(uu_dict_is_empty(d));

  uu_dict_deinit(d);
  assert(!d);

  printf("pass\n");
}

void test_dict_at() {
  printf("test(uu_dict_at): ");

  uu_dict(int, int*) d = uu_dict_init(d, cmp_fn_int);

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

  uu_dict_deinit(d);
  assert(!d);

  printf("pass\n");
}

void test_dict_insert() {
  printf("test(uu_dict_insert): ");

  uu_dict(int, int*) d = uu_dict_init(d, cmp_fn_int);

  int result = uu_dict_insert(d, 1, (void*)0x11);
  assert(result);
  assert(1 == uu_dict_len(d));

  result = uu_dict_insert(d, 2, (void*)0x22);
  assert(result);
  assert(2 == uu_dict_len(d));

  result = uu_dict_insert(d, 3, (void*)0x33);
  assert(result);
  assert(3 == uu_dict_len(d));

  result = uu_dict_insert(d, 1, (void*)0x11);
  assert(!result);
  assert(3 == uu_dict_len(d));

  assert((void*)0x11 == uu_dict_at(d, 1));
  assert((void*)0x22 == uu_dict_at(d, 2));
  assert((void*)0x33 == uu_dict_at(d, 3));

  uu_dict_deinit(d);
  assert(!d);

  printf("pass\n");
}

void test_dict_remove() {
  printf("test(uu_dict_remove): ");

  uu_dict(int, int*) d = uu_dict_init(d, cmp_fn_int);

  uu_dict_insert(d, 1, (void*)0x11);
  uu_dict_insert(d, 2, (void*)0x22);
  uu_dict_insert(d, 3, (void*)0x33);
  uu_dict_insert(d, 4, (void*)0x44);
  uu_dict_insert(d, 5, (void*)0x55);
  assert(5 == uu_dict_len(d));

  void* result = uu_dict_remove(d, 3);
  assert((void*)0x33 == result);
  assert(4 == uu_dict_len(d));

  result = uu_dict_remove(d, 3);
  assert(!result);
  assert(4 == uu_dict_len(d));

  assert((void*)0x11 == uu_dict_at(d, 1));
  assert((void*)0x22 == uu_dict_at(d, 2));
  assert((void*)0x44 == uu_dict_at(d, 4));
  assert((void*)0x55 == uu_dict_at(d, 5));
  assert(!uu_dict_at(d, 3));

  uu_dict_deinit(d);
  assert(!d);

  printf("pass\n");
}

void test_dict_each() {
  printf("test(uu_dict_each): ");

  uu_dict(int, int*) d = uu_dict_init(d, cmp_fn_int);

  uu_dict_insert(d, 1, (void*)1);
  uu_dict_insert(d, 2, (void*)2);
  uu_dict_insert(d, 3, (void*)3);
  uu_dict_insert(d, 4, (void*)4);
  uu_dict_insert(d, 5, (void*)5);

  int cnt = 0;
  uu_dict_each(d, key, int*, uptr) {
    assert(key == (int)(uintptr_t)uptr);
    cnt++;
  }

  assert(5 == cnt);

  uu_dict_deinit(d);
  assert(!d);

  printf("pass\n");
}

void test_dict_each_if() {
  printf("test(uu_dict_each_if): ");

  uu_dict(int, int*) d = uu_dict_init(d, cmp_fn_int);

  uu_dict_insert(d, 1, (void*)0x11);
  uu_dict_insert(d, 2, (void*)0x22);
  uu_dict_insert(d, 3, (void*)0x33);
  uu_dict_insert(d, 4, (void*)0x44);
  uu_dict_insert(d, 5, (void*)0x55);

  int cnt = 0;
  uu_dict_each_if(d, key, int*, uptr, key % 2 == 1) {
    (void)uptr;
    assert(key % 2 == 1);
    cnt++;
  }

  assert(3 == cnt);

  uu_dict_deinit(d);
  assert(!d);

  printf("pass\n");
}

void test_dict_find_if() {
  printf("test(uu_dict_find_if): ");

  uu_dict(int, int*) d = uu_dict_init(d, cmp_fn_int);

  uu_dict_insert(d, 1, (void*)0x11);
  uu_dict_insert(d, 2, (void*)0x22);
  uu_dict_insert(d, 3, (void*)0x33);
  uu_dict_insert(d, 4, (void*)0x44);
  uu_dict_insert(d, 5, (void*)0x55);

  int* found = uu_dict_find_if(d, key, int*, uptr, uptr == (int*)(uintptr_t)0x33);
  assert(found && (int)(uintptr_t)found == 0x33);

  found = uu_dict_find_if(d, key, int*, uptr, uptr == (int*)(uintptr_t)0x66);
  assert(!found);

  uu_dict_deinit(d);
  assert(!d);

  printf("pass\n");
}

void test_dict_any_if() {
  printf("test(uu_dict_any_if): ");

  uu_dict(int, int*) d = uu_dict_init(d, cmp_fn_int);

  uu_dict_insert(d, 1, (void*)0x11);
  uu_dict_insert(d, 2, (void*)0x22);
  uu_dict_insert(d, 3, (void*)0x33);
  uu_dict_insert(d, 4, (void*)0x44);
  uu_dict_insert(d, 5, (void*)0x55);

  int result = uu_dict_any_if(d, key, int*, uptr, uptr == (int*)(uintptr_t)0x33);
  assert(result);

  result = uu_dict_any_if(d, key, int*, uptr, uptr == (int*)(uintptr_t)0x66);
  assert(!result);

  uu_dict_deinit(d);
  assert(!d);

  printf("pass\n");
}

void test_dict_all_if() {
  printf("test(uu_dict_all_if): ");

  uu_dict(int, int*) d = uu_dict_init(d, cmp_fn_int);

  uu_dict_insert(d, 1, (void*)0x11);
  uu_dict_insert(d, 2, (void*)0x22);
  uu_dict_insert(d, 3, (void*)0x33);
  uu_dict_insert(d, 4, (void*)0x44);
  uu_dict_insert(d, 5, (void*)0x55);

  int result = uu_dict_all_if(d, key, int*, uptr, uptr >= (int*)(uintptr_t)0x11);
  assert(result);

  result = uu_dict_all_if(d, key, int*, uptr, uptr > (int*)(uintptr_t)0x11);
  assert(!result);

  uu_dict_deinit(d);
  assert(!d);

  printf("pass\n");
}

// 新增边界条件测试函数
void test_dict_access_nonexistent_key() {
  printf("test(dict_access_nonexistent_key): ");

  uu_dict(int, int*) d = uu_dict_init(d, cmp_fn_int);

  // 尝试访问不存在的键应该返回NULL
  assert(NULL == uu_dict_at(d, 1));
  assert(NULL == uu_dict_at(d, 999));

  // 插入一些键后再尝试访问不存在的键
  uu_dict_insert(d, 1, (void*)0x11);
  uu_dict_insert(d, 2, (void*)0x22);
  assert((void*)0x11 == uu_dict_at(d, 1));
  assert(NULL == uu_dict_at(d, 3));  // 应该返回NULL

  uu_dict_deinit(d);
  assert(!d);

  printf("pass\n");
}

void test_dict_large_data_operations() {
  printf("test(dict_large_data_operations): ");

  uu_dict(int, int*) d = uu_dict_init(d, cmp_fn_int);

  // 插入大量数据 - 使用较小的数量以避免性能问题
  const int num_items = 1000;
  for (int i = 0; i < num_items; i++) {
    int result = uu_dict_insert(d, i, (void*)(uintptr_t)(i + 100));
    assert(result);
  }

  assert(num_items == uu_dict_len(d));

  // 验证前100个键值对都正确存储
  for (int i = 0; i < 100; i++) {
    assert((void*)(uintptr_t)(i + 100) == uu_dict_at(d, i));
  }

  // 删除一些数据（每10个删除一个）
  for (int i = 0; i < num_items; i += 10) {
    void* result = uu_dict_remove(d, i);
    assert((void*)(uintptr_t)(i + 100) == result);
  }

  // 计算剩余项目数
  uint32_t expected_remaining = num_items - num_items / 10;
  assert(expected_remaining == uu_dict_len(d));

  // 验证删除后的数据 - 确保删除的项不再存在，未删除的项仍存在
  for (int i = 0; i < 100; i++) {
    if (i % 10 == 0) {
      // 这些应该已经被删除了
      assert(NULL == uu_dict_at(d, i));
    } else {
      // 这些应该仍然存在
      assert((void*)(uintptr_t)(i + 100) == uu_dict_at(d, i));
    }
  }

  // 尝试删除已经删除的元素
  for (int i = 0; i < 100; i += 10) {  // 这些应该已经被删除了
    void* result = uu_dict_remove(d, i);
    assert(NULL == result);
  }

  uu_dict_deinit(d);
  assert(!d);

  printf("pass\n");
}

void test_dict_duplicate_keys() {
  printf("test(dict_duplicate_keys): ");

  uu_dict(int, int*) d = uu_dict_init(d, cmp_fn_int);

  // 插入相同的键多次
  int result = uu_dict_insert(d, 1, (void*)0x11);
  assert(result);

  result = uu_dict_insert(d, 1, (void*)0x22);  // 重复键，应该失败
  assert(!result);

  // 值不应该改变
  assert((void*)0x11 == uu_dict_at(d, 1));

  // 测试插入另一个键
  result = uu_dict_insert(d, 2, (void*)0x22);
  assert(result);
  assert((void*)0x22 == uu_dict_at(d, 2));

  // 再次尝试插入相同键2但不同值，应该失败
  result = uu_dict_insert(d, 2, (void*)0x33);
  assert(!result);
  // 值仍应为原始值
  assert((void*)0x22 == uu_dict_at(d, 2));

  // 尝试插入多个不同的键
  for (int i = 3; i < 10; i++) {
    result = uu_dict_insert(d, i, (void*)(uintptr_t)(i + 100));
    assert(result);  // 所有插入都应该成功，因为键都不同
    assert((void*)(uintptr_t)(i + 100) == uu_dict_at(d, i));
  }

  // 尝试重复插入其中一个键，应该失败
  result = uu_dict_insert(d, 5, (void*)0xDEADBEEF);
  assert(!result);  // 应该失败，因为键5已经存在
  // 值仍应为原始值
  assert((void*)(uintptr_t)(5 + 100) == uu_dict_at(d, 5));

  uu_dict_deinit(d);
  assert(!d);

  printf("pass\n");
}

void test_dict_extreme_values() {
  printf("test(dict_extreme_values): ");

  uu_dict(int, int*) d = uu_dict_init(d, cmp_fn_int);

  // 使用极值作为键进行测试
  int min_int = INT_MIN;
  int max_int = INT_MAX;

  int result = uu_dict_insert(d, min_int, (void*)0xAA);
  assert(result);

  result = uu_dict_insert(d, max_int, (void*)0xBB);
  assert(result);

  assert((void*)0xAA == uu_dict_at(d, min_int));
  assert((void*)0xBB == uu_dict_at(d, max_int));

  // 删除极值键
  void* removed_val = uu_dict_remove(d, min_int);
  assert((void*)0xAA == removed_val);

  removed_val = uu_dict_remove(d, max_int);
  assert((void*)0xBB == removed_val);

  // 确认它们已被删除
  assert(NULL == uu_dict_at(d, min_int));
  assert(NULL == uu_dict_at(d, max_int));

  // 重新插入
  result = uu_dict_insert(d, min_int, (void*)0xCC);
  assert(result);
  result = uu_dict_insert(d, max_int, (void*)0xDD);
  assert(result);

  assert((void*)0xCC == uu_dict_at(d, min_int));
  assert((void*)0xDD == uu_dict_at(d, max_int));

  uu_dict_deinit(d);
  assert(!d);

  printf("pass\n");
}

void test_dict_zero_key() {
  printf("test(dict_zero_key): ");

  uu_dict(int, int*) d = uu_dict_init(d, cmp_fn_int);

  // 测试零作为键
  int result = uu_dict_insert(d, 0, (void*)0x00);
  assert(result);

  assert((void*)0x00 == uu_dict_at(d, 0));

  // 尝试再次插入零键
  result = uu_dict_insert(d, 0, (void*)0xFF);
  assert(!result);  // 应该失败

  // 值仍应为原值
  assert((void*)0x00 == uu_dict_at(d, 0));

  // 删除零键
  void* removed_val = uu_dict_remove(d, 0);
  assert((void*)0x00 == removed_val);

  // 确认已删除
  assert(NULL == uu_dict_at(d, 0));

  uu_dict_deinit(d);
  assert(!d);

  printf("pass\n");
}

void test_dict_negative_keys() {
  printf("test(dict_negative_keys): ");

  uu_dict(int, int*) d = uu_dict_init(d, cmp_fn_int);

  // 测试负数键
  int result = uu_dict_insert(d, -1, (void*)0x11);
  assert(result);

  result = uu_dict_insert(d, -100, (void*)0x22);
  assert(result);

  result = uu_dict_insert(d, -999, (void*)0x33);
  assert(result);

  assert((void*)0x11 == uu_dict_at(d, -1));
  assert((void*)0x22 == uu_dict_at(d, -100));
  assert((void*)0x33 == uu_dict_at(d, -999));

  // 混合正负键
  result = uu_dict_insert(d, 1, (void*)0x44);
  assert(result);

  result = uu_dict_insert(d, 100, (void*)0x55);
  assert(result);

  assert((void*)0x44 == uu_dict_at(d, 1));
  assert((void*)0x55 == uu_dict_at(d, 100));

  // 删除负数键
  void* removed_val = uu_dict_remove(d, -100);
  assert((void*)0x22 == removed_val);

  assert(NULL == uu_dict_at(d, -100));

  uu_dict_deinit(d);
  assert(!d);

  printf("pass\n");
}

void test_dict_edge_case_operations() {
  printf("test(dict_edge_case_operations): ");

  uu_dict(int, int*) d = uu_dict_init(d, cmp_fn_int);

  // 测试在空字典中删除
  void* result = uu_dict_remove(d, 1);
  assert(NULL == result);

  // 插入、删除、再插入同一键
  int insert_result = uu_dict_insert(d, 42, (void*)0x42);
  assert(insert_result);
  assert((void*)0x42 == uu_dict_at(d, 42));

  result = uu_dict_remove(d, 42);
  assert((void*)0x42 == result);
  assert(NULL == uu_dict_at(d, 42));

  insert_result = uu_dict_insert(d, 42, (void*)0x84);
  assert(insert_result);
  assert((void*)0x84 == uu_dict_at(d, 42));

  // 清空后重新填充
  uu_dict_clear(d);
  assert(0 == uu_dict_len(d));
  assert(uu_dict_is_empty(d));

  // 再次测试基本操作
  insert_result = uu_dict_insert(d, 1, (void*)0x11);
  assert(insert_result);
  assert((void*)0x11 == uu_dict_at(d, 1));

  uu_dict_deinit(d);
  assert(!d);

  printf("pass\n");
}

void test_dict_memory_operations() {
  printf("test(dict_memory_operations): ");

  // 多次初始化和销毁，测试内存管理
  for (int i = 0; i < 3; i++) {  // 减少循环次数进一步
    uu_dict(int, int*) d = uu_dict_init(d, cmp_fn_int);

    // 插入少量数据
    for (int j = 0; j < 10; j++) {
      int insert_result = uu_dict_insert(d, j, (void*)(uintptr_t)(j + 1000));
      assert(insert_result);
    }

    // 验证数据
    for (int j = 0; j < 10; j++) {
      assert((void*)(uintptr_t)(j + 1000) == uu_dict_at(d, j));
    }

    // 清理
    uu_dict_deinit(d);
    assert(!d);
  }

  printf("pass\n");
}

void test_dict_error_usage() {
  printf("test(dict_error_usage): ");

  // 注意：根据uu.h中的实现，宏会检查指针是否为NULL，所以不会出现段错误
  // 但我们仍可以测试一些逻辑边界情况

  uu_dict(int, int*) d = uu_dict_init(d, cmp_fn_int);

  // 在空字典中查找不存在的键应该返回NULL而不是崩溃
  assert(NULL == uu_dict_at(d, 123));
  assert(NULL == uu_dict_at(d, -1));
  assert(NULL == uu_dict_at(d, INT_MAX));
  assert(NULL == uu_dict_at(d, INT_MIN));

  // 删除不存在的键应该返回NULL而不是崩溃
  assert(NULL == uu_dict_remove(d, 999));
  assert(NULL == uu_dict_remove(d, -999));

  // 插入后删除，然后再次删除相同键
  uu_dict_insert(d, 42, (void*)0x42);
  assert((void*)0x42 == uu_dict_remove(d, 42));  // 第一次删除成功
  assert(NULL == uu_dict_remove(d, 42));         // 第二次删除返回NULL

  // 尝试获取删除后的值
  assert(NULL == uu_dict_at(d, 42));

  // 测试大量连续插入和删除
  for (int i = 0; i < 100; i++) {
    uu_dict_insert(d, i, (void*)(uintptr_t)i);
  }

  for (int i = 0; i < 100; i++) {
    assert((void*)(uintptr_t)i == uu_dict_at(d, i));
  }

  for (int i = 0; i < 100; i++) {
    assert((void*)(uintptr_t)i == uu_dict_remove(d, i));
  }

  // 确保全部被删除
  for (int i = 0; i < 100; i++) {
    assert(NULL == uu_dict_at(d, i));
  }

  assert(0 == uu_dict_len(d));

  uu_dict_deinit(d);
  assert(!d);

  printf("pass\n");
}

int main() {
  test_dict_init();
  test_dict_clear();
  test_dict_deinit();
  test_dict_len();
  test_dict_is_empty();
  test_dict_at();
  test_dict_insert();
  test_dict_remove();
  test_dict_each();
  test_dict_each_if();
  test_dict_find_if();
  test_dict_any_if();
  test_dict_all_if();

  // 新增的边界条件测试
  printf("\n=== Additional boundary condition tests ===\n");
  test_dict_access_nonexistent_key();
  test_dict_large_data_operations();
  test_dict_duplicate_keys();
  test_dict_extreme_values();
  test_dict_zero_key();
  test_dict_negative_keys();
  test_dict_edge_case_operations();
  test_dict_memory_operations();
  test_dict_error_usage();

  return 0;
}
