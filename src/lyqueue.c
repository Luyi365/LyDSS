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
#include "../inc/lydss_api.h"

struct LyQueue {
    unsigned long length;   ///< 队列可容纳的成员数量
    unsigned long item_size;    ///< 单个成员的大小（单位：字节）

    unsigned long en_index;     ///< 入队位置索引
    unsigned long de_index;     ///< 出队位置索引

    unsigned long count;        ///< 队列中成员的数量

    bool is_static;             ///< 成员区是否由调用者提供

    void *items;    ///< 成员区
};

/// @brief 创建一个队列
LyQueue_t *lyqueue_create(unsigned long len, unsigned long item_size)
{
    LYDSS_ASSERT(item_size);

    LyQueue_t *queue = malloc(sizeof(LyQueue_t));
    if(queue == NULL) {
        return NULL;
    }

    queue->items = calloc(len, item_size);
    if(queue->items == NULL) {
        free(queue);
        return NULL;
    }

    queue->length = len;
    queue->item_size = item_size;
    queue->en_index = 0;
    queue->de_index = 0;
    queue->count = 0;
    queue->is_static = false;

    return queue;
}

/// @brief 创建一个静态队列
void lyqueue_create_static(LyQueue_t *queue, void *buf, unsigned long len, unsigned long item_size)
{
    LYDSS_ASSERT(queue);
    LYDSS_ASSERT(buf);
    LYDSS_ASSERT(item_size);

    queue->length = len;
    queue->item_size = item_size;
    queue->en_index = 0;
    queue->de_index = 0;
    queue->count = 0;
    queue->is_static = true;
    queue->items = buf;
}

/// @brief 数据入队
bool lyqueue_send(LyQueue_t *queue, void *item)
{
    LYDSS_ASSERT(queue);
    LYDSS_ASSERT(item);

    if(queue->count >= queue->length) {
        return false;
    }

    memcpy((unsigned char *)queue->items + queue->en_index * queue->item_size, item, queue->item_size);
    queue->en_index = (queue->en_index + 1) % queue->length;
    queue->count++;

    return true;
}

/// @brief 数据出队
bool lyqueue_receive(LyQueue_t *queue, void *buf)
{
    LYDSS_ASSERT(queue);
    LYDSS_ASSERT(buf);

    if(queue->count == 0) {
        return false;
    }

    memcpy(buf, (unsigned char *)queue->items + queue->de_index * queue->item_size, queue->item_size);
    queue->de_index = (queue->de_index + 1) % queue->length;
    queue->count--;

    return true;
}

/// @brief 预读队列数据
bool lyqueue_peek(LyQueue_t *queue, void *buf)
{
    LYDSS_ASSERT(queue);
    LYDSS_ASSERT(buf);

    if(queue->count == 0) {
        return false;
    }

    memcpy(buf, (unsigned char *)queue->items + queue->de_index * queue->item_size, queue->item_size);

    return true;
}

/// @brief 清空队列
bool lyqueue_clear(LyQueue_t *queue)
{
    LYDSS_ASSERT(queue);

    queue->en_index = 0;
    queue->de_index = 0;
    queue->count = 0;

    return true;
}

/// @brief 删除队列
/// @note 静态队列只解除成员区绑定，不释放调用者提供的内存与句柄
void lyqueue_delete(LyQueue_t *queue)
{
    LYDSS_ASSERT(queue);

    if(queue->is_static == true) {
        queue->items = NULL;
        queue->en_index = 0;
        queue->de_index = 0;
        queue->count = 0;
        return;
    }

    free(queue->items);
    free(queue);
}

/// @brief 获取队列中当前的成员个数
unsigned long lyqueue_count(LyQueue_t *queue)
{
    LYDSS_ASSERT(queue);

    return queue->count;
}

/// @brief 判断队列是否为空
bool lyqueue_is_empty(LyQueue_t *queue)
{
    LYDSS_ASSERT(queue);

    return queue->count == 0;
}

/// @brief 判断队列是否已满
bool lyqueue_is_full(LyQueue_t *queue)
{
    LYDSS_ASSERT(queue);

    return queue->count >= queue->length;
}