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
#include <stdlib.h>

int list_basic()
{
    LyList_t *list = lylist_create();

    int a = 1, b = 2, c = 3, d = 4;
    LyListNode_t *node_a = lylist_push_rear(list, &a);
    lylist_push_rear(list, &b);
    lylist_push_front(list, &c);

    /* 链表的特征：可在任意已有节点前后插入，顺序 c -> a -> d -> b */
    lylist_insert_after(list, node_a, &d);

    LYLIST_FOREACH(list, it) {
        printf("%d ", *(int *)it->data);
    }
    printf("\n");

    printf("pop front = %d\n", *(int *)lylist_pop_front(list));  // 3

    lylist_destroy(list);
    free(list);

    return 0;
}

#endif /* LYDSS_LIST_ENABLED */
