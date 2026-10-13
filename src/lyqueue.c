/**
 * @file lyqueue.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief 定长环形队列
 * @date 2026-08-07
 *
 * @copyright Copyright (c) 2026
 *
 * @note send/receive 是全有或全无，与流的部分传输刻意不同。
 */


#include "../inc/lydss_api.h"

#if LYDSS_QUEUE_ENABLED

#include "../inc/lydss_priv.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

struct LyQueue {
    LyStore_t store;                ///< 成员区描述（地址、容量、成员大小）
    size_t en_index;                ///< 入队位置索引
    size_t de_index;                ///< 出队位置索引
    size_t count;                   ///< 队列中成员的数量
    bool is_static;                 ///< 成员区是否由调用者提供
};

_Static_assert(sizeof(struct LyQueue) <= sizeof(LyQueueStatic_t)
               && _Alignof(struct LyQueue) <= _Alignof(LyQueueStatic_t),
               "LyQueueStatic_t is too small or under-aligned for struct LyQueue");

/// @brief 创建一个队列
LyQueue_t *lyqueue_create(size_t len, size_t item_size)
{
    LYDSS_ASSERT(item_size);

    LyQueue_t *queue = malloc(sizeof(LyQueue_t));
    if(queue == NULL) {
        return NULL;
    }

    if(lystore_alloc(&queue->store, len, item_size, true) == false) {
        free(queue);
        return NULL;
    }

    queue->en_index = 0;
    queue->de_index = 0;
    queue->count = 0;
    queue->is_static = false;

    return queue;
}

/// @brief 创建一个静态队列
LyQueue_t *lyqueue_create_static(LyQueueStatic_t *storage, void *buf, size_t len, size_t item_size)
{
    LYDSS_ASSERT(storage);
    LYDSS_ASSERT(buf);
    LYDSS_ASSERT(item_size);

    LyQueue_t *queue = (LyQueue_t *)storage;
    lystore_bind(&queue->store, buf, len, item_size);
    queue->en_index = 0;
    queue->de_index = 0;
    queue->count = 0;
    queue->is_static = true;

    return queue;
}

/// @brief 数据入队
bool lyqueue_send(LyQueue_t *queue, void *item)
{
    LYDSS_ASSERT(queue);
    LYDSS_ASSERT(item);

    if(queue->count >= queue->store.length) {
        return false;
    }

    memcpy(lystore_address(&queue->store, queue->en_index), item, queue->store.item_size);
    queue->en_index = lyring_next(queue->en_index, queue->store.length);
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

    memcpy(buf, lystore_address(&queue->store, queue->de_index), queue->store.item_size);
    queue->de_index = lyring_next(queue->de_index, queue->store.length);
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

    memcpy(buf, lystore_address(&queue->store, queue->de_index), queue->store.item_size);

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
void lyqueue_delete(LyQueue_t *queue)
{
    LYDSS_ASSERT(queue);

    if(queue->is_static == true) {
        lystore_unbind(&queue->store);
        queue->en_index = 0;
        queue->de_index = 0;
        queue->count = 0;
        return;
    }

    lystore_free(&queue->store);
    free(queue);
}

/// @brief 获取队列中当前的成员个数
size_t lyqueue_count(LyQueue_t *queue)
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

    return queue->count >= queue->store.length;
}

#endif /* LYDSS_QUEUE_ENABLED */
