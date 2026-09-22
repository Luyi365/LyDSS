/**
 * @file run_all_basic_test.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief
 * @date 2025-07-16
 *
 * @copyright Copyright (c) 2026
 *
 * @note 每个调用由对应功能开关保护，裁剪掉的容器连同其测试一起跳过。
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

    /* 栈、队列的用例尚未接入（queue_basic 针对旧 API，需重写）；接入时记得加开关保护 */

#if LYDSS_STREAM_ENABLED
    /** ./test/stream_basic.c */
    extern int stream_basic();
    status |= stream_basic();
    printf("\n");
#endif

    return status;
}
