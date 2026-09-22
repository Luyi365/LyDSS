/**
 * @file vector_basic.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief 动态数组用例
 * @date 2026-09-03
 *
 * @copyright Copyright (c) 2026
 */

#include "../inc/lydss_api.h"

#if LYDSS_VECTOR_ENABLED

#include <stdio.h>

int vector_basic()
{
    /* 初始容量 2，观察倍增过程 */
    LyVector_t *vector = lyvector_create(2, sizeof(int));

    printf("empty     = %d\n", lyvector_is_empty(vector));       // 1

    for(int i = 1; i <= 5; i++) {
        lyvector_push_rear(vector, &i);
        printf("push %d -> size = %lu, capacity = %lu\n",
               i, lyvector_size(vector), lyvector_capacity(vector));
    }
    /* 容量序列: 2 2 4 4 8 */

    printf("dump      = ");
    for(unsigned long i = 0; i < lyvector_size(vector); i++) {
        printf("%d ", *(int *)lyvector_get(vector, i));           // 1 2 3 4 5
    }
    printf("\n");

    /* 重分配可能迁移成员区也可能原地扩展，moved 两种取值都合法，
       调用者一律不能沿用旧地址；无论哪种，已有成员必须完整保留 */
    void *before = lyvector_get(vector, 0);
    lyvector_reserve(vector, 64);
    void *after = lyvector_get(vector, 0);
    printf("moved     = %d, capacity = %lu\n",
           before != after, lyvector_capacity(vector));           // 0 或 1, 64
    printf("kept      = ");
    for(unsigned long i = 0; i < lyvector_size(vector); i++) {
        printf("%d ", *(int *)lyvector_get(vector, i));           // 1 2 3 4 5
    }
    printf("\n");

    int val = 0;
    lyvector_read(vector, 2, &val);
    printf("read2     = %d\n", val);                              // 3

    int x = 99;
    lyvector_set(vector, 2, &x);
    lyvector_read(vector, 2, &val);
    printf("set2      = %d\n", val);                              // 99

    /* 边界对 size 而非 capacity */
    printf("set5      = %d\n", lyvector_set(vector, 5, &x));      // 0
    printf("get5      = %p\n", lyvector_get(vector, 5));          // (nil)

    /* pop 只减 size，容量不动 */
    lyvector_pop_rear(vector, &val);
    printf("pop       = %d, size = %lu, capacity = %lu\n",
           val, lyvector_size(vector), lyvector_capacity(vector)); // 5, 4, 64

    /* 释放容量需显式收缩，收缩同样不能损坏已有成员 */
    lyvector_shrink(vector);
    printf("shrink    -> size = %lu, capacity = %lu, kept = ",
           lyvector_size(vector), lyvector_capacity(vector));      // 4, 4
    for(unsigned long i = 0; i < lyvector_size(vector); i++) {
        printf("%d ", *(int *)lyvector_get(vector, i));            // 1 2 99 4
    }
    printf("\n");

    /* clear 只归零 size */
    lyvector_clear(vector);
    printf("clear     -> size = %lu, capacity = %lu, empty = %d\n",
           lyvector_size(vector), lyvector_capacity(vector),
           lyvector_is_empty(vector));                             // 0, 4, 1

    printf("pop empty = %d\n", lyvector_pop_rear(vector, &val));   // 0

    lyvector_delete(vector);

    return 0;
}

#endif /* LYDSS_VECTOR_ENABLED */
