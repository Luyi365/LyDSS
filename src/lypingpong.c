/**
 * @file lypingpong.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief 两块等长缓冲轮换的乒乓结构
 * @date 2026-09-16
 *
 * @copyright Copyright (c) 2026
 */

#include "../inc/lydss_api.h"

#if LYDSS_PINGPONG_ENABLED

#include "../inc/lydss_priv.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define LYPINGPONG_BUFFER_COUNT 2   ///< 乒乓结构固定为两块缓冲

struct LyPingPong {
    LyStore_t store[LYPINGPONG_BUFFER_COUNT];       ///< 两块等长成员区
    size_t count[LYPINGPONG_BUFFER_COUNT];          ///< 两块各自已写入的成员数量
    uint8_t fill;                                   ///< 当前填充块的索引
    bool is_static;                                 ///< 成员区是否由调用者提供
};

_Static_assert(sizeof(struct LyPingPong) <= sizeof(LyPingPongStatic_t)
               && _Alignof(struct LyPingPong) <= _Alignof(LyPingPongStatic_t),
               "LyPingPongStatic_t is too small or under-aligned for struct LyPingPong");

/// @brief 创建一个乒乓缓冲
LyPingPong_t *lypingpong_create(size_t len, size_t item_size)
{
    LYDSS_ASSERT(len);
    LYDSS_ASSERT(item_size);

    LyPingPong_t *pingpong = malloc(sizeof(LyPingPong_t));
    if(pingpong == NULL) {
        return NULL;
    }

    if(lystore_alloc(&pingpong->store[0], len, item_size, true) == false) {
        free(pingpong);
        return NULL;
    }

    if(lystore_alloc(&pingpong->store[1], len, item_size, true) == false) {
        lystore_free(&pingpong->store[0]);
        free(pingpong);
        return NULL;
    }

    pingpong->count[0] = 0;
    pingpong->count[1] = 0;
    pingpong->fill = 0;
    pingpong->is_static = false;

    return pingpong;
}

/// @brief 创建一个静态乒乓缓冲，不清零 buf_a 与 buf_b
LyPingPong_t *lypingpong_create_static(LyPingPongStatic_t *storage, void *buf_a, void *buf_b,
                                       size_t len, size_t item_size)
{
    LYDSS_ASSERT(storage);
    LYDSS_ASSERT(buf_a);
    LYDSS_ASSERT(buf_b);
    LYDSS_ASSERT(len);
    LYDSS_ASSERT(item_size);

    LyPingPong_t *pingpong = (LyPingPong_t *)storage;
    lystore_bind(&pingpong->store[0], buf_a, len, item_size);
    lystore_bind(&pingpong->store[1], buf_b, len, item_size);
    pingpong->count[0] = 0;
    pingpong->count[1] = 0;
    pingpong->fill = 0;
    pingpong->is_static = true;

    return pingpong;
}

/// @brief 向填充块写入一个成员
bool lypingpong_write(LyPingPong_t *pingpong, const void *item)
{
    LYDSS_ASSERT(pingpong);
    LYDSS_ASSERT(item);

    LyStore_t *store = &pingpong->store[pingpong->fill];
    size_t count = pingpong->count[pingpong->fill];
    if(count >= store->length) {
        return false;
    }

    memcpy(lystore_address(store, count), item, store->item_size);
    pingpong->count[pingpong->fill]++;

    return true;
}

/// @brief 取得填充块的基址
void *lypingpong_fill_data(LyPingPong_t *pingpong)
{
    LYDSS_ASSERT(pingpong);

    return pingpong->store[pingpong->fill].buffer;
}

/// @brief 以绝对值设置填充块的已写入成员数量
bool lypingpong_commit(LyPingPong_t *pingpong, size_t count)
{
    LYDSS_ASSERT(pingpong);

    if(count > pingpong->store[pingpong->fill].length) {
        return false;
    }

    pingpong->count[pingpong->fill] = count;

    return true;
}

/// @brief 判断填充块是否已满
bool lypingpong_is_full(LyPingPong_t *pingpong)
{
    LYDSS_ASSERT(pingpong);

    return pingpong->count[pingpong->fill] >= pingpong->store[pingpong->fill].length;
}

/// @brief 交换两块缓冲的角色
bool lypingpong_swap(LyPingPong_t *pingpong)
{
    LYDSS_ASSERT(pingpong);

    uint8_t ready = 1 - pingpong->fill;

    /* 就绪块未释放就交换，新一轮写入会覆盖尚未消费的数据 */
    if(pingpong->count[ready] > 0) {
        return false;
    }

    pingpong->fill = ready;

    return true;
}

/// @brief 取得就绪块的基址
void *lypingpong_ready_data(LyPingPong_t *pingpong)
{
    LYDSS_ASSERT(pingpong);

    return pingpong->store[1 - pingpong->fill].buffer;
}

/// @brief 取得就绪块的成员数量
size_t lypingpong_ready_count(LyPingPong_t *pingpong)
{
    LYDSS_ASSERT(pingpong);

    return pingpong->count[1 - pingpong->fill];
}

/// @brief 释放就绪块
void lypingpong_release(LyPingPong_t *pingpong)
{
    LYDSS_ASSERT(pingpong);

    pingpong->count[1 - pingpong->fill] = 0;
}

/// @brief 删除乒乓缓冲
void lypingpong_delete(LyPingPong_t *pingpong)
{
    LYDSS_ASSERT(pingpong);

    if(pingpong->is_static == true) {
        lystore_unbind(&pingpong->store[0]);
        lystore_unbind(&pingpong->store[1]);
        return;
    }

    lystore_free(&pingpong->store[0]);
    lystore_free(&pingpong->store[1]);
    free(pingpong);
}

/// @brief 获取单块可容纳的成员数量
size_t lypingpong_length(LyPingPong_t *pingpong)
{
    LYDSS_ASSERT(pingpong);

    return pingpong->store[0].length;
}

/// @brief 获取单个成员的大小
size_t lypingpong_item_size(LyPingPong_t *pingpong)
{
    LYDSS_ASSERT(pingpong);

    return pingpong->store[0].item_size;
}

#endif /* LYDSS_PINGPONG_ENABLED */
