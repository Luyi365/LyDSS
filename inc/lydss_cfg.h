/**
 * @file lydss_cfg.h
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief LyDSS 的编译期配置
 * @date 2026-08-07
 *
 * @copyright Copyright (c) 2026
 *
 * @note
 * 所有开关取值语义一致：非 0 启用，0 裁剪。
 * 开关一律用 #ifndef 包裹，构建系统里 -DLYDSS_XXX=0 即可覆盖，不必改动本文件；
 * 这是配置本库的推荐方式，这里的值只是未被覆盖时的默认值。
 */

#ifndef LYDSS_CFG_H
#define LYDSS_CFG_H

/* ----------------------------------- 断言 ----------------------------------- */
/**
 * @note
 * 启用时断言失败会打印表达式与所在函数名，随后进入陷阱且**不再返回** ——
 * 因此不能依赖断言来处理可恢复的错误，那类情况应由返回值表达。
 * 关闭时断言退化为对表达式求值的空语句。
 */
#ifndef LYDSS_ASSERT_ENABLED
#define LYDSS_ASSERT_ENABLED 1
#endif

#if LYDSS_ASSERT_ENABLED

/* 断言失败时的输出。MicroLIB 下 printf 需要工程重定向 fputc */
#ifndef LYDSS_PRINTF
#include <stdio.h>
#define LYDSS_PRINTF(...) printf(__VA_ARGS__)
#endif

/* 断言失败后的陷阱动作。默认为空，以保证任何架构都能编译 */
#ifndef LYDSS_TRAP
#define LYDSS_TRAP()
#endif
#endif

/* ---------------------------------- 功能裁剪 ---------------------------------- */
/**
 * @note
 * 关闭某个容器后，它在 lydss_api.h 中的声明与在 src/ 中的实现都不参与编译，
 * 对应静态库成为空库。十一个容器互不依赖，可以任意组合。
 * 排列顺序与 lydss_api.h 的分节顺序一致，新增开关请插入对应位置。
 *
 * 新增容器时必须同步四处：本文件的默认值、lydss_api.h 的声明块、
 * src/ 的实现，以及 test/ 用例与 examples/ 运行器里的调用点。
 * 只改其中一处会导致裁剪后链接失败。
 */

#ifndef LYDSS_POOL_ENABLED
#define LYDSS_POOL_ENABLED 1
#endif

#ifndef LYDSS_ARRAY_ENABLED
#define LYDSS_ARRAY_ENABLED 1
#endif

#ifndef LYDSS_VECTOR_ENABLED
#define LYDSS_VECTOR_ENABLED 1
#endif

#ifndef LYDSS_PINGPONG_ENABLED
#define LYDSS_PINGPONG_ENABLED 1
#endif

#ifndef LYDSS_STACK_ENABLED
#define LYDSS_STACK_ENABLED 1
#endif

#ifndef LYDSS_QUEUE_ENABLED
#define LYDSS_QUEUE_ENABLED 1
#endif

#ifndef LYDSS_LIST_ENABLED
#define LYDSS_LIST_ENABLED 1
#endif

#ifndef LYDSS_STREAM_ENABLED
#define LYDSS_STREAM_ENABLED 1
#endif

#ifndef LYDSS_TABLE_ENABLED
#define LYDSS_TABLE_ENABLED 1
#endif

#ifndef LYDSS_SET_ENABLED
#define LYDSS_SET_ENABLED 1
#endif

#ifndef LYDSS_BUCKET_ENABLED
#define LYDSS_BUCKET_ENABLED 0
#endif

#endif        /* LYDSS_CFG_H */
