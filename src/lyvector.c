/**
 * @file lyvector.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief
 * @date 2026-09-03
 *
 * @copyright Copyright (c) 2026
 *
 * @note
 * 自持成员区的变长数组。size 为已用数量，capacity 为成员区可容纳的数量，
 * 两者分离是变长的前提：容量按倍数预支，已用数量随追加逐个增长。
 *
 * 容量不足时按 LYVECTOR_GROWTH_FACTOR 倍增而非按需加一，
 * 使连续 n 次追加的搬移总代价为 O(n)，单次追加的摊还代价为 O(1)。
 *
 * 空闲容量不做初始化，因索引边界对 size 判定，空闲槽位对外不可见。
 * 重分配可能迁移成员区，此前由 lyvector_get 取得的地址随之失效。
 * 因增长依赖重分配，不提供静态构造。
 */

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limits.h>
#include "../inc/lydss_api.h"

#define LYVECTOR_GROWTH_FACTOR 2    ///< 容量增长倍数

struct LyVector {
    void *buffer;               ///< 成员区
    unsigned long capacity;     ///< 成员区可容纳的成员数量
    unsigned long item_size;    ///< 单个成员的大小（单位：字节）
    unsigned long size;         ///< 当前成员数量
};

static bool vector_reserve_capacity(LyVector_t *vector, unsigned long need);
static bool vector_resize_buffer(LyVector_t *vector, unsigned long new_len);
static void *vector_locate_item(LyVector_t *vector, unsigned long index);

/// @brief 创建一个变长数组
LyVector_t *lyvector_create(unsigned long len, unsigned long item_size)
{
    LYDSS_ASSERT(len);
    LYDSS_ASSERT(item_size);

    if(len > ULONG_MAX / item_size) {
        return NULL;
    }

    LyVector_t *vector = malloc(sizeof(LyVector_t));
    if(vector == NULL) {
        return NULL;
    }

    vector->buffer = malloc(len * item_size);
    if(vector->buffer == NULL) {
        free(vector);
        return NULL;
    }

    vector->capacity = len;
    vector->item_size = item_size;
    vector->size = 0;

    return vector;
}

/// @brief 预留容量
/// @note 只增不减；预留足够后的追加不会再触发重分配
bool lyvector_reserve(LyVector_t *vector, unsigned long len)
{
    LYDSS_ASSERT(vector);

    if(len <= vector->capacity) {
        return true;
    }

    return vector_resize_buffer(vector, len);
}

/// @brief 在尾部追加成员
/// @note 容量不足时触发重分配，此前取得的成员地址随之失效
bool lyvector_push_rear(LyVector_t *vector, const void *item)
{
    LYDSS_ASSERT(vector);
    LYDSS_ASSERT(item);

    if(vector_reserve_capacity(vector, vector->size + 1) == false) {
        return false;
    }

    memcpy(vector_locate_item(vector, vector->size), item, vector->item_size);
    vector->size++;

    return true;
}

/// @brief 写入指定下标的成员
/// @note 边界对 size 判定，不能越过尾部写进空闲容量
bool lyvector_set(LyVector_t *vector, unsigned long index, const void *item)
{
    LYDSS_ASSERT(vector);
    LYDSS_ASSERT(item);

    if(index >= vector->size) {
        return false;
    }

    memcpy(vector_locate_item(vector, index), item, vector->item_size);

    return true;
}

/// @brief 获取指定下标成员的地址
/// @note 任何触发重分配的操作之后该地址失效
void *lyvector_get(LyVector_t *vector, unsigned long index)
{
    LYDSS_ASSERT(vector);

    if(index >= vector->size) {
        return NULL;
    }

    return vector_locate_item(vector, index);
}

/// @brief 读取指定下标的成员到缓冲区
bool lyvector_read(LyVector_t *vector, unsigned long index, void *buf)
{
    LYDSS_ASSERT(vector);
    LYDSS_ASSERT(buf);

    if(index >= vector->size) {
        return false;
    }

    memcpy(buf, vector_locate_item(vector, index), vector->item_size);

    return true;
}

/// @brief 取出尾部的成员
/// @note 只减少 size，不收缩容量；释放容量需显式调用 lyvector_shrink
bool lyvector_pop_rear(LyVector_t *vector, void *buf)
{
    LYDSS_ASSERT(vector);

    if(vector->size == 0) {
        return false;
    }

    vector->size--;
    if(buf != NULL) {
        memcpy(buf, vector_locate_item(vector, vector->size), vector->item_size);
    }

    return true;
}

/// @brief 清空成员
/// @note 只把 size 归零，容量保持不变
bool lyvector_clear(LyVector_t *vector)
{
    LYDSS_ASSERT(vector);

    vector->size = 0;

    return true;
}

/// @brief 将容量收缩到当前成员数量
/// @note 空 vector 保留一个槽位，避免出现零长度的成员区
bool lyvector_shrink(LyVector_t *vector)
{
    LYDSS_ASSERT(vector);

    unsigned long new_len = vector->size > 0 ? vector->size : 1;
    if(new_len >= vector->capacity) {
        return true;
    }

    return vector_resize_buffer(vector, new_len);
}

/// @brief 删除变长数组
void lyvector_delete(LyVector_t *vector)
{
    LYDSS_ASSERT(vector);

    free(vector->buffer);
    free(vector);
}

/// @brief 获取当前成员数量
unsigned long lyvector_size(LyVector_t *vector)
{
    LYDSS_ASSERT(vector);

    return vector->size;
}

/// @brief 获取当前容量
unsigned long lyvector_capacity(LyVector_t *vector)
{
    LYDSS_ASSERT(vector);

    return vector->capacity;
}

/// @brief 判断是否为空
bool lyvector_is_empty(LyVector_t *vector)
{
    LYDSS_ASSERT(vector);

    return vector->size == 0;
}

/* --------------------------------- static --------------------------------- */
/// @brief 确保容量至少能容纳 need 个成员，不足则按倍数增长
static bool vector_reserve_capacity(LyVector_t *vector, unsigned long need)
{
    if(need <= vector->capacity) {
        return true;
    }

    unsigned long new_len = vector->capacity;
    while(new_len < need) {
        if(new_len > ULONG_MAX / LYVECTOR_GROWTH_FACTOR) {
            new_len = need;     // 继续倍增会溢出，退化为按需分配
            break;
        }
        new_len *= LYVECTOR_GROWTH_FACTOR;
    }

    return vector_resize_buffer(vector, new_len);
}

/// @brief 重新分配成员区
/// @note 分配失败时原成员区与容量保持不变
static bool vector_resize_buffer(LyVector_t *vector, unsigned long new_len)
{
    LYDSS_ASSERT(new_len >= vector->size);

    if(new_len > ULONG_MAX / vector->item_size) {
        return false;       // 字节数会溢出
    }

    void *new_buffer = realloc(vector->buffer, new_len * vector->item_size);
    if(new_buffer == NULL) {
        return false;
    }

    vector->buffer = new_buffer;
    vector->capacity = new_len;

    return true;
}

/// @brief 计算指定下标成员的地址
static void *vector_locate_item(LyVector_t *vector, unsigned long index)
{
    return (unsigned char *)vector->buffer + index * vector->item_size;
}
