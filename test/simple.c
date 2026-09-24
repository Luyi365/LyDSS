/**
 * @file simple.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief 简单用例，随时更改
 * @date 2025-07-16
 *
 * @copyright Copyright (c) 2025
 */

#include "../inc/lydss_api.h"

#include <stdio.h>
#include <string.h>

int simple()
{
    LyStream_t *stream = lystream_create(16);

    cell_t text[] = "LyDSS stream";
    size_t len = sizeof(text) - 1;               /* 不写入结尾的 '\0' */

    printf("in    = %u\n", (unsigned)lystream_in(stream, text, len));    // 12
    printf("count = %u\n", (unsigned)lystream_count(stream));            // 12

    /* 流的特征：写入的是一段连续数据，取出时可按需分段，不必整存整取；
       余量不足时只给出剩下的部分，由返回值告知实际取到多少 */
    cell_t buf[8] = { 0 };
    for(int seg = 1; lystream_count(stream) > 0; seg++) {
        size_t got = lystream_out(stream, buf, 5);
        printf("seg%d  = %u : %.*s\n", seg, (unsigned)got, (int)got, (char *)buf);
    }
    printf("count = %u\n", (unsigned)lystream_count(stream));            // 0

    lystream_delete(stream);

    return 0;
}
