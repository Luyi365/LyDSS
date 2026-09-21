/**
 * @file array_basic.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief
 * @date 2026-09-03
 *
 * @copyright Copyright (c) 2026
 *
 * @note
 * Matters related to this document.
 */

#include "../inc/lydss_api.h"

#if LYDSS_ARRAY_ENABLED

#include <stdio.h>

int array_basic()
{
    LyArray_t *array = lyarray_create(5, sizeof(int));

    printf("length    = %lu\n", lyarray_length(array));     // 5
    printf("item_size = %lu\n", lyarray_item_size(array));  // 4

    /* 全部槽位自始存在，创建时已清零 */
    int val = -1;
    lyarray_read(array, 4, &val);
    printf("slot4     = %d\n", val);                        // 0

    /* 任意下标可直接写入，无需按顺序追加 */
    int a = 10, b = 30;
    lyarray_set(array, 0, &a);
    lyarray_set(array, 3, &b);
    printf("dump      = ");
    for(unsigned long i = 0; i < lyarray_length(array); i++) {
        printf("%d ", *(int *)lyarray_get(array, i));        // 10 0 0 30 0
    }
    printf("\n");

    lyarray_read(array, 3, &val);
    printf("read3     = %d\n", val);                        // 30

    /* 边界对 length 判定 */
    printf("set5      = %d\n", lyarray_set(array, 5, &a));  // 0
    printf("read5     = %d\n", lyarray_read(array, 5, &val));// 0
    printf("get5      = %p\n", lyarray_get(array, 5));      // (nil)

    lyarray_delete(array);

    return 0;
}

#endif /* LYDSS_ARRAY_ENABLED */
