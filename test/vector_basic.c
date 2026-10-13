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

int vector_basic(void)
{
    LyVector_t *vector = lyvector_create(2, sizeof(int));
    printf("创建初始容量 %zu 的动态数组\n", lyvector_capacity(vector));

    /* 变长容器的特征：容量随追加按倍数增长 */
    for(int i = 1; i <= 5; i++) {
        lyvector_push_rear(vector, &i);
        printf("追加 %d：长度 %zu，容量 %zu\n",
            i, lyvector_size(vector), lyvector_capacity(vector));
    }

    printf("当前内容");
    for(size_t i = 0; i < lyvector_size(vector); i++) {
        printf(" %d", *(int *)lyvector_get(vector, i));
    }
    printf("\n");

    int val = 0;
    lyvector_read(vector, 2, &val);
    printf("读取 [2]：%d\n", val);

    lyvector_pop_rear(vector, &val);
    printf("弹出尾部：%d，剩余长度 %zu\n", val, lyvector_size(vector));

    lyvector_delete(vector);

    return 0;
}

#endif /* LYDSS_VECTOR_ENABLED */
