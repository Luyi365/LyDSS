/**
 * @file lyarray.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief
 * @date 2026-09-03
 *
 * @copyright Copyright (c) 2026
 *
 * @note
 * 为 C 原生数组补齐运行时元数据（长度与成员大小）的定长存储原语。
 * 全部 length 个槽位自始存在，索引边界对 length 判定，不维护已用数量。
 * 「已用多少」属于上层容器的不变式，由 vector、queue、stack 各自表达。
 */

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "../inc/lydss_api.h"

struct LyArray {
    void *buffer;               ///< 成员区
    unsigned long length;       ///< 成员区可容纳的成员数量
    unsigned long item_size;    ///< 单个成员的大小（单位：字节）
    bool is_static;             ///< 成员区是否由调用者提供
};

static void *array_locate_item(LyArray_t *array, unsigned long index);

/// @brief 创建一个数组
/// @note 成员区清零后交付
LyArray_t *lyarray_create(unsigned long len, unsigned long item_size)
{
    LYDSS_ASSERT(len);
    LYDSS_ASSERT(item_size);

    LyArray_t *array = malloc(sizeof(LyArray_t));
    if(array == NULL) {
        return NULL;
    }

    array->buffer = calloc(len, item_size);
    if(array->buffer == NULL) {
        free(array);
        return NULL;
    }

    array->length = len;
    array->item_size = item_size;
    array->is_static = false;

    return array;
}

/// @brief 创建一个静态数组
/// @note 不改动 buf 的既有内容，槽位初值由调用者负责
void lyarray_create_static(LyArray_t *array, void *buf, unsigned long len, unsigned long item_size)
{
    LYDSS_ASSERT(array);
    LYDSS_ASSERT(buf);
    LYDSS_ASSERT(len);
    LYDSS_ASSERT(item_size);

    array->buffer = buf;
    array->length = len;
    array->item_size = item_size;
    array->is_static = true;
}

/// @brief 写入指定下标的成员
bool lyarray_set(LyArray_t *array, unsigned long index, const void *item)
{
    LYDSS_ASSERT(array);
    LYDSS_ASSERT(item);

    if(index >= array->length) {
        return false;
    }

    memcpy(array_locate_item(array, index), item, array->item_size);

    return true;
}

/// @brief 获取指定下标成员的地址
void *lyarray_get(LyArray_t *array, unsigned long index)
{
    LYDSS_ASSERT(array);

    if(index >= array->length) {
        return NULL;
    }

    return array_locate_item(array, index);
}

/// @brief 读取指定下标的成员到缓冲区
bool lyarray_read(LyArray_t *array, unsigned long index, void *buf)
{
    LYDSS_ASSERT(array);
    LYDSS_ASSERT(buf);

    if(index >= array->length) {
        return false;
    }

    memcpy(buf, array_locate_item(array, index), array->item_size);

    return true;
}

/// @brief 删除数组
/// @note 静态数组只解除成员区绑定，不释放调用者提供的内存与句柄
void lyarray_delete(LyArray_t *array)
{
    LYDSS_ASSERT(array);

    if(array->is_static == true) {
        array->buffer = NULL;
        array->length = 0;
        return;
    }

    free(array->buffer);
    free(array);
}

/// @brief 获取数组可容纳的成员数量
unsigned long lyarray_length(LyArray_t *array)
{
    LYDSS_ASSERT(array);

    return array->length;
}

/// @brief 获取单个成员的大小
unsigned long lyarray_item_size(LyArray_t *array)
{
    LYDSS_ASSERT(array);

    return array->item_size;
}

/* --------------------------------- static --------------------------------- */
/// @brief 计算指定下标成员的地址
static void *array_locate_item(LyArray_t *array, unsigned long index)
{
    return (unsigned char *)array->buffer + index * array->item_size;
}
