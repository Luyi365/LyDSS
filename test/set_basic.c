/**
 * @file set_basic.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief
 * @date 2026-08-10
 *
 * @copyright Copyright (c) 2026
 *
 * @note
 * Matters related to this document.
 */

#include "../inc/lydss_api.h"

#if LYDSS_SET_ENABLED

#include <stdio.h>
#include <string.h>

/* ---------- 哈希函数（FNV-1a） ---------- */
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

/* ---------- 默认相等比较（按字节） ---------- */
static bool byte_equals(const void *a, const void *b)
{
    return memcmp(a, b, sizeof(int)) == 0; /* 示例按 int 比较 */
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
