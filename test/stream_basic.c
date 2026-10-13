/**
 * @file stream_basic.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief 流用例
 * @date 2026-08-23
 *
 * @copyright Copyright (c) 2026
 */

#include "../inc/lydss_api.h"

#if LYDSS_STREAM_ENABLED

#include <stdio.h>

int stream_basic(void)
{
    LyStream_t *stream = lystream_create(16);

    cell_t text[] = "LyDSS stream";
    size_t len = sizeof(text) - 1;               /* 不写入结尾的 '\0' */
    size_t in = lystream_in(stream, text, len);
    printf("容量 16 字节的流写入 \"%s\"（%zu 字节）：实际写入 %zu 字节\n", (char *)text, len, in);

    /* 流的特征：写入的是一段连续数据，取出时可按需分段，不必整存整取；
       余量不足时只给出剩下的部分，由返回值告知实际取到多少 */
    cell_t buf[8] = { 0 };
    for(int seg = 1; lystream_count(stream) > 0; seg++) {
        size_t got = lystream_out(stream, buf, 5);
        printf("第 %d 次最多取 5 字节：取到 %zu 字节 \"%.*s\"\n", seg, got, (int)got, (char *)buf);
    }
    printf("取完后流中剩余 %zu 字节\n", lystream_count(stream));

    lystream_delete(stream);

    return 0;
}

#endif /* LYDSS_STREAM_ENABLED */
