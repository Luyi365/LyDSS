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

int table_basic(void)
{
    int a = 10, b = 20;
    LyTable_t *table = lytable_create(NULL);     /* NULL 表示使用内置哈希 */

    lytable_set(table, "alpha", &a);
    lytable_set(table, "beta", &b);
    printf("设置 alpha = %d、beta = %d，共 %zu 个键\n", a, b, lytable_size(table));

    /* 表的特征：字符串键映射到值指针，未命中返回 NULL */
    printf("查 alpha：%d\n", *(int *)lytable_get(table, "alpha"));
    printf("查 gamma：%s\n", lytable_get(table, "gamma") != NULL ? "找到" : "未找到");

    int *gone = lytable_remove(table, "beta");
    printf("删除 beta：取回 %d，剩余 %zu 个键\n", *gone, lytable_size(table));

    lytable_destroy(table);

    return 0;
}

#endif /* LYDSS_TABLE_ENABLED */
