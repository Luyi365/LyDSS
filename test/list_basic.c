/**
 * @file list_basic.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief 链表用例
 * @date 2026-08-14
 *
 * @copyright Copyright (c) 2026
 */

#include "../inc/lydss_api.h"

#if LYDSS_LIST_ENABLED

#include <stdio.h>
#include <string.h>

int list_basic()
{
    LyList_t *list = lylist_create();

    int a = 1, b = 2, c = 3;
    lylist_push_rear(list, &a);
    lylist_push_rear(list, &b);
    lylist_push_front(list, &c);   /* 结果顺序: c -> a -> b */
    LYLIST_FOREACH(list, it) {
        printf("%d ", *(int *)it->data);
    }
    printf("\n");
    lylist_destroy(list);

    return 0;
}

#endif /* LYDSS_LIST_ENABLED */
