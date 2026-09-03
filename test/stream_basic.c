/**
 * @file stream_basic.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief 
 * @date 2026-08-23
 * 
 * @copyright Copyright (c) 2026
 * 
 * @note 
 * Matters related to this document.
 */

#include <stdio.h>
#include "../inc/lydss_api.h"

int stream_basic()
{
    LyStream_t *stream = lystream_create(8);

    cell_t in[5] = { 1, 2, 3, 4, 5 };
    cell_t out[8] = { 0 };

    printf("in     = %u\n", (unsigned)lystream_in(stream, in, 5));      // 5
    printf("count  = %u\n", (unsigned)lystream_count(stream));          // 5

    printf("out    = %u : ", (unsigned)lystream_out(stream, out, 3));   // 3
    for(int i = 0; i < 3; i++) {
        printf("%u ", out[i]);                                          // 1 2 3
    }
    printf("\n");
    printf("count  = %u\n", (unsigned)lystream_count(stream));          // 2

    /* 写入 6 个单元触发游标回绕（容量 8，已用 2） */
    cell_t more[6] = { 6, 7, 8, 9, 10, 11 };
    printf("in     = %u\n", (unsigned)lystream_in(stream, more, 6));    // 6
    printf("full   = %u\n", (unsigned)lystream_in(stream, more, 1));    // 0，已满

    printf("out    = %u : ", (unsigned)lystream_out(stream, out, 8));   // 8
    for(int i = 0; i < 8; i++) {
        printf("%u ", out[i]);                            // 4 5 6 7 8 9 10 11
    }
    printf("\n");
    printf("empty  = %u\n", (unsigned)lystream_count(stream));          // 0

    lystream_in(stream, in, 5);
    printf("clean  = %u\n", (unsigned)lystream_clean(stream));          // 5
    printf("count  = %u\n", (unsigned)lystream_count(stream));          // 0

    lystream_delete(stream);

    return 0;
}
