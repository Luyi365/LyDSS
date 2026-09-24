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
    LyVector_t *vector = lyvector_create(2, sizeof(int));

    /* 变长容器的特征：容量随追加按倍数增长，容量序列 2 2 4 4 8 */
    for(int i = 1; i <= 5; i++) {
        lyvector_push_rear(vector, &i);
        printf("push %d -> size = %lu, capacity = %lu\n",
            i, lyvector_size(vector), lyvector_capacity(vector));
    }

    printf("dump     = ");
    for(unsigned long i = 0; i < lyvector_size(vector); i++) {
        printf("%d ", *(int *)lyvector_get(vector, i));          // 1 2 3 4 5
    }
    printf("\n");

    int val = 0;
    lyvector_read(vector, 2, &val);
    printf("read2    = %d\n", val);                             // 3

    lyvector_pop_rear(vector, &val);
    printf("pop      = %d, size = %lu\n", val, lyvector_size(vector));  // 5, 4

    lyvector_delete(vector);

    return 0;
}

#endif /* LYDSS_VECTOR_ENABLED */
