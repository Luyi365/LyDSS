/**
 * @file lydss_cfg.h
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief LyDSS 的编译期配置
 * @date 2026-08-07
 *
 * @copyright Copyright (c) 2026
 *
 * @note
 * 所有开关的取值语义一致：非 0 启用，0 裁剪。
 * 每个开关都用 #ifndef 包裹，因此可以在构建系统里用 -DLYDSS_XXX=0 覆盖，
 * 不必改动本文件；未被覆盖时才取这里的默认值。
 */

#ifndef LYDSS_CFG_H
#define LYDSS_CFG_H

/* ----------------------------------- 断言 ----------------------------------- */
/**
 * @note
 * 启用时断言失败会打印表达式与所在函数名，然后进入陷阱且不再返回。
 * 关闭时断言退化为对表达式求值的空语句，不产生任何代码与输出依赖。
 */
#ifndef LYDSS_ASSERT_ENABLED
#define LYDSS_ASSERT_ENABLED 1
#endif

#if LYDSS_ASSERT_ENABLED
#include <stdio.h>      ///< 仅断言输出需要，裁剪断言后不引入
#endif

/* ---------------------------------- 功能裁剪 ---------------------------------- */
/**
 * @note
 * 每个容器都是独立的翻译单元，关闭后它在 lydss_api.h 中的声明与
 * 在 src/ly<名称>.c 中的实现都不参与编译，对应静态库成为空库。
 * 十个容器互不依赖，可以任意组合，关掉任意一个不会影响其余容器。
 * 开关顺序与 lydss_api.h 的分节顺序一致。
 */

/* 池 */
#ifndef LYDSS_POOL_ENABLED
#define LYDSS_POOL_ENABLED 1
#endif

/* 数组 */
#ifndef LYDSS_ARRAY_ENABLED
#define LYDSS_ARRAY_ENABLED 1
#endif

/* 变长数组 */
#ifndef LYDSS_VECTOR_ENABLED
#define LYDSS_VECTOR_ENABLED 1
#endif

/* 栈 */
#ifndef LYDSS_STACK_ENABLED
#define LYDSS_STACK_ENABLED 1
#endif

/* 队列 */
#ifndef LYDSS_QUEUE_ENABLED
#define LYDSS_QUEUE_ENABLED 1
#endif

/* 链表 */
#ifndef LYDSS_LIST_ENABLED
#define LYDSS_LIST_ENABLED 1
#endif

/* 流 */
#ifndef LYDSS_STREAM_ENABLED
#define LYDSS_STREAM_ENABLED 1
#endif

/* 表 */
#ifndef LYDSS_TABLE_ENABLED
#define LYDSS_TABLE_ENABLED 1
#endif

/* 集合 */
#ifndef LYDSS_SET_ENABLED
#define LYDSS_SET_ENABLED 1
#endif

/* 桶 */
#ifndef LYDSS_BUCKET_ENABLED
#define LYDSS_BUCKET_ENABLED 1
#endif

#endif        /* LYDSS_CFG_H */
