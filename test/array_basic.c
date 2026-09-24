/**
 * @file array_basic.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief 数组用例
 * @date 2026-09-03
 *
 * @copyright Copyright (c) 2026
 */

#include "../inc/lydss_api.h"

#if LYDSS_ARRAY_ENABLED

#include <stdio.h>

int array_basic()
{
    LyArray_t *array = lyarray_create(5, sizeof(int));

    printf("length    = %lu\n", lyarray_length(array));         // 5
    printf("item_size = %lu\n", lyarray_item_size(array));      // 4

    /* 定长容器的特征：槽位自始存在，可按任意下标直接写入，无需先追加 */
    int a = 10, b = 30;
    lyarray_set(array, 0, &a);
    lyarray_set(array, 3, &b);

    printf("dump      = ");
    for(unsigned long i = 0; i < lyarray_length(array); i++) {
        printf("%d ", *(int *)lyarray_get(array, i));           // 10 0 0 30 0
    }
    printf("\n");

    int val = 0;
    lyarray_read(array, 3, &val);
    printf("read3     = %d\n", val);                            // 30

    lyarray_delete(array);

    return 0;
}

#endif /* LYDSS_ARRAY_ENABLED */
