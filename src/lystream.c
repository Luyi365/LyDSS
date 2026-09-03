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
#include "../inc/lydds_api.h"

struct LyStream {
    size_t head;  /* 读游标 */
    size_t tail;  /* 写游标 */
    size_t size;         ///< 最大容量
    cell_t *buffer;      ///< 缓冲区
};

LyStream_t *lystream_create(size_t size)
{
    LyStream_t *new = calloc(1, sizeof(LyStream_t) + size * sizeof(cell_t));
    new->size = size;
    new->buffer = (cell_t *)(new + 1);
    new->head = new->tail = new->buffer;

    return new;
}

void lystream_create_static(LyStream_t *stream, void *buf, size_t size)
{
    stream->head = stream->tail = buf;
    stream->size = size;
    stream->buffer = buf;
}

/// @brief 数据流入
/// @note 这里的size是以cell_t为单位的
size_t lystream_in(LyStream_t *stream, cell_t *data, size_t size)
{
    if(stream->size < size) {
        size = stream->size;
    }

    memcpy(stream->buffer, data, sizeof(cell_t) * size);

    return size;
}

/// @brief 数据流出
size_t lystream_out(LyStream_t *stream, cell_t *buf, size_t size)
{
    if(stream->size < size) {
        size = stream->size;
    }

    memcpy(buf, stream->buffer, sizeof(cell_t) * size);

    return size;
}

size_t lystream_count(LyStream_t *stream)
{
    return (stream->tail - stream->head) / sizeof(cell_t);
}

size_t lystream_clean(LyStream_t *stream)
{
    memset(stream->buffer, 0, sizeof(cell_t) * stream->size);

    return stream->size;
}

void lystream_delete(LyStream_t *stream)
{
    free(stream);
}
