/**
 * @file lystream.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief 定长环形字节流
 * @date 2026-08-14
 *
 * @copyright Copyright (c) 2026
 *
 * @note in/out 返回实际传输的单元数，可能少于请求量，调用者须按返回值推进。
 */

#include "../inc/lydss_api.h"

#if LYDSS_STREAM_ENABLED

#include "../inc/lydss_priv.h"

#include <stdlib.h>
#include <string.h>

struct LyStream {
    size_t head;         ///< 读游标
    size_t tail;         ///< 写游标
    size_t count;        ///< 流中当前的单元个数
    size_t size;         ///< 最大容量
#if LYDSS_STREAM_STATIC_ENABLED
    bool is_static;      ///< 缓冲区是否由调用者提供
#endif
    cell_t *buffer;      ///< 缓冲区
};

#if LYDSS_STREAM_STATIC_ENABLED
_Static_assert(sizeof(struct LyStream) <= sizeof(LyStreamStatic_t)
               && _Alignof(struct LyStream) <= _Alignof(LyStreamStatic_t),
               "LyStreamStatic_t is too small or under-aligned for struct LyStream");
#endif

/// @brief 创建流
LyStream_t *lystream_create(size_t size)
{
    LYDSS_ASSERT(size);

    size_t bytes = 0;
    if(lystore_bytes(size, sizeof(cell_t), &bytes) == false
        || bytes > SIZE_MAX - sizeof(LyStream_t)) {
        return NULL;        // 字节数会溢出
    }

    LyStream_t *new_stream = calloc(1, sizeof(LyStream_t) + bytes);
    if(new_stream == NULL) {
        return NULL;
    }

    new_stream->head = 0;
    new_stream->tail = 0;
    new_stream->count = 0;
    new_stream->size = size;
#if LYDSS_STREAM_STATIC_ENABLED
    new_stream->is_static = false;
#endif
    new_stream->buffer = (cell_t *)(new_stream + 1);

    return new_stream;
}

#if LYDSS_STREAM_STATIC_ENABLED
/// @brief 创建一个静态流
LyStream_t *lystream_create_static(LyStreamStatic_t *storage, void *buf, size_t size)
{
    LYDSS_ASSERT(storage);
    LYDSS_ASSERT(buf);
    LYDSS_ASSERT(size);

    LyStream_t *stream = (LyStream_t *)storage;
    stream->head = 0;
    stream->tail = 0;
    stream->count = 0;
    stream->size = size;
    stream->is_static = true;
    stream->buffer = buf;

    return stream;
}
#endif

/// @brief 数据流入，返回实际写入的单元数
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
        stream->tail = lyring_next(stream->tail, stream->size);
    }
    stream->count += size;

    return size;
}

/// @brief 数据流出，返回实际读出的单元数
size_t lystream_out(LyStream_t *stream, cell_t *buf, size_t size)
{
    LYDSS_ASSERT(stream);
    LYDSS_ASSERT(buf);

    if(size > stream->count) {
        size = stream->count;
    }

    for(size_t i = 0; i < size; i++) {
        buf[i] = stream->buffer[stream->head];
        stream->head = lyring_next(stream->head, stream->size);
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
void lystream_delete(LyStream_t *stream)
{
    LYDSS_ASSERT(stream);

#if LYDSS_STREAM_STATIC_ENABLED
    if(stream->is_static == true) {
        stream->buffer = NULL;
        stream->head = 0;
        stream->tail = 0;
        stream->count = 0;
        return;
    }
#endif

    free(stream);
}

#endif /* LYDSS_STREAM_ENABLED */
