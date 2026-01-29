/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-10-25 32:37:34
 * @FilePath: /hlibc/queue/hqueue.c
 * @Description: None
 * @other: None
 */

/*********************
 *      INCLUDES
 *********************/
#include <string.h>
#include "hqueue.h"
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
typedef struct hnode queue_node_t;
struct hqueue {
    uint32_t size;
    queue_node_t *front, *rear;
};
#else
/* 静态分配使用环形队列实现 */
struct hqueue {
  uint32_t size;
  uint32_t capacity;
  uint32_t type_size;
  uint32_t head;      /* 队头索引 */
  uint32_t tail;      /* 队尾索引 */
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

hqueue_ptr_t hqueue_create(uint32_t type_size)
{
    hqueue_ptr_t queue = (hqueue_ptr_t) malloc(sizeof (struct hqueue));
    if (queue == NULL) return NULL;
    queue_node_t *head = (queue_node_t *) malloc(sizeof (queue_node_t));
    if (head == NULL) {
      free(queue);
      return NULL;
    }
    head->data_ptr = (hdata_ptr_t)malloc(type_size);
    if (head->data_ptr == NULL) {
      free(head);
      free(queue);
      return NULL;
    }
    memset(head->data_ptr, '\0', type_size);
    head->next = NULL;
    queue->front = queue->rear = head;
    queue->size = 0;
    return queue;
}

void hqueue_destroy(hqueue_ptr_t queue)
{
    hqueue_clear(queue);
    free(queue->front->data_ptr);
    free(queue->front);
    free(queue);
}

/*=====================
 * Setter functions
 *====================*/

hlib_status_t hqueue_push(hqueue_ptr_t queue, hdata_ptr_t data_ptr, uint32_t data_size, copy_data_f copy_data)
{
    queue_node_t *node = (queue_node_t*) malloc(sizeof (queue_node_t));
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
    node->next = NULL;
    queue->rear->next = node;
    queue->rear = node;
    ++queue->size;
    return HLIB_OK;
}

hlib_status_t hqueue_pop(hqueue_ptr_t queue)
{
    if(queue->rear == queue->front) return HLIB_ERROR;
    queue_node_t *p = queue->front->next;
    queue->front->next = p->next;
    if (queue->rear == p) queue->rear = queue->front;
    free(p->data_ptr);
    free(p);
    --queue->size;
    return HLIB_OK;
}

void hqueue_clear(hqueue_ptr_t queue)
{
    while(!hqueue_empty(queue)) {
        hqueue_pop(queue);
    }
}

/*=======================
 * Getter functions
 *======================*/

hdata_ptr_t hqueue_front(hqueue_ptr_t queue)
{
    return !hqueue_empty(queue) ? queue->front->next->data_ptr : queue->front->data_ptr;
}

hdata_ptr_t hqueue_rear(hqueue_ptr_t queue)
{
    return queue->rear->data_ptr;
}

bool hqueue_empty(hqueue_ptr_t queue)
{
    return (queue->size == 0);
}

uint32_t hqueue_size(hqueue_ptr_t queue)
{
    return queue->size;
}

#else /* HLIBC_USE_STATIC_ALLOC == 1 */
/* ==================== 静态分配实现（环形队列） ==================== */

hqueue_ptr_t hqueue_create_static(void* buffer, uint32_t buffer_size,
                                  uint32_t type_size) {
  if (buffer == NULL || type_size == 0) return NULL;

  uint32_t header_size = sizeof(struct hqueue);
  if (buffer_size <= header_size) return NULL;

  uint32_t remaining = buffer_size - header_size;
  uint32_t capacity = remaining / type_size;

  if (capacity == 0) return NULL;

  hqueue_ptr_t queue = (hqueue_ptr_t)buffer;
  queue->size = 0;
  queue->capacity = capacity;
  queue->type_size = type_size;
  queue->head = 0;
  queue->tail = 0;
  queue->data_pool = (uint8_t*)buffer + header_size;

  return queue;
}

void hqueue_destroy_static(hqueue_ptr_t queue) {
  if (queue == NULL) return;
  queue->size = 0;
  queue->head = 0;
  queue->tail = 0;
}

/*=====================
 * Setter functions
 *====================*/

hlib_status_t hqueue_push(hqueue_ptr_t queue, hdata_ptr_t data_ptr,
                          uint32_t data_size, copy_data_f copy_data) {
  if (queue->size >= queue->capacity) return HLIB_OVERFLOW;
  if (data_size != queue->type_size) return HLIB_ERROR;

  uint8_t* dest = queue->data_pool + queue->tail * queue->type_size;
  if (copy_data != NULL)
    copy_data(dest, data_ptr);
  else
    memcpy(dest, data_ptr, data_size);

  queue->tail = (queue->tail + 1) % queue->capacity;
  ++queue->size;
  return HLIB_OK;
}

hlib_status_t hqueue_pop(hqueue_ptr_t queue) {
  if (queue->size == 0) return HLIB_ERROR;
  queue->head = (queue->head + 1) % queue->capacity;
  --queue->size;
  return HLIB_OK;
}

void hqueue_clear(hqueue_ptr_t queue) {
  queue->size = 0;
  queue->head = 0;
  queue->tail = 0;
}

/*=======================
 * Getter functions
 *======================*/

hdata_ptr_t hqueue_front(hqueue_ptr_t queue) {
  if (queue->size == 0) return NULL;
  return queue->data_pool + queue->head * queue->type_size;
}

hdata_ptr_t hqueue_rear(hqueue_ptr_t queue) {
  if (queue->size == 0) return NULL;
  uint32_t rear_idx = (queue->tail + queue->capacity - 1) % queue->capacity;
  return queue->data_pool + rear_idx * queue->type_size;
}

bool hqueue_empty(hqueue_ptr_t queue) { return (queue->size == 0); }

uint32_t hqueue_size(hqueue_ptr_t queue) { return queue->size; }

uint32_t hqueue_capacity(hqueue_ptr_t queue) { return queue->capacity; }

bool hqueue_full(hqueue_ptr_t queue) {
  return (queue->size >= queue->capacity);
}

#endif /* HLIBC_USE_STATIC_ALLOC */
