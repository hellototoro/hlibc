/*
 * @Author: totoro huangjian921@outlook.com
 * @Date: 2026-01-28
 * @FilePath: /hlibc/common/hlibc_config.h
 * @Description: hlibc configuration header
 * @other: None
 */
#ifndef __HLIBC_CONFIG_H__
#define __HLIBC_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *  COMPILE OPTIONS
 *********************/

/**
 * 内存分配模式：
 * 0 - 动态分配 (使用 malloc/free)
 * 1 - 静态分配 (用户提供 buffer)
 *
 * 可以在编译时通过 -DHLIBC_USE_STATIC_ALLOC=1 来定义
 */
#ifndef HLIBC_USE_STATIC_ALLOC
#define HLIBC_USE_STATIC_ALLOC 0
#endif

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* __HLIBC_CONFIG_H__ */
