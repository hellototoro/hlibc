<!--
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-10-26 00:17:22
 * @FilePath: /hlibc/README.md
 * @Description: Generic C data structures and container library with static/dynamic allocation support
 * @other: None
-->
# ***hlibc***
## 基于C语言的通用型数据结构与容器库

支持**动态分配**（malloc/free）和**静态分配**（用户提供缓冲区）两种模式，特别适合 MCU 等无动态内存环境。

博客地址：[https://www.cnblogs.com/hellototoro/p/16838272.html](https://www.cnblogs.com/hellototoro/p/16838272.html)

---

## 快速开始

### 动态分配模式（默认）
```bash
cd build
cmake ..
cmake --build . --config Release
./bin/Release/hlibc_example
```

### 静态分配模式（MCU 友好）
```bash
cd build
cmake -DHLIBC_USE_STATIC_ALLOC=ON ..
cmake --build . --config Release
./bin/Release/hlibc_example
```

---

## 核心特性

### 📦 容器
- **hlist** - 双向链表，支持随机位置插入/删除
- **hstack** - 栈（LIFO），支持 push/pop/top
- **hqueue** - 队列（FIFO），支持 push/pop/front/rear

### 🔄 双模式支持

| 特性 | 动态分配 | 静态分配 |
|------|--------|--------|
| 创建 API | `hlist_create(size)` | `hlist_create_static(buf, buf_size, elem_size)` |
| 内存来源 | malloc/free | 用户提供缓冲区 |
| 容量检查 | N/A | 支持 `hlist_capacity()` |
| 溢出保护 | N/A | 返回 `HLIB_OVERFLOW` |
| MCU 友好 | ❌ | ✅ |

### 💾 静态分配的优势
- ✅ 无动态内存分配，完全可预测
- ✅ 栈分配缓冲区，零堆碎片
- ✅ 适合嵌入式/MCU 环境
- ✅ 编译时容量配置

---

# **hlist** - 双向链表

### 描述
双向序列容器，用于将它们的元素保持为线性排列，并允许在序列的任何位置插入和删除。

⚠️ **注意**：对于堆数据来说，本库提供的 clear 函数不能释放开发者申请的堆空间，开发人员应该自己负责清理申请的堆空间。

### API 

#### 创建和删除

**动态分配：**
```c
hlist_ptr_t hlist_create(uint32_t type_size);
void hlist_destroy(hlist_ptr_t list);

/* 示例 */
hlist_ptr_t list = hlist_create(sizeof(int));
hlist_destroy(list);
```

**静态分配：**
```c
hlist_ptr_t hlist_create_static(void* buffer, uint32_t buffer_size, uint32_t type_size);
void hlist_destroy_static(hlist_ptr_t list);

/* 示例 */
static uint8_t my_buffer[HLIST_CALC_BUFFER_SIZE(int, 16)];  /* 16个int */
hlist_ptr_t list = hlist_create_static(my_buffer, sizeof(my_buffer), sizeof(int));
hlist_destroy_static(list);
```

#### 容量查询（仅静态模式）
```c
uint32_t hlist_capacity(hlist_ptr_t list);    /* 最大容量 */
uint32_t hlist_size(hlist_ptr_t list);        /* 当前大小 */
int hlist_empty(hlist_ptr_t list);            /* 是否为空 */
```

#### 迭代器
```c
hlist_iterator_ptr_t it = hlist_begin(list);
hlist_iter_forward(&it);
printf("it = %d\n", *(int*)hlist_iter_data(it));

/* 遍历 */
for (hlist_iterator_ptr_t it = hlist_begin(list); it != hlist_end(list); hlist_iter_forward(&it)) {
    printf("%d ", *(int*)hlist_iter_data(it));
}
```

#### 插入元素
```c
/* 头部/尾部插入 */
hlib_status_t hlist_push_back(hlist_ptr_t list, const void* data, uint32_t data_size);
hlib_status_t hlist_push_front(hlist_ptr_t list, const void* data, uint32_t data_size);

/* 迭代器指定位置插入 */
hlib_status_t hlist_insert(hlist_ptr_t list, hlist_iterator_ptr_t it, const void* data, uint32_t data_size);

/* 示例 */
struct test_str {
    char a;
    int b;
};

/* 动态分配 */
hlist_ptr_t list = hlist_create(sizeof(struct test_str));
struct test_str t1 = {'a', 10};
hlist_push_back(list, &t1, sizeof(t1));

/* 静态分配 */
static uint8_t buf[HLIST_CALC_BUFFER_SIZE(struct test_str, 8)];
hlist_ptr_t list = hlist_create_static(buf, sizeof(buf), sizeof(struct test_str));
hlib_status_t status = hlist_push_back(list, &t1, sizeof(t1));
if (status == HLIB_OVERFLOW) {
    printf("List is full!\n");
}
```

#### 删除元素
```c
void hlist_pop_back(hlist_ptr_t list);
void hlist_pop_front(hlist_ptr_t list);
void hlist_remove(hlist_ptr_t list, hlist_iterator_ptr_t it);
```

---

# **hstack** - 栈

### 描述
Stack 容器遵循 LIFO（后进先出）语义。堆栈上最后推送的元素将第一个弹出。

### API 

#### 创建和删除

**动态分配：**
```c
hstack_ptr_t hstack_create(uint32_t type_size);
void hstack_destroy(hstack_ptr_t stack);
```

**静态分配：**
```c
hstack_ptr_t hstack_create_static(void* buffer, uint32_t buffer_size, uint32_t type_size);
void hstack_destroy_static(hstack_ptr_t stack);

/* 示例 */
static uint8_t buf[HSTACK_CALC_BUFFER_SIZE(int, 32)];  /* 32个int */
hstack_ptr_t stack = hstack_create_static(buf, sizeof(buf), sizeof(int));
```

#### 基本操作
```c
/* push/pop */
hlib_status_t hstack_push(hstack_ptr_t stack, const void* data, uint32_t data_size, void (*copy)(void*, const void*));
void hstack_pop(hstack_ptr_t stack);

/* 查询 */
void* hstack_top(hstack_ptr_t stack);
uint32_t hstack_size(hstack_ptr_t stack);
uint32_t hstack_capacity(hstack_ptr_t stack);  /* 仅静态模式 */
int hstack_empty(hstack_ptr_t stack);
```

---

# **hqueue** - 队列

### 描述
Queue 容器遵循 FIFO（先进先出）语义。第一个推送的元素将第一个弹出。

### API 

#### 创建和删除

**动态分配：**
```c
hqueue_ptr_t hqueue_create(uint32_t type_size);
void hqueue_destroy(hqueue_ptr_t queue);
```

**静态分配：**
```c
hqueue_ptr_t hqueue_create_static(void* buffer, uint32_t buffer_size, uint32_t type_size);
void hqueue_destroy_static(hqueue_ptr_t queue);

/* 示例 */
static uint8_t buf[HQUEUE_CALC_BUFFER_SIZE(int, 8)];  /* 8个int */
hqueue_ptr_t queue = hqueue_create_static(buf, sizeof(buf), sizeof(int));
```

#### 基本操作
```c
/* push/pop */
hlib_status_t hqueue_push(hqueue_ptr_t queue, const void* data, uint32_t data_size, void (*copy)(void*, const void*));
void hqueue_pop(hqueue_ptr_t queue);

/* 查询 */
void* hqueue_front(hqueue_ptr_t queue);
void* hqueue_rear(hqueue_ptr_t queue);
uint32_t hqueue_size(hqueue_ptr_t queue);
uint32_t hqueue_capacity(hqueue_ptr_t queue);  /* 仅静态模式 */
int hqueue_empty(hqueue_ptr_t queue);
```

---

## 编译配置选项

### HLIBC_USE_STATIC_ALLOC
- **ON**: 启用静态分配模式（所有容器使用静态缓冲区）
- **OFF**: 启用动态分配模式（默认，使用 malloc/free）

### HLIBC_BUILD_EXAMPLES
- **ON**: 编译示例程序（默认）
- **OFF**: 仅编译库

---

## 常见问题

**1、为什么要写这个库？**<br>
在C语言中，由于不能泛型编程，所以能开箱即用的数据结构很少，一般都要自己造轮子。hlibc 提供了通用的容器实现，支持任意数据类型。

**2、如何使用 C 语言进行"泛型"编程？**<br>
虽然在C语言中不能使用模板，但是可以使用万能指针（void*）来替代。

**3、什么时候选择静态分配模式？**<br>
- 嵌入式/MCU 环境（无动态内存）
- 需要完全可预测的内存分配
- 实时系统对内存碎片敏感
- 容量固定且已知

**4、如何计算缓冲区大小？**<br>
使用提供的宏：
```c
HLIST_CALC_BUFFER_SIZE(type, capacity)   /* list */
HSTACK_CALC_BUFFER_SIZE(type, capacity)  /* stack */
HQUEUE_CALC_BUFFER_SIZE(type, capacity)  /* queue */
```

**5、静态模式下容量溢出了怎么办？**<br>
insert/push 操作会返回 `HLIB_OVERFLOW` 状态码，可以检查返回值判断是否溢出：
```c
hlib_status_t status = hlist_push_back(list, &data, sizeof(data));
if (status == HLIB_OVERFLOW) {
    printf("List is full!\n");
}
```

---

## 许可证

根据项目主页确定
