/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-10-12 14:04:29
 * @FilePath: /hlibc/example/main.c
 * @Description: 统一示例入口 - 支持动态和静态分配
 * @other: None
 */
#include <stdio.h>
#include "example.h"

int main(int argc, char** argv) {
  (void)argc;
  (void)argv;

  printf("---------list data struct test---------\n");
  list_example1();
  list_example2();
  list_example3();

  printf("---------stack data struct test---------\n");
  stack_example1();

  printf("---------queue data struct test---------\n");
  queue_example1();

  return 0;
}
