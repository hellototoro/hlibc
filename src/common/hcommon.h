/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2022-10-12 14:07:17
 * @FilePath: /hlibc/common/hcommon.h
 * @Description: None
 * @other: None
 */
#ifndef __HLIBC_HCOMMON_H__
#define __HLIBC_HCOMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/*********************
 *      MACROS
 *********************/
#ifndef HLIBC_DISABLE_HEAP
#define HLIBC_DISABLE_HEAP 0
#endif

#if defined(__cplusplus)
#define HLIB_ALIGNOF(type) alignof(type)
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
#define HLIB_ALIGNOF(type) _Alignof(type)
#elif defined(_MSC_VER)
#define HLIB_ALIGNOF(type) __alignof(type)
#else
#define HLIB_ALIGNOF(type) sizeof(void*)
#endif

#define HLIB_MAX_ALIGN              ((size_t)HLIB_ALIGNOF(max_align_t))
#define HLIB_ALIGN_UP(value, align) (((size_t)(value) + ((size_t)(align) - 1U)) & ~((size_t)(align) - 1U))
#define HLIB_STATIC_STORAGE_SIZE(header_size, node_size, type_size, capacity) \
    ((size_t)(HLIB_MAX_ALIGN - 1U) + \
     HLIB_ALIGN_UP((header_size), HLIB_MAX_ALIGN) + \
     HLIB_ALIGN_UP((size_t)(node_size) * (size_t)(capacity), HLIB_MAX_ALIGN) + \
     HLIB_ALIGN_UP((type_size), HLIB_MAX_ALIGN) * (size_t)(capacity))

#define DATA_CAST(data_type)        *(data_type*)

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    HLIB_OK          =  1,
    HLIB_ERROR       =  0,
    HLIB_OVERFLOW    = -2
} hlib_status_t;

typedef enum {
    HLIB_STORAGE_HEAP   = 0,
    HLIB_STORAGE_STATIC = 1
} hlib_storage_mode_t;

typedef void* hdata_ptr_t;
typedef const void * hcdata_ptr_t;
typedef void (*copy_data_f)(hdata_ptr_t, hcdata_ptr_t);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
