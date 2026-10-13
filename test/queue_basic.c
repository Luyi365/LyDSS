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

int queue_basic(void)
{
    LyQueue_t *queue = lyqueue_create(4, sizeof(int));

    printf("创建容量 4 的队列，依次入队");
    for(int i = 1; i <= 4; i++) {
        lyqueue_send(queue, &i);
        printf(" %d", i);
    }
    printf("，队列%s\n", lyqueue_is_full(queue) ? "已满" : "未满");

    /* 队列的特征：先进先出；满时拒收而不覆盖既有成员 */
    int more = 5;
    bool sent = lyqueue_send(queue, &more);
    printf("再入队 %d：%s\n", more, sent ? "成功" : "失败，队列已满");

    int val = 0;
    printf("依次出队");
    while(lyqueue_is_empty(queue) == false) {
        lyqueue_receive(queue, &val);
        printf(" %d", val);
    }
    printf("\n");

    lyqueue_delete(queue);

    return 0;
}

#endif /* LYDSS_QUEUE_ENABLED */
