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

/// @brief 供集合使用的 FNV-1a 哈希
static unsigned fnv1a(const void *data, size_t len)
{
    const unsigned char *p = data;
    unsigned h = 2166136261u;
    for(size_t i = 0; i < len; i++) {
        h ^= p[i];
        h *= 16777619u;
    }
    return h;
}

/// @brief 按字节比较相等，长度写死 sizeof(int)，仅适用于 int 元素
static bool byte_equals(const void *a, const void *b)
{
    return memcmp(a, b, sizeof(int)) == 0;
}

int set_basic()
{
    LySet_t *set = lyset_init(5, sizeof(int), fnv1a, byte_equals);

    int a = 1, b = 2, c = 3;
    lyset_add(set, &a);
    lyset_add(set, &b);
    lyset_add(set, &c);

    int d = 2;
    if(lyset_contains(set, &d)) {
        printf("set contains %d\n", d);
    }
    else {
        printf("set does not contain %d\n", d);
    }

    return 0;
}

#endif /* LYDSS_SET_ENABLED */
