#define UU_DEBUG

#include "uu.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

uu_cmp_fn_def(int, x, y, x - y);

void test_vec_init() {
  printf("test(uu_vec_init): ");

  uu_vec(int) v = NULL;
  v             = uu_vec_init(v);

  assert(v);
  assert(uu_vec_is_empty(v));
  assert(0 == uu_vec_len(v));

  uu_vec_deinit(v);
  assert(!v);

  printf("pass\n");
}

void test_vec_clear() {
  printf("test(uu_vec_clear): ");

  uu_vec(int) v = uu_vec_init(v);

  uu_vec_insert_tail(v, 1);
  uu_vec_insert_tail(v, 2);
  assert(2 == uu_vec_len(v));

  uu_vec_clear(v);
  assert(0 == uu_vec_len(v));
  assert(uu_vec_is_empty(v));

  uu_vec_deinit(v);
  assert(!v);

  printf("pass\n");
}

void test_vec_deinit() {
  printf("test(uu_vec_deinit): ");

  uu_vec(int) v = uu_vec_init(v);

  uu_vec_insert_tail(v, 1);
  uu_vec_insert_tail(v, 2);
  assert(2 == uu_vec_len(v));

  uu_vec_deinit(v);
  assert(!v);

  printf("pass\n");
}

void test_vec_len() {
  printf("test(uu_vec_len): ");

  uu_vec(int) v = uu_vec_init(v);

  assert(0 == uu_vec_len(v));

  uu_vec_insert_tail(v, 1);
  assert(1 == uu_vec_len(v));

  uu_vec_insert_tail(v, 2);
  assert(2 == uu_vec_len(v));

  uu_vec_insert_tail(v, 3);
  assert(3 == uu_vec_len(v));

  uu_vec_deinit(v);
  assert(!v);

  printf("pass\n");
}

void test_vec_is_empty() {
  printf("test(uu_vec_is_empty): ");

  uu_vec(int) v = uu_vec_init(v);

  assert(uu_vec_is_empty(v));

  uu_vec_insert_tail(v, 1);
  assert(!uu_vec_is_empty(v));

  uu_vec_insert_tail(v, 2);
  assert(!uu_vec_is_empty(v));

  uu_vec_clear(v);
  assert(uu_vec_is_empty(v));

  uu_vec_deinit(v);
  assert(!v);

  printf("pass\n");
}

void test_vec_at() {
  printf("test(uu_vec_at): ");

  uu_vec(int) v = uu_vec_init(v);

  uu_vec_insert_tail(v, 10);
  uu_vec_insert_tail(v, 20);
  uu_vec_insert_tail(v, 30);
  uu_vec_insert_tail(v, 40);
  uu_vec_insert_tail(v, 50);

  assert(10 == uu_vec_at(v, 0));
  assert(20 == uu_vec_at(v, 1));
  assert(30 == uu_vec_at(v, 2));
  assert(40 == uu_vec_at(v, 3));
  assert(50 == uu_vec_at(v, 4));

  assert(10 == uu_vec_at_head(v));
  assert(50 == uu_vec_at_tail(v));

  uu_vec_deinit(v);
  assert(!v);

  printf("pass\n");
}

void test_vec_swap() {
  printf("test(uu_vec_swap): ");

  uu_vec(int) v = uu_vec_init(v);

  uu_vec_insert_tail(v, 10);
  uu_vec_insert_tail(v, 20);
  uu_vec_insert_tail(v, 30);

  assert(10 == uu_vec_at_head(v));
  uu_vec_swap_head(v, 100);
  assert(100 == uu_vec_at_head(v));

  assert(30 == uu_vec_at_tail(v));
  uu_vec_swap_tail(v, 300);
  assert(300 == uu_vec_at_tail(v));

  assert(20 == uu_vec_at(v, 1));
  uu_vec_swap(v, 1, 200);
  assert(200 == uu_vec_at(v, 1));

  uu_vec_deinit(v);
  assert(!v);

  printf("pass\n");
}

void test_vec_insert() {
  printf("test(uu_vec_insert): ");

  uu_vec(int) v = uu_vec_init(v);

  uu_vec_insert(v, 0, 10);  // {10}
  assert(1 == uu_vec_len(v));
  assert(10 == uu_vec_at(v, 0));

  uu_vec_insert_head(v, 5);  // {5, 10}
  assert(2 == uu_vec_len(v));
  assert(5 == uu_vec_at_head(v));

  uu_vec_insert_tail(v, 15);  // {5, 10, 15}
  assert(3 == uu_vec_len(v));
  assert(15 == uu_vec_at_tail(v));

  uu_vec_insert(v, 1, 7);  // {5, 7, 10, 15}
  assert(4 == uu_vec_len(v));
  assert(7 == uu_vec_at(v, 1));

  uint32_t len = uu_vec_len(v);
  uu_vec_insert(v, len, 20);  // {5, 7, 10, 15, 20}
  assert(5 == uu_vec_len(v));
  assert(20 == uu_vec_at_tail(v));

  uu_vec_deinit(v);
  assert(!v);

  printf("pass\n");
}

void test_vec_remove() {
  printf("test(uu_vec_remove): ");

  uu_vec(int) v = uu_vec_init(v);

  uu_vec_insert_tail(v, 10);
  uu_vec_insert_tail(v, 20);
  uu_vec_insert_tail(v, 30);
  uu_vec_insert_tail(v, 40);
  uu_vec_insert_tail(v, 50);
  assert(5 == uu_vec_len(v));

  int result = uu_vec_remove(v, 2);  // remove element at index 2 (30)
  assert(30 == result);
  assert(4 == uu_vec_len(v));

  result = uu_vec_remove_head(v);  // remove head (10)
  assert(10 == result);
  assert(3 == uu_vec_len(v));

  result = uu_vec_remove_tail(v);  // remove tail (50)
  assert(50 == result);
  assert(2 == uu_vec_len(v));

  assert(20 == uu_vec_at(v, 0));
  assert(40 == uu_vec_at(v, 1));

  uu_vec_deinit(v);
  assert(!v);

  printf("pass\n");
}

void test_vec_each() {
  printf("test(uu_vec_each): ");

  uu_vec(int) v = uu_vec_init(v);

  uu_vec_insert_tail(v, 1);
  uu_vec_insert_tail(v, 2);
  uu_vec_insert_tail(v, 3);
  uu_vec_insert_tail(v, 4);
  uu_vec_insert_tail(v, 5);

  int cnt = 0;
  int sum = 0;
  uu_vec_each(v, it) {
    cnt++;
    sum += it;
  }

  assert(5 == cnt);
  assert(15 == sum);  // 1+2+3+4+5 = 15

  uu_vec_deinit(v);
  assert(!v);

  printf("pass\n");
}

void test_vec_each_if() {
  printf("test(uu_vec_each_if): ");

  uu_vec(int) v = uu_vec_init(v);

  uu_vec_insert_tail(v, 1);
  uu_vec_insert_tail(v, 2);
  uu_vec_insert_tail(v, 3);
  uu_vec_insert_tail(v, 4);
  uu_vec_insert_tail(v, 5);

  int cnt = 0;
  uu_vec_each_if(v, it, it % 2 == 1) {
    assert(it % 2 == 1);
    cnt++;
  }

  assert(3 == cnt);  // 1, 3, 5 are odd

  uu_vec_deinit(v);
  assert(!v);

  printf("pass\n");
}

void test_vec_reach() {
  printf("test(uu_vec_reach): ");

  uu_vec(int) v = uu_vec_init(v);

  uu_vec_insert_tail(v, 1);
  uu_vec_insert_tail(v, 2);
  uu_vec_insert_tail(v, 3);
  uu_vec_insert_tail(v, 4);
  uu_vec_insert_tail(v, 5);

  int cnt = 4;
  uu_vec_reach(v, it) {
    assert(cnt == it - 1);  // it starts from 5, decreases to 1
    cnt--;
  }

  assert(-1 == cnt);

  uu_vec_deinit(v);
  assert(!v);

  printf("pass\n");
}

void test_vec_reach_if() {
  printf("test(uu_vec_reach_if): ");

  uu_vec(int) v = uu_vec_init(v);

  uu_vec_insert_tail(v, 1);
  uu_vec_insert_tail(v, 2);
  uu_vec_insert_tail(v, 3);
  uu_vec_insert_tail(v, 4);
  uu_vec_insert_tail(v, 5);

  int cnt = 0;
  uu_vec_reach_if(v, it, it % 2 == 1) {
    assert(it % 2 == 1);
    cnt++;
  }

  assert(3 == cnt);  // 5, 3, 1 are odd

  uu_vec_deinit(v);
  assert(!v);

  printf("pass\n");
}

void test_vec_count_if() {
  printf("test(uu_vec_count_if): ");

  uu_vec(int) v = uu_vec_init(v);

  uu_vec_insert_tail(v, 1);
  uu_vec_insert_tail(v, 2);
  uu_vec_insert_tail(v, 3);
  uu_vec_insert_tail(v, 4);
  uu_vec_insert_tail(v, 5);

  uint32_t count = uu_vec_count_if(v, it, it % 2 == 0);
  assert(2 == count);  // 2, 4 are even

  count = uu_vec_count_if(v, it, it % 2 != 0);
  assert(3 == count);  // 1, 3, 5 are odd

  count = uu_vec_count_if(v, it, it > 3);
  assert(2 == count);  // 4, 5 are greater than 3

  uu_vec_deinit(v);
  assert(!v);

  printf("pass\n");
}

void test_vec_find_if() {
  printf("test(uu_vec_find_if): ");

  uu_vec(int) v = uu_vec_init(v);

  uu_vec_insert_tail(v, 10);
  uu_vec_insert_tail(v, 20);
  uu_vec_insert_tail(v, 30);
  uu_vec_insert_tail(v, 20);
  uu_vec_insert_tail(v, 50);

  int64_t idx = uu_vec_find_if(v, it, it == 20);
  assert(1 == idx);  // first occurrence of 20 is at index 1

  idx = uu_vec_find_if(v, it, it == 60);
  assert(-1 == idx);  // 60 is not in the vector

  idx = uu_vec_find_if(v, it, it == 50);
  assert(4 == idx);  // 50 is at index 4

  uu_vec_deinit(v);
  assert(!v);

  printf("pass\n");
}

void test_vec_rfind_if() {
  printf("test(uu_vec_rfind_if): ");

  uu_vec(int) v = uu_vec_init(v);

  uu_vec_insert_tail(v, 10);
  uu_vec_insert_tail(v, 20);
  uu_vec_insert_tail(v, 30);
  uu_vec_insert_tail(v, 20);
  uu_vec_insert_tail(v, 50);

  int64_t idx = uu_vec_rfind_if(v, it, it == 20);
  assert(3 == idx);  // last occurrence of 20 is at index 3

  idx = uu_vec_rfind_if(v, it, it == 60);
  assert(-1 == idx);  // 60 is not in the vector

  idx = uu_vec_rfind_if(v, it, it == 10);
  assert(0 == idx);  // 10 is at index 0

  uu_vec_deinit(v);
  assert(!v);

  printf("pass\n");
}

void test_vec_map_by() {
  printf("test(uu_vec_map_by): ");

  uu_vec(int) v = uu_vec_init(v);

  uu_vec_insert_tail(v, 1);
  uu_vec_insert_tail(v, 2);
  uu_vec_insert_tail(v, 3);
  uu_vec_insert_tail(v, 4);
  uu_vec_insert_tail(v, 5);

  uu_vec_map_by(v, it, { it = it * 2; });

  assert(2 == uu_vec_at(v, 0));
  assert(4 == uu_vec_at(v, 1));
  assert(6 == uu_vec_at(v, 2));
  assert(8 == uu_vec_at(v, 3));
  assert(10 == uu_vec_at(v, 4));

  uu_vec_deinit(v);
  assert(!v);

  printf("pass\n");
}

void test_vec_any_if() {
  printf("test(uu_vec_any_if): ");

  uu_vec(int) v = uu_vec_init(v);

  uu_vec_insert_tail(v, 1);
  uu_vec_insert_tail(v, 2);
  uu_vec_insert_tail(v, 3);
  uu_vec_insert_tail(v, 4);
  uu_vec_insert_tail(v, 5);

  int result = uu_vec_any_if(v, it, it >= 5);
  assert(result);

  result = uu_vec_any_if(v, it, it > 5);
  assert(!result);

  result = uu_vec_any_if(v, it, it == 3);
  assert(result);

  result = uu_vec_any_if(v, it, it < 0);
  assert(!result);

  uu_vec_deinit(v);
  assert(!v);

  printf("pass\n");
}

void test_vec_all_if() {
  printf("test(uu_vec_all_if): ");

  uu_vec(int) v = uu_vec_init(v);

  uu_vec_insert_tail(v, 1);
  uu_vec_insert_tail(v, 2);
  uu_vec_insert_tail(v, 3);
  uu_vec_insert_tail(v, 4);
  uu_vec_insert_tail(v, 5);

  int result = uu_vec_all_if(v, it, it > 0);
  assert(result);

  result = uu_vec_all_if(v, it, it > 1);
  assert(!result);

  result = uu_vec_all_if(v, it, it <= 5);
  assert(result);

  result = uu_vec_all_if(v, it, it < 5);
  assert(!result);

  uu_vec_deinit(v);
  assert(!v);

  printf("pass\n");
}

void test_vec_sort() {
  printf("test(uu_vec_sort): ");

  uu_vec(int) v = uu_vec_init(v);

  // Insert elements in reverse order
  uu_vec_insert_tail(v, 5);
  uu_vec_insert_tail(v, 4);
  uu_vec_insert_tail(v, 3);
  uu_vec_insert_tail(v, 2);
  uu_vec_insert_tail(v, 1);

  assert(5 == uu_vec_len(v));
  assert(5 == uu_vec_at(v, 0));  // first element is 5
  assert(1 == uu_vec_at(v, 4));  // last element is 1

  uu_vec_sort(v, cmp_fn_int);

  assert(5 == uu_vec_len(v));
  assert(1 == uu_vec_at(v, 0));  // first element is now 1
  assert(2 == uu_vec_at(v, 1));  // second element is now 2
  assert(3 == uu_vec_at(v, 2));  // third element is now 3
  assert(4 == uu_vec_at(v, 3));  // fourth element is now 4
  assert(5 == uu_vec_at(v, 4));  // last element is now 5

  // Test with duplicates
  uu_vec_clear(v);
  uu_vec_insert_tail(v, 3);
  uu_vec_insert_tail(v, 1);
  uu_vec_insert_tail(v, 4);
  uu_vec_insert_tail(v, 1);
  uu_vec_insert_tail(v, 5);
  uu_vec_insert_tail(v, 9);
  uu_vec_insert_tail(v, 2);
  uu_vec_insert_tail(v, 6);
  uu_vec_insert_tail(v, 5);
  uu_vec_insert_tail(v, 3);

  uu_vec_sort(v, cmp_fn_int);

  // Check if sorted properly: 1, 1, 2, 3, 3, 4, 5, 5, 6, 9
  assert(1 == uu_vec_at(v, 0));
  assert(1 == uu_vec_at(v, 1));
  assert(2 == uu_vec_at(v, 2));
  assert(3 == uu_vec_at(v, 3));
  assert(3 == uu_vec_at(v, 4));
  assert(4 == uu_vec_at(v, 5));
  assert(5 == uu_vec_at(v, 6));
  assert(5 == uu_vec_at(v, 7));
  assert(6 == uu_vec_at(v, 8));
  assert(9 == uu_vec_at(v, 9));

  uu_vec_deinit(v);
  assert(!v);

  printf("pass\n");
}

void test_vec_edge_cases() {
  printf("test(uu_vec_edge_cases): ");

  // Test empty vector operations
  uu_vec(int) v = uu_vec_init(v);

  // Test accessing head/tail of empty vector - this should trigger assertion failure
  // We won't run these tests as they would crash the program
  // assert(uu_vec_is_empty(v));
  // int head = uu_vec_at_head(v);  // Would fail assertion
  // int tail = uu_vec_at_tail(v);  // Would fail assertion

  // Test insert at invalid index (should trigger assertion)
  // uu_vec_insert(v, 1, 10);  // Would fail assertion since len=0 and index=1

  // Test remove from empty vector (should trigger assertion)
  // uu_vec_remove_head(v);  // Would fail assertion
  // uu_vec_remove_tail(v);  // Would fail assertion

  // Test operations on single element vector
  uu_vec_insert_tail(v, 42);
  assert(1 == uu_vec_len(v));
  assert(42 == uu_vec_at_head(v));
  assert(42 == uu_vec_at_tail(v));
  assert(42 == uu_vec_at(v, 0));

  // Head and tail should be the same for single element
  assert(uu_vec_at_head(v) == uu_vec_at_tail(v));

  // Remove the only element
  int removed = uu_vec_remove_head(v);
  assert(42 == removed);
  assert(0 == uu_vec_len(v));
  assert(uu_vec_is_empty(v));

  // Re-add and test tail removal
  uu_vec_insert_tail(v, 99);
  removed = uu_vec_remove_tail(v);
  assert(99 == removed);
  assert(0 == uu_vec_len(v));
  assert(uu_vec_is_empty(v));

  // Test large vector operations
  for (int i = 0; i < 1000; i++) {
    uu_vec_insert_tail(v, i);
  }
  assert(1000 == uu_vec_len(v));

  // Check a few values
  assert(0 == uu_vec_at(v, 0));
  assert(500 == uu_vec_at(v, 500));
  assert(999 == uu_vec_at(v, 999));

  // Clear and check
  uu_vec_clear(v);
  assert(0 == uu_vec_len(v));
  assert(uu_vec_is_empty(v));

  uu_vec_deinit(v);
  assert(!v);

  printf("pass\n");
}

void test_vec_boundary_conditions() {
  printf("test(uu_vec_boundary_conditions): ");

  uu_vec(int) v = uu_vec_init(v);

  // ===========================================================================
  // 1. 插入操作边界条件
  // ===========================================================================

  // 1.1 在索引 0 处插入（空向量）
  uu_vec_insert(v, 0, 100);
  assert(1 == uu_vec_len(v));
  assert(100 == uu_vec_at(v, 0));

  // 1.2 在索引 len 处插入（尾部）
  uu_vec_insert(v, 1, 200);
  assert(2 == uu_vec_len(v));
  assert(200 == uu_vec_at(v, 1));

  // 1.3 在索引 len-1 处插入
  uu_vec_insert(v, 1, 150);  // {100, 150, 200}
  assert(3 == uu_vec_len(v));
  assert(150 == uu_vec_at(v, 1));

  // 1.4 连续插入导致容量扩展（初始容量为 16）
  uu_vec_clear(v);
  for (int i = 0; i < 20; i++) {
    uu_vec_insert_tail(v, i * 10);
  }
  assert(20 == uu_vec_len(v));
  // 验证扩容后数据完整性
  for (int i = 0; i < 20; i++) {
    assert(i * 10 == uu_vec_at(v, i));
  }

  // ===========================================================================
  // 2. 删除操作边界条件
  // ===========================================================================

  // 2.1 删除第一个元素
  uu_vec_remove_head(v);
  assert(19 == uu_vec_len(v));
  assert(10 == uu_vec_at(v, 0));  // 原索引 1 的元素现在在索引 0

  // 2.2 删除最后一个元素
  uu_vec_remove_tail(v);
  assert(18 == uu_vec_len(v));
  assert(180 == uu_vec_at(v, uu_vec_len(v) - 1));

  // 2.3 删除中间元素
  uu_vec_remove(v, 5);  // 删除索引 5 的元素
  assert(17 == uu_vec_len(v));

  // 2.4 连续删除所有元素
  uu_vec_clear(v);
  for (int i = 0; i < 5; i++) {
    uu_vec_insert_tail(v, i);
  }
  while (!uu_vec_is_empty(v)) {
    uu_vec_remove_head(v);
  }
  assert(0 == uu_vec_len(v));

  // ===========================================================================
  // 3. Swap 操作边界条件
  // ===========================================================================

  uu_vec_insert_tail(v, 1);
  uu_vec_insert_tail(v, 2);
  uu_vec_insert_tail(v, 3);

  // 3.1 swap 同一位置
  uu_vec_swap(v, 1, 2);  // 值不变
  assert(2 == uu_vec_at(v, 1));

  // 3.2 swap 首尾元素
  uu_vec_swap_head(v, 100);
  uu_vec_swap_tail(v, 300);
  assert(100 == uu_vec_at_head(v));
  assert(300 == uu_vec_at_tail(v));

  // 3.3 swap 后再次访问
  int head_val = uu_vec_at_head(v);
  int tail_val = uu_vec_at_tail(v);
  uu_vec_swap_head(v, tail_val);
  uu_vec_swap_tail(v, head_val);
  assert(300 == uu_vec_at_head(v));
  assert(100 == uu_vec_at_tail(v));

  uu_vec_clear(v);

  // ===========================================================================
  // 4. 迭代器边界条件
  // ===========================================================================

  // 4.1 空向量遍历
  int count = 0;
  uu_vec_each(v, it) {
    count++;
  }
  assert(0 == count);

  // 4.2 单元素向量遍历
  uu_vec_insert_tail(v, 42);
  count = 0;
  uu_vec_each(v, it) {
    count++;
    assert(42 == it);
  }
  assert(1 == count);

  // 4.3 反向遍历空向量
  uu_vec_clear(v);
  count = 0;
  uu_vec_reach(v, it) {
    count++;
  }
  assert(0 == count);

  // 4.4 反向遍历单元素
  uu_vec_insert_tail(v, 99);
  count = 0;
  uu_vec_reach(v, it) {
    count++;
    assert(99 == it);
  }
  assert(1 == count);

  uu_vec_clear(v);

  // ===========================================================================
  // 5. 查找操作边界条件
  // ===========================================================================

  // 5.1 空向量查找
  int64_t idx = uu_vec_find_if(v, it, it == 1);
  assert(-1 == idx);

  idx = uu_vec_rfind_if(v, it, it == 1);
  assert(-1 == idx);

  // 5.2 查找第一个匹配项（index 0）
  uu_vec_insert_tail(v, 10);
  uu_vec_insert_tail(v, 20);
  uu_vec_insert_tail(v, 30);
  idx = uu_vec_find_if(v, it, it == 10);
  assert(0 == idx);

  // 5.3 查找最后一个匹配项（index len-1）
  idx = uu_vec_rfind_if(v, it, it == 30);
  assert(2 == idx);

  // 5.4 所有元素都匹配
  uu_vec_clear(v);
  uu_vec_insert_tail(v, 5);
  uu_vec_insert_tail(v, 5);
  uu_vec_insert_tail(v, 5);
  idx = uu_vec_find_if(v, it, it == 5);
  assert(0 == idx);  // 返回第一个匹配项
  idx = uu_vec_rfind_if(v, it, it == 5);
  assert(2 == idx);  // 返回最后一个匹配项

  // 5.5 查找条件都不匹配
  idx = uu_vec_find_if(v, it, it == 100);
  assert(-1 == idx);

  uu_vec_clear(v);

  // ===========================================================================
  // 6. 计数和条件判断边界条件
  // ===========================================================================

  // 6.1 空向量计数
  uint32_t cnt = uu_vec_count_if(v, it, it == 5);
  assert(0 == cnt);

  // 6.2 所有元素都满足条件
  uu_vec_insert_tail(v, 5);
  uu_vec_insert_tail(v, 5);
  uu_vec_insert_tail(v, 5);
  cnt = uu_vec_count_if(v, it, it == 5);
  assert(3 == cnt);

  // 6.3 没有元素满足条件
  cnt = uu_vec_count_if(v, it, it == 100);
  assert(0 == cnt);

  // 6.4 any_if 空向量
  assert(!uu_vec_any_if(v, it, it > 100));

  // 6.5 any_if 第一个元素就满足
  assert(uu_vec_any_if(v, it, it == 5));

  // 6.6 all_if 空向量（数学上应为 true，但实现可能不同）
  // 注意：空向量的 all_if 行为取决于实现

  // 6.7 all_if 所有元素满足
  assert(uu_vec_all_if(v, it, it == 5));

  // 6.8 all_if 部分元素不满足
  uu_vec_insert_tail(v, 10);
  assert(!uu_vec_all_if(v, it, it == 5));

  uu_vec_clear(v);

  // ===========================================================================
  // 7. 排序边界条件
  // ===========================================================================

  // 7.1 空向量排序
  uu_vec_sort(v, cmp_fn_int);  // 不应崩溃

  // 7.2 单元素向量排序
  uu_vec_insert_tail(v, 42);
  uu_vec_sort(v, cmp_fn_int);
  assert(1 == uu_vec_len(v));
  assert(42 == uu_vec_at(v, 0));

  // 7.3 已排序向量排序
  uu_vec_clear(v);
  uu_vec_insert_tail(v, 1);
  uu_vec_insert_tail(v, 2);
  uu_vec_insert_tail(v, 3);
  uu_vec_insert_tail(v, 4);
  uu_vec_insert_tail(v, 5);
  uu_vec_sort(v, cmp_fn_int);
  assert(1 == uu_vec_at(v, 0));
  assert(5 == uu_vec_at(v, 4));

  // 7.4 完全逆序向量排序
  uu_vec_clear(v);
  uu_vec_insert_tail(v, 5);
  uu_vec_insert_tail(v, 4);
  uu_vec_insert_tail(v, 3);
  uu_vec_insert_tail(v, 2);
  uu_vec_insert_tail(v, 1);
  uu_vec_sort(v, cmp_fn_int);
  assert(1 == uu_vec_at(v, 0));
  assert(5 == uu_vec_at(v, 4));

  // 7.5 所有元素相同的向量排序
  uu_vec_clear(v);
  uu_vec_insert_tail(v, 7);
  uu_vec_insert_tail(v, 7);
  uu_vec_insert_tail(v, 7);
  uu_vec_insert_tail(v, 7);
  uu_vec_sort(v, cmp_fn_int);
  assert(4 == uu_vec_len(v));
  for (int i = 0; i < 4; i++) {
    assert(7 == uu_vec_at(v, i));
  }

  uu_vec_clear(v);

  // ===========================================================================
  // 8. 容量扩展测试（详细）
  // ===========================================================================

  // 初始容量为 16，测试跨越容量边界
  for (int i = 0; i < 16; i++) {
    uu_vec_insert_tail(v, i);
  }
  assert(16 == uu_vec_len(v));

  // 插入第 17 个元素，触发扩容
  uu_vec_insert_tail(v, 100);
  assert(17 == uu_vec_len(v));
  assert(100 == uu_vec_at(v, 16));

  // 验证扩容前的所有元素仍然存在
  for (int i = 0; i < 16; i++) {
    assert(i == uu_vec_at(v, i));
  }

  // 继续插入直到再次扩容（容量变为 32）
  for (int i = 17; i < 32; i++) {
    uu_vec_insert_tail(v, i);
  }
  assert(32 == uu_vec_len(v));

  // 验证所有元素
  for (int i = 0; i < 16; i++) {
    assert(i == uu_vec_at(v, i));
  }
  assert(100 == uu_vec_at(v, 16));
  for (int i = 17; i < 32; i++) {
    assert(i == uu_vec_at(v, i));
  }

  // 插入第 33 个元素，再次触发扩容（容量变为 64）
  uu_vec_insert_tail(v, 999);
  assert(33 == uu_vec_len(v));
  assert(999 == uu_vec_at(v, 32));

  uu_vec_deinit(v);
  assert(!v);

  printf("pass\n");
}

int main() {
  test_vec_init();
  test_vec_clear();
  test_vec_deinit();
  test_vec_len();
  test_vec_is_empty();
  test_vec_at();
  test_vec_swap();
  test_vec_insert();
  test_vec_remove();
  test_vec_each();
  test_vec_each_if();
  test_vec_reach();
  test_vec_reach_if();
  test_vec_count_if();
  test_vec_find_if();
  test_vec_rfind_if();
  test_vec_map_by();
  test_vec_any_if();
  test_vec_all_if();
  test_vec_sort();
  test_vec_edge_cases();
  test_vec_boundary_conditions();  // 新增：全面的边界条件测试

  return 0;
}
