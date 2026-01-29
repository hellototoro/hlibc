/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-10-25 12:35:41
 * @FilePath: /hlibc/stack/hstack.c
 * @Description: None
 * @other: None
 */

/*********************
 *      INCLUDES
 *********************/
#include <string.h>
#include "hstack.h"
#include "../common/hlibc_type.h"

#if HLIBC_USE_STATIC_ALLOC == 0
#include <stdlib.h>
#endif

/*********************
 *      MACROS
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
#if HLIBC_USE_STATIC_ALLOC == 0
typedef struct hnode hstack_node_t;
struct hstack {
  uint32_t size;
  hstack_node_t* top;
};
#else
/* 静态分配使用数组实现栈 */
struct hstack {
  uint32_t size;
  uint32_t capacity;
  uint32_t type_size;
  uint8_t* data_pool; /* 数据存储池 */
};
#endif

/**********************
 *   GLOBAL VARIABLES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

#if HLIBC_USE_STATIC_ALLOC == 0
/* ==================== 动态分配实现 ==================== */

hstack_ptr_t hstack_create(uint32_t type_size)
{
  (void)type_size; /* 动态版本不需要记录 type_size */
  hstack_ptr_t stack = (hstack_ptr_t)malloc(sizeof(struct hstack));
  if (stack == NULL) return NULL;
  stack->top = NULL;
  stack->size = 0;
  return stack;
}

void hstack_destroy(hstack_ptr_t stack)
{
    hstack_clear(stack);
    free(stack);
}

/*=====================
 * Setter functions
 *====================*/

hlib_status_t hstack_push(hstack_ptr_t stack, hdata_ptr_t data_ptr, uint32_t data_size, copy_data_f copy_data)
{
    hstack_node_t *node = (hstack_node_t*) malloc(sizeof (hstack_node_t));
    if (node == NULL) return HLIB_ERROR;
    node->data_ptr = (hdata_ptr_t) malloc(data_size);
    if (node->data_ptr == NULL) {
      free(node);
      return HLIB_ERROR;
    }
    if (copy_data != NULL)
        copy_data(node->data_ptr, data_ptr);
    else
        memcpy(node->data_ptr, data_ptr, data_size);
    node->next = stack->top;
    stack->top = node;
    ++stack->size;
    return HLIB_OK;
}

hlib_status_t hstack_pop(hstack_ptr_t stack)
{
    if(stack->top == NULL) return HLIB_ERROR;
    hstack_node_t *p = stack->top;
    stack->top = stack->top->next;
    free(p->data_ptr);
    free(p);
    --stack->size;
    return HLIB_OK;
}

void hstack_clear(hstack_ptr_t stack)
{
    while (!hstack_empty(stack)) {
        hstack_pop(stack);
    }
}

/*=======================
 * Getter functions
 *======================*/

hdata_ptr_t hstack_top(hstack_ptr_t stack)
{
    return !hstack_empty(stack) ? stack->top->data_ptr : NULL;
}

bool hstack_empty(hstack_ptr_t stack)
{
    return (stack->size == 0);
}

uint32_t hstack_size(hstack_ptr_t stack)
{
    return stack->size;
}

#else /* HLIBC_USE_STATIC_ALLOC == 1 */
/* ==================== 静态分配实现 ==================== */

hstack_ptr_t hstack_create_static(void* buffer, uint32_t buffer_size,
                                  uint32_t type_size) {
  if (buffer == NULL || type_size == 0) return NULL;

  uint32_t header_size = sizeof(struct hstack);
  if (buffer_size <= header_size) return NULL;

  uint32_t remaining = buffer_size - header_size;
  uint32_t capacity = remaining / type_size;

  if (capacity == 0) return NULL;

  hstack_ptr_t stack = (hstack_ptr_t)buffer;
  stack->size = 0;
  stack->capacity = capacity;
  stack->type_size = type_size;
  stack->data_pool = (uint8_t*)buffer + header_size;

  return stack;
}

void hstack_destroy_static(hstack_ptr_t stack) {
  if (stack == NULL) return;
  stack->size = 0;
}

/*=====================
 * Setter functions
 *====================*/

hlib_status_t hstack_push(hstack_ptr_t stack, hdata_ptr_t data_ptr,
                          uint32_t data_size, copy_data_f copy_data) {
  if (stack->size >= stack->capacity) return HLIB_OVERFLOW;
  if (data_size != stack->type_size) return HLIB_ERROR;

  uint8_t* dest = stack->data_pool + stack->size * stack->type_size;
  if (copy_data != NULL)
    copy_data(dest, data_ptr);
  else
    memcpy(dest, data_ptr, data_size);

  ++stack->size;
  return HLIB_OK;
}

hlib_status_t hstack_pop(hstack_ptr_t stack) {
  if (stack->size == 0) return HLIB_ERROR;
  --stack->size;
  return HLIB_OK;
}

void hstack_clear(hstack_ptr_t stack) { stack->size = 0; }

/*=======================
 * Getter functions
 *======================*/

hdata_ptr_t hstack_top(hstack_ptr_t stack) {
  if (stack->size == 0) return NULL;
  return stack->data_pool + (stack->size - 1) * stack->type_size;
}

bool hstack_empty(hstack_ptr_t stack) { return (stack->size == 0); }

uint32_t hstack_size(hstack_ptr_t stack) { return stack->size; }

uint32_t hstack_capacity(hstack_ptr_t stack) { return stack->capacity; }

bool hstack_full(hstack_ptr_t stack) {
  return (stack->size >= stack->capacity);
}

#endif /* HLIBC_USE_STATIC_ALLOC */
