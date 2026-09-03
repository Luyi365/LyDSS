/**
 * @file lystream.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief
 * @date 2026-08-14
 *
 * @copyright Copyright (c) 2026
 *
 * @note
 * Matters related to this document.
 */

#include <stdlib.h>
#include <string.h>
#include "../inc/lydss_api.h"

struct LyStream {
    size_t head;         ///< 读游标
    size_t tail;         ///< 写游标
    size_t count;        ///< 流中当前的单元个数
    size_t size;         ///< 最大容量
    bool is_static;      ///< 缓冲区是否由调用者提供
    cell_t *buffer;      ///< 缓冲区
};

/// @brief 创建流
LyStream_t *lystream_create(size_t size)
{
    LYDSS_ASSERT(size);

    LyStream_t *new_stream = calloc(1, sizeof(LyStream_t) + size * sizeof(cell_t));
    if(new_stream == NULL) {
        return NULL;
    }

    new_stream->head = 0;
    new_stream->tail = 0;
    new_stream->count = 0;
    new_stream->size = size;
    new_stream->is_static = false;
    new_stream->buffer = (cell_t *)(new_stream + 1);

    return new_stream;
}

/// @brief 创建一个静态流
void lystream_create_static(LyStream_t *stream, void *buf, size_t size)
{
    LYDSS_ASSERT(stream);
    LYDSS_ASSERT(buf);
    LYDSS_ASSERT(size);

    stream->head = 0;
    stream->tail = 0;
    stream->count = 0;
    stream->size = size;
    stream->is_static = true;
    stream->buffer = buf;
}

/// @brief 数据流入
/// @note 这里的size是以cell_t为单位的；缓冲区剩余空间不足时只写入能容纳的部分
size_t lystream_in(LyStream_t *stream, cell_t *data, size_t size)
{
    LYDSS_ASSERT(stream);
    LYDSS_ASSERT(data);

    size_t vacancy = stream->size - stream->count;
    if(size > vacancy) {
        size = vacancy;
    }

    for(size_t i = 0; i < size; i++) {
        stream->buffer[stream->tail] = data[i];
        stream->tail = (stream->tail + 1) % stream->size;
    }
    stream->count += size;

    return size;
}

/// @brief 数据流出
/// @note 流中数据不足时只读出已有的部分
size_t lystream_out(LyStream_t *stream, cell_t *buf, size_t size)
{
    LYDSS_ASSERT(stream);
    LYDSS_ASSERT(buf);

    if(size > stream->count) {
        size = stream->count;
    }

    for(size_t i = 0; i < size; i++) {
        buf[i] = stream->buffer[stream->head];
        stream->head = (stream->head + 1) % stream->size;
    }
    stream->count -= size;

    return size;
}

/// @brief 获取流中当前的数据个数
size_t lystream_count(LyStream_t *stream)
{
    LYDSS_ASSERT(stream);

    return stream->count;
}

/// @brief 清空流
size_t lystream_clean(LyStream_t *stream)
{
    LYDSS_ASSERT(stream);

    size_t count = stream->count;

    memset(stream->buffer, 0, sizeof(cell_t) * stream->size);
    stream->head = 0;
    stream->tail = 0;
    stream->count = 0;

    return count;
}

/// @brief 删除流
/// @note 静态流只解除缓冲区绑定，不释放调用者提供的内存与句柄
void lystream_delete(LyStream_t *stream)
{
    LYDSS_ASSERT(stream);

    if(stream->is_static == true) {
        stream->buffer = NULL;
        stream->head = 0;
        stream->tail = 0;
        stream->count = 0;
        return;
    }

    free(stream);
}
