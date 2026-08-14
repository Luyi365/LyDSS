/**
 * @file list_basic.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief
 * @date 2026-08-14
 *
 * @copyright Copyright (c) 2026
 *
 * @note
 * Matters related to this document.
 */

#include <stdio.h>
#include <string.h>
#include "../inc/lydds_api.h"

int list_basic()
{
    LyList_t *list = lylist_creatr();

    int a = 1, b = 2, c = 3;
    lylist_push_tail(list, &a);
    lylist_push_tail(list, &b);
    lylist_push_front(list, &c);   /* 结果顺序: c -> a -> b */
    LYLIST_FOREACH(list, it) {
        printf("%d ", *(int *)it->data);
    }
    printf("\n");
    lylist_destroy(list);

    return 0;
}