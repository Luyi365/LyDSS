/**
 * @file lyvector.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief 自持成员区的动态数组
 * @date 2026-09-03
 *
 * @copyright Copyright (c) 2026 Luyi365
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @note 容量按倍增而非按需加一，使追加摊还 O(1)。增长依赖 realloc，故不提供静态构造。
 */

#include "../inc/lydss_api.h"

#if LYDSS_VECTOR_ENABLED

#include "../inc/lydss_priv.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define LYVECTOR_GROWTH_FACTOR 2    ///< 容量增长倍数

struct LyVector {
    LyStore_t store;            ///< 成员区描述，其中 store.length 即容量
    size_t size;                ///< 当前成员数量
};

static bool vector_reserve_capacity(LyVector_t *vector, size_t need);
static bool vector_resize_buffer(LyVector_t *vector, size_t new_len);

/// @brief 创建一个动态数组
LyVector_t *lyvector_create(size_t len, size_t item_size)
{
    LYDSS_ASSERT(len);
    LYDSS_ASSERT(item_size);

    LyVector_t *vector = malloc(sizeof(LyVector_t));
    if(vector == NULL) {
        return NULL;
    }

    if(lystore_alloc(&vector->store, len, item_size, false) == false) {
        free(vector);
        return NULL;
    }

    vector->size = 0;

    return vector;
}

/// @brief 预留容量，只增不减
bool lyvector_reserve(LyVector_t *vector, size_t len)
{
    LYDSS_ASSERT(vector);

    if(len <= vector->store.length) {
        return true;
    }

    return vector_resize_buffer(vector, len);
}

/// @brief 在尾部追加成员
/// @note 可能触发重分配，之前 lyvector_get 取得的地址随之失效
bool lyvector_push_rear(LyVector_t *vector, const void *item)
{
    LYDSS_ASSERT(vector);
    LYDSS_ASSERT(item);

    if(vector_reserve_capacity(vector, vector->size + 1) == false) {
        return false;
    }

    memcpy(lystore_address(&vector->store, vector->size), item, vector->store.item_size);
    vector->size++;

    return true;
}

/// @brief 写入指定下标的成员，边界对 size 判定，不能写进空闲容量
bool lyvector_set(LyVector_t *vector, size_t index, const void *item)
{
    LYDSS_ASSERT(vector);
    LYDSS_ASSERT(item);

    if(index >= vector->size) {
        return false;
    }

    memcpy(lystore_address(&vector->store, index), item, vector->store.item_size);

    return true;
}

/// @brief 获取指定下标成员的地址
/// @note 任何触发重分配的操作之后该地址失效
void *lyvector_get(LyVector_t *vector, size_t index)
{
    LYDSS_ASSERT(vector);

    if(index >= vector->size) {
        return NULL;
    }

    return lystore_address(&vector->store, index);
}

/// @brief 读取指定下标的成员到缓冲区
bool lyvector_read(LyVector_t *vector, size_t index, void *buf)
{
    LYDSS_ASSERT(vector);
    LYDSS_ASSERT(buf);

    if(index >= vector->size) {
        return false;
    }

    memcpy(buf, lystore_address(&vector->store, index), vector->store.item_size);

    return true;
}

/// @brief 取出尾部的成员，不收缩容量
bool lyvector_pop_rear(LyVector_t *vector, void *buf)
{
    LYDSS_ASSERT(vector);

    if(vector->size == 0) {
        return false;
    }

    vector->size--;
    if(buf != NULL) {
        memcpy(buf, lystore_address(&vector->store, vector->size), vector->store.item_size);
    }

    return true;
}

/// @brief 清空成员，容量保持不变
bool lyvector_clear(LyVector_t *vector)
{
    LYDSS_ASSERT(vector);

    vector->size = 0;

    return true;
}

/// @brief 将容量收缩到当前成员数量，空 vector 保留一个槽位
bool lyvector_shrink(LyVector_t *vector)
{
    LYDSS_ASSERT(vector);

    size_t new_len = vector->size > 0 ? vector->size : 1;
    if(new_len >= vector->store.length) {
        return true;
    }

    return vector_resize_buffer(vector, new_len);
}

/// @brief 删除动态数组
void lyvector_delete(LyVector_t *vector)
{
    LYDSS_ASSERT(vector);

    lystore_free(&vector->store);
    free(vector);
}

/// @brief 获取当前成员数量
size_t lyvector_size(LyVector_t *vector)
{
    LYDSS_ASSERT(vector);

    return vector->size;
}

/// @brief 获取当前容量
size_t lyvector_capacity(LyVector_t *vector)
{
    LYDSS_ASSERT(vector);

    return vector->store.length;
}

/// @brief 判断是否为空
bool lyvector_is_empty(LyVector_t *vector)
{
    LYDSS_ASSERT(vector);

    return vector->size == 0;
}

/* --------------------------------- static --------------------------------- */
/// @brief 确保容量至少能容纳 need 个成员，不足则按倍数增长
static bool vector_reserve_capacity(LyVector_t *vector, size_t need)
{
    if(need <= vector->store.length) {
        return true;
    }

    size_t new_len = vector->store.length;
    while(new_len < need) {
        if(new_len > SIZE_MAX / LYVECTOR_GROWTH_FACTOR) {
            new_len = need;     // 继续倍增会溢出，退化为按需分配
            break;
        }
        new_len *= LYVECTOR_GROWTH_FACTOR;
    }

    return vector_resize_buffer(vector, new_len);
}

/// @brief 重新分配成员区
static bool vector_resize_buffer(LyVector_t *vector, size_t new_len)
{
    LYDSS_ASSERT(new_len >= vector->size);

    return lystore_realloc(&vector->store, new_len);
}

#endif /* LYDSS_VECTOR_ENABLED */
