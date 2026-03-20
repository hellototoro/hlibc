# hlibc

基于 C 语言的通用型数据结构与容器库，当前包含：

- `hlist`: 双向链表
- `hstack`: 栈
- `hqueue`: 队列

现在同时支持两种使用模式：

- 桌面/OS 模式：默认启用，容器对象和节点使用堆分配。
- MCU 模式：启用 `HLIBC_DISABLE_HEAP=ON`，完全去除 `malloc/free` 依赖，由调用方提供静态缓冲区。

## Build

桌面/OS 模式：

```bash
cmake -S example -B build/example
cmake --build build/example
```

MCU/无堆模式：

```bash
cmake -S example -B build/example-mcu -DHLIBC_DISABLE_HEAP=ON
cmake --build build/example-mcu
```

## Dynamic Mode

默认构建下保留原有 `create/destroy` 接口。

```c
#include "stack/hstack.h"

hstack_ptr_t stack = hstack_create(sizeof(int));
int value = 10;
hstack_push(stack, &value, sizeof(value), NULL);
hstack_destroy(stack);
```

`list`、`stack`、`queue` 都会按值拷贝元素内容。如果元素类型本身是指针，则容器只保存指针值，不负责释放外部对象。

## MCU Static Mode

MCU 模式下不导出 `*_create()`，改为由调用方提供一段连续缓冲区：

```c
#include <stdint.h>
#include "queue/hqueue.h"

uint8_t queue_buffer[HQUEUE_STATIC_SIZE(sizeof(int), 8)];
hqueue_ptr_t queue = hqueue_init_static(
    queue_buffer,
    sizeof(queue_buffer),
    sizeof(int),
    8
);
```

对应接口：

- `hlist_init_static(void *buffer, size_t buffer_size, uint32_t type_size, uint32_t capacity)`
- `hstack_init_static(void *buffer, size_t buffer_size, uint32_t type_size, uint32_t capacity)`
- `hqueue_init_static(void *buffer, size_t buffer_size, uint32_t type_size, uint32_t capacity)`

对应缓冲区大小宏：

- `HLIST_STATIC_SIZE(type_size, capacity)`
- `HSTACK_STATIC_SIZE(type_size, capacity)`
- `HQUEUE_STATIC_SIZE(type_size, capacity)`

如果缓冲区不足或 `type_size == 0`，初始化返回 `NULL`。

### Capacity

静态容器容量固定，插入满时返回 `HLIB_OVERFLOW`：

```c
uint8_t stack_buffer[HSTACK_STATIC_SIZE(sizeof(int), 2)];
hstack_ptr_t stack = hstack_init_static(stack_buffer, sizeof(stack_buffer), sizeof(int), 2);

int a = 1;
int b = 2;
int c = 3;

hstack_push(stack, &a, sizeof(a), NULL);
hstack_push(stack, &b, sizeof(b), NULL);
hlib_status_t status = hstack_push(stack, &c, sizeof(c), NULL);
/* status == HLIB_OVERFLOW */
```

### Pointer Elements

静态模式同样只拷贝指针值，不托管指针指向的对象生命周期：

```c
struct item {
    int id;
};

uint8_t list_buffer[HLIST_STATIC_SIZE(sizeof(struct item *), 2)];
hlist_ptr_t list = hlist_init_static(list_buffer, sizeof(list_buffer), sizeof(struct item *), 2);

struct item items[2] = {{1}, {2}};
struct item *p = &items[0];
hlist_push_back(list, &p, sizeof(p));
```

## API Notes

- `hstack_push` / `hqueue_push` 会校验 `data_size == type_size`。
- `hlist_insert` / `hlist_push_front` / `hlist_push_back` 也会校验 `data_size == type_size`。
- 静态模式容量满时，`push/insert` 返回 `HLIB_OVERFLOW`。
- `hlist_end()` 现在表示尾后哨兵，遍历方式如下：

```c
for (hlist_iterator_ptr_t it = hlist_begin(list); it != hlist_end(list); hlist_iter_forward(&it)) {
    /* ... */
}
```

## Containers

### hlist

```c
#include "list/hlist.h"

hlist_ptr_t list = hlist_init_static(buffer, sizeof(buffer), sizeof(int), 4);
int value = 42;
hlist_push_front(list, &value, sizeof(value));
hlist_push_back(list, &value, sizeof(value));
hlist_destroy(list);
```

### hstack

```c
#include "stack/hstack.h"

hstack_ptr_t stack = hstack_init_static(buffer, sizeof(buffer), sizeof(int), 4);
int value = 42;
hstack_push(stack, &value, sizeof(value), NULL);
hstack_pop(stack);
hstack_destroy(stack);
```

### hqueue

```c
#include "queue/hqueue.h"

hqueue_ptr_t queue = hqueue_init_static(buffer, sizeof(buffer), sizeof(int), 4);
int value = 42;
hqueue_push(queue, &value, sizeof(value), NULL);
hqueue_pop(queue);
hqueue_destroy(queue);
```
