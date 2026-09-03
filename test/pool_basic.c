/**
 * @file pool_basic.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief
 * @date 2026-08-07
 *
 * @copyright Copyright (c) 2026
 *
 * @note
 * Matters related to this document.
 */

#include <stdio.h>
#include "../inc/lydss_api.h"

int pool_basic()
{
    int a = 5;
    LyPool_t *pool = lypool_create(10);

    lypool_post(pool, "TEST1", &a);

    printf("a = %d\n", *(int *)(lypool_get(pool, "TEST1")));

    return 0;
}