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

int set_basic()
{
    /* hash 传 NULL 表示使用内置的按字节 FNV-1a */
    LySet_t *set = lyset_init(5, sizeof(int), NULL, byte_equals);

    int a = 1, b = 2;
    lyset_add(set, &a);
    lyset_add(set, &b);

    /* 集合的特征：元素唯一，重复添加被拒绝 */
    printf("add dup  = %d\n", lyset_add(set, &b));               // 0
    printf("count    = %u\n", (unsigned)lyset_count(set));       // 2
    printf("contains = %d\n", lyset_contains(set, &b));          // 1

    lyset_destroy(set);

    return 0;
}

#endif /* LYDSS_SET_ENABLED */
