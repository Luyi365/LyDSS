/**
 * @file run_all_basic_test.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief
 * @date 2025-07-16
 *
 * @copyright Copyright (c) 2026
 *
 * @note
 * 各容器的调用顺序与 lydss_api.h 的分节顺序一致。
 * 每个调用都由对应的功能开关保护，被裁剪的容器连同它的测试一起跳过。
 */

#include "../inc/lydss_cfg.h"

#include <stdio.h>
#include <stdlib.h>

int main()
{
    int status = 0;

#if LYDSS_POOL_ENABLED
    /** ./test/pool_basic.c */
    extern int pool_basic();
    status |= pool_basic();
    printf("\n");
#endif

#if LYDSS_ARRAY_ENABLED
    /** ./test/array_basic.c */
    extern int array_basic();
    status |= array_basic();
    printf("\n");
#endif

#if LYDSS_VECTOR_ENABLED
    /** ./test/vector_basic.c */
    extern int vector_basic();
    status |= vector_basic();
    printf("\n");
#endif

    /** ./test/stack_basic.c */
// #if LYDSS_STACK_ENABLED
//     extern int stack_basic();
//     status |= stack_basic();
//     printf("\n");
// #endif

    /** ./test/queue_basic.c */
// #if LYDSS_QUEUE_ENABLED
//     extern int queue_basic();
//     status |= queue_basic();
//     printf("\n");
// #endif

#if LYDSS_STREAM_ENABLED
    /** ./test/stream_basic.c */
    extern int stream_basic();
    status |= stream_basic();
    printf("\n");
#endif

    return status;
}
