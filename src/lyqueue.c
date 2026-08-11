/**
 * @file lyqueue.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief
 * @date 2026-08-07
 *
 * @copyright Copyright (c) 2026
 *
 * @note
 * Matters related to this document.
 */


#include <stdlib.h>
#include <string.h>
#include <stdint.h> 
#include <stddef.h>
#include <stdbool.h>
#include <malloc.h>
#include "../inc/lydds_api.h"

struct LyQueue {
    unsigned long length;   ///< 队列可容纳的成员数量
    unsigned long item_size;    ///< 单个成员的大小（单位：字节）

    unsigned long en_index;     ///< 入队位置索引
    unsigned long de_index;     ///< 出队位置索引

    unsigned long count;        ///< 队列中成员的数量

    void *items;    ///< 成员区
};

/// @brief 创建一个队列
LyQueue_t *lyqueue_create(unsigned long length, unsigned long item_size)
{
    LYDDS_ASSERT(item_size);

    LyQueue_t *queue = malloc(sizeof(LyQueue_t));

    queue->length = length;
    queue->item_size = item_size;
    queue->en_index = 0;
    queue->de_index = 0;
    queue->count = 0;

    queue->items = calloc(length, item_size);

    return queue;
}

/// @brief 创建一个静态队列
void lyqueue_create_static(LyQueue_t *queue, void *buffer, unsigned long length, unsigned long item_size)
{
    LYDDS_ASSERT(queue);
    LYDDS_ASSERT(buffer);
    LYDDS_ASSERT(item_size);

    queue->length = length;
    queue->item_size = item_size;
    queue->en_index = 0;
    queue->de_index = 0;
    queue->count = 0;

    queue->items = buffer;
}

/// @brief 数据入队
bool lyqueue_send(LyQueue_t *queue, void *item)
{
    LYDDS_ASSERT(queue);
    LYDDS_ASSERT(item);

    memcpy(queue->items + queue->en_index * queue->item_size, item, queue->item_size);
    queue->en_index = (queue->en_index + 1) % queue->length;
    queue->count++;

    return true;
}

/// @brief 数据出队
bool lyqueue_receive(LyQueue_t *queue, void *buff)
{
    LYDDS_ASSERT(queue);
    LYDDS_ASSERT(buff);

    memcpy(buff, queue->items + queue->de_index * queue->item_size, queue->item_size);
    queue->de_index = (queue->de_index + 1) % queue->length;
    queue->count--;

    return true;
}

/// @brief 预读队列数据
bool lyqueue_peek(LyQueue_t *queue, void *buff)
{
    LYDDS_ASSERT(queue);
    LYDDS_ASSERT(buff);

    memcpy(buff, queue->items + queue->de_index * queue->item_size, queue->item_size);

    return true;
}

/// @brief 清空队列
bool lyqueue_clear(LyQueue_t *queue)
{
    LYDDS_ASSERT(queue);

    queue->en_index = 0;
    queue->de_index = 0;
    queue->count = 0;

    return true;
}

/// @brief 删除队列
void lyqueue_delete(LyQueue_t *queue)
{
    LYDDS_ASSERT(queue);

    free(queue->items);
    free(queue);
}

/// @brief 判断队列是否已满
bool lyqueue_is_full(LyQueue_t *queue)
{
    LYDDS_ASSERT(queue);

    return queue->count >= queue->length;
}