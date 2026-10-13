/**
 * @file set_basic.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief 集合用例
 * @date 2026-08-10
 *
 * @copyright Copyright (c) 2026
 */

#include "../inc/lydss_api.h"

#if LYDSS_SET_ENABLED

#include <stdio.h>
#include <string.h>

/// @brief 按字节比较相等，长度写死 sizeof(int)，仅适用于 int 元素
/// @note Equals_t 不接收长度参数，库无法提供默认实现，必须由调用者给出
static bool byte_equals(const void *a, const void *b)
{
    return memcmp(a, b, sizeof(int)) == 0;
}

int set_basic(void)
{
    /* hash 传 NULL 表示使用内置的按字节 FNV-1a */
    LySet_t *set = lyset_init(5, sizeof(int), NULL, byte_equals);

    int a = 1, b = 2;
    lyset_add(set, &a);
    lyset_add(set, &b);
    printf("添加 %d、%d\n", a, b);

    /* 集合的特征：元素唯一，重复添加被拒绝 */
    bool added = lyset_add(set, &b);
    printf("再添加 %d：%s\n", b, added ? "成功" : "失败，元素已存在");
    printf("元素个数：%zu\n", lyset_count(set));
    printf("是否包含 %d：%s\n", b, lyset_contains(set, &b) ? "是" : "否");

    lyset_destroy(set);

    return 0;
}

#endif /* LYDSS_SET_ENABLED */
