/**
 * @file simple.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief 简单用例，随时更改
 * @date 2025-07-16
 *
 * @copyright Copyright (c) 2025
 */

#include "../inc/lydss_api.h"

#include <stdio.h>
#include <string.h>

int simple(void)
{
    int a = 5, b = 6;
    LyPool_t *pool = lypool_create(2);

    /* 池的特征：以名字登记外部对象，只保留指针不复制 */
    lypool_post(pool, "TEST1", &a);
    lypool_post(pool, "TEST2", &b);
    printf("创建容量 2 的池，登记 TEST1 = %d、TEST2 = %d\n", a, b);

    bool posted = lypool_post(pool, "TEST3", &a);
    printf("再登记 TEST3：%s\n", posted ? "成功" : "失败，池已满");

    printf("取 TEST1：%d\n", *(int *)lypool_get(pool, "TEST1"));
    printf("取 NONE：%s\n", lypool_get(pool, "NONE") != NULL ? "找到" : "未找到");

    lypool_clear(pool, "TEST1");
    printf("清除 TEST1 后再取：%s\n", lypool_get(pool, "TEST1") != NULL ? "找到" : "未找到");

    lypool_delete(pool);

    return 0;
}
