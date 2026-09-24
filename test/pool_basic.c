/**
 * @file pool_basic.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief 池用例
 * @date 2026-08-07
 *
 * @copyright Copyright (c) 2026
 */

#include "../inc/lydss_api.h"

#if LYDSS_POOL_ENABLED

#include <stdio.h>

int pool_basic()
{
    int a = 5, b = 6;
    LyPool_t *pool = lypool_create(10);

    /* 池的特征：以名字登记外部对象，只保留指针不复制 */
    lypool_post(pool, "TEST1", &a);
    lypool_post(pool, "TEST2", &b);

    printf("TEST1 = %d\n", *(int *)lypool_get(pool, "TEST1"));   // 5
    printf("miss  = %p\n", lypool_get(pool, "NONE"));            // (nil)

    lypool_clear(pool, "TEST1");
    printf("clear = %p\n", lypool_get(pool, "TEST1"));           // (nil)

    lypool_delete(pool);

    return 0;
}

#endif /* LYDSS_POOL_ENABLED */
