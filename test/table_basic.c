/**
 * @file table_basic.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief 表用例
 * @date 2026-08-23
 *
 * @copyright Copyright (c) 2026
 */

#include "../inc/lydss_api.h"

#if LYDSS_TABLE_ENABLED

#include <stdio.h>

int table_basic()
{
    int a = 10, b = 20;
    LyTable_t *table = lytable_create(NULL);     /* NULL 表示使用内置哈希 */

    lytable_set(table, "alpha", &a);
    lytable_set(table, "beta", &b);
    printf("size   = %u\n", (unsigned)lytable_size(table));          // 2

    /* 表的特征：字符串键映射到值指针，未命中返回 NULL */
    printf("alpha  = %d\n", *(int *)lytable_get(table, "alpha"));    // 10
    printf("miss   = %p\n", lytable_get(table, "gamma"));            // (nil)

    int *gone = lytable_remove(table, "beta");
    printf("remove = %d, size = %u\n", *gone, (unsigned)lytable_size(table));  // 20, 1

    lytable_destroy(table);

    return 0;
}

#endif /* LYDSS_TABLE_ENABLED */
