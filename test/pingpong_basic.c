/**
 * @file pingpong_basic.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief 乒乓缓冲用例
 * @date 2026-09-16
 *
 * @copyright Copyright (c) 2026
 */

#include "../inc/lydss_api.h"

#if LYDSS_PINGPONG_ENABLED

#include <stdio.h>

int pingpong_basic(void)
{
    LyPingPong_t *pingpong = lypingpong_create(4, sizeof(int));
    printf("创建单块容量 %zu 的乒乓缓冲\n", lypingpong_length(pingpong));

    /* 第一轮：逐个写满填充块 */
    printf("依次写入");
    for(int i = 1; i <= 4; i++) {
        lypingpong_write(pingpong, &i);
        printf(" %d", i);
    }
    printf("，填充块%s\n", lypingpong_is_full(pingpong) ? "已满" : "未满");

    int extra = 9;
    bool written = lypingpong_write(pingpong, &extra);
    printf("再写入 %d：%s\n", extra, written ? "成功" : "失败，填充块已满");

    /* 消费方取整块基址，一次处理完 */
    bool swapped = lypingpong_swap(pingpong);
    const int *batch = lypingpong_ready_data(pingpong);
    printf("交换：%s，就绪块", swapped ? "成功" : "失败");
    for(size_t i = 0; i < lypingpong_ready_count(pingpong); i++) {
        printf(" %d", batch[i]);
    }
    printf("\n");

    /* 乒乓的核心：消费就绪块的同时，生产方已经在往另一块写 */
    int x = 7;
    lypingpong_write(pingpong, &x);
    printf("消费期间向填充块写入 %d\n", x);

    /* 就绪块尚未释放，此时交换会覆盖未消费的数据，因此被拒 */
    swapped = lypingpong_swap(pingpong);
    printf("未释放就绪块就交换：%s\n", swapped ? "成功" : "被拒绝");

    lypingpong_release(pingpong);
    swapped = lypingpong_swap(pingpong);
    batch = lypingpong_ready_data(pingpong);
    printf("释放后交换：%s，就绪块 %d\n", swapped ? "成功" : "失败", batch[0]);
    lypingpong_release(pingpong);

    /* 第二轮改走零拷贝：外部写入方直接写基址，再报告实际写入量 */
    int *raw = lypingpong_fill_data(pingpong);
    raw[0] = 10;
    raw[1] = 20;
    raw[2] = 30;
    lypingpong_commit(pingpong, 3);
    printf("外部直接写入 10 20 30，提交数量 3\n");

    lypingpong_swap(pingpong);
    batch = lypingpong_ready_data(pingpong);
    printf("交换后就绪块 %zu 个成员：%d %d %d\n",
        lypingpong_ready_count(pingpong), batch[0], batch[1], batch[2]);

    lypingpong_release(pingpong);
    lypingpong_delete(pingpong);

    return 0;
}

#endif /* LYDSS_PINGPONG_ENABLED */
