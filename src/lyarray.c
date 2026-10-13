/**
 * @file lyarray.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief 为 C 原生数组补齐运行时元数据的定长存储原语
 * @date 2026-09-03
 *
 * @copyright Copyright (c) 2026
 *
 * @note 全部 length 个槽位自始存在，不维护已用数量；不要添加 push/pop 等序列操作。
 */

#include "../inc/lydss_api.h"

#if LYDSS_ARRAY_ENABLED

#include "../inc/lydss_priv.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

struct LyArray {
    LyStore_t store;            ///< 成员区描述（地址、长度、成员大小）
    bool is_static;             ///< 成员区是否由调用者提供
};

_Static_assert(sizeof(struct LyArray) <= sizeof(LyArrayStatic_t)
               && _Alignof(struct LyArray) <= _Alignof(LyArrayStatic_t),
               "LyArrayStatic_t is too small or under-aligned for struct LyArray");

/// @brief 创建一个数组
LyArray_t *lyarray_create(size_t len, size_t item_size)
{
    LYDSS_ASSERT(len);
    LYDSS_ASSERT(item_size);

    LyArray_t *array = malloc(sizeof(LyArray_t));
    if(array == NULL) {
        return NULL;
    }

    if(lystore_alloc(&array->store, len, item_size, true) == false) {
        free(array);
        return NULL;
    }

    array->is_static = false;

    return array;
}

/// @brief 创建一个静态数组，不清零 buf
LyArray_t *lyarray_create_static(LyArrayStatic_t *storage, void *buf, size_t len, size_t item_size)
{
    LYDSS_ASSERT(storage);
    LYDSS_ASSERT(buf);
    LYDSS_ASSERT(len);
    LYDSS_ASSERT(item_size);

    LyArray_t *array = (LyArray_t *)storage;
    lystore_bind(&array->store, buf, len, item_size);
    array->is_static = true;

    return array;
}

/// @brief 写入指定下标的成员
bool lyarray_set(LyArray_t *array, size_t index, const void *item)
{
    LYDSS_ASSERT(array);
    LYDSS_ASSERT(item);

    if(index >= array->store.length) {
        return false;
    }

    memcpy(lystore_address(&array->store, index), item, array->store.item_size);

    return true;
}

/// @brief 获取指定下标成员的地址
void *lyarray_get(LyArray_t *array, size_t index)
{
    LYDSS_ASSERT(array);

    if(index >= array->store.length) {
        return NULL;
    }

    return lystore_address(&array->store, index);
}

/// @brief 读取指定下标的成员到缓冲区
bool lyarray_read(LyArray_t *array, size_t index, void *buf)
{
    LYDSS_ASSERT(array);
    LYDSS_ASSERT(buf);

    if(index >= array->store.length) {
        return false;
    }

    memcpy(buf, lystore_address(&array->store, index), array->store.item_size);

    return true;
}

/// @brief 删除数组
void lyarray_delete(LyArray_t *array)
{
    LYDSS_ASSERT(array);

    if(array->is_static == true) {
        lystore_unbind(&array->store);
        return;
    }

    lystore_free(&array->store);
    free(array);
}

/// @brief 获取数组可容纳的成员数量
size_t lyarray_length(LyArray_t *array)
{
    LYDSS_ASSERT(array);

    return array->store.length;
}

/// @brief 获取单个成员的大小
size_t lyarray_item_size(LyArray_t *array)
{
    LYDSS_ASSERT(array);

    return array->store.item_size;
}

#endif /* LYDSS_ARRAY_ENABLED */
