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
#include <string.h>

int stream_basic(void)
{
    LyStream_t *stream = lystream_create(16);

    /* 逐字符转成 cell_t，单元类型换成 uint16_t、uint32_t 时同样适用 */
    const char *text = "LyDSS stream";
    size_t len = strlen(text);
    cell_t data[16];
    for(size_t i = 0; i < len; i++) {
        data[i] = (cell_t)text[i];
    }

    size_t in = lystream_in(stream, data, len);
    printf("容量 16 个单元（每单元 %zu 字节）的流写入 \"%s\"：实际写入 %zu 个单元\n",
        sizeof(cell_t), text, in);

    /* 流的特征：写入的是一段连续数据，取出时可按需分段，不必整存整取；
       余量不足时只给出剩下的部分，由返回值告知实际取到多少 */
    cell_t buf[8] = { 0 };
    for(int seg = 1; lystream_count(stream) > 0; seg++) {
        size_t got = lystream_out(stream, buf, 5);
        printf("第 %d 次最多取 5 个单元：取到 %zu 个 \"", seg, got);
        for(size_t i = 0; i < got; i++) {
            printf("%c", (char)buf[i]);
        }
        printf("\"\n");
    }
    printf("取完后流中剩余 %zu 个单元\n", lystream_count(stream));

    lystream_delete(stream);

    return 0;
}

#endif /* LYDSS_STREAM_ENABLED */
