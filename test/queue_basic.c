/**
 * @file queue_basic.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief 队列用例
 * @date 2025-08-06
 *
 * @copyright Copyright (c) 2026
 */

#include "../inc/lydss_api.h"

#if LYDSS_QUEUE_ENABLED

#include <stdio.h>

int queue_basic()
{
    LyQueue_t *queue = lyqueue_create(4, sizeof(int));

    for(int i = 1; i <= 4; i++) {
        lyqueue_send(queue, &i);
    }
    printf("count = %lu, full = %d\n",
        lyqueue_count(queue), lyqueue_is_full(queue));      // 4, 1

    /* 队列的特征：先进先出；满时拒收而不覆盖既有成员 */
    int more = 5;
    printf("send5 = %d\n", lyqueue_send(queue, &more));      // 0

    int val = 0;
    printf("recv  = ");
    while(lyqueue_is_empty(queue) == false) {
        lyqueue_receive(queue, &val);
        printf("%d ", val);                                 // 1 2 3 4
    }
    printf("\n");

    lyqueue_delete(queue);

    return 0;
}

#endif /* LYDSS_QUEUE_ENABLED */
