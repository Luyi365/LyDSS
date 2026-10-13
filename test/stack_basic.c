/**
 * @file stack_basic.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief 栈用例
 * @date 2026-08-18
 *
 * @copyright Copyright (c) 2026
 */

#include "../inc/lydss_api.h"

#if LYDSS_STACK_ENABLED

#include <stdio.h>

struct Record {
    int id;
    char tag[5];
};
typedef struct Record Record_t;

int stack_basic(void)
{
    /* 静态构造，全程不调用 malloc */
    static LyStackStatic_t storage;
    static int buffer[8];
    LyStack_t *stack = lystack_create_static(&storage, buffer, 8, sizeof(int));

    int a = 10, b = 20, c = 30, val = 0;
    lystack_push(stack, &a);
    lystack_push(stack, &b);
    lystack_push(stack, &c);
    printf("静态构造容量 8 的栈，依次入栈 %d %d %d，当前 %zu 个成员\n",
        a, b, c, lystack_count(stack));

    /* 栈的特征：后进先出，取出顺序与写入顺序相反 */
    printf("依次出栈");
    while(lystack_is_empty(stack) == false) {
        lystack_pop(stack, &val);
        printf(" %d", val);
    }
    printf("\n");
    lystack_delete(stack);

    /* 成员大小由 item_size 决定，同一套接口可承载任意定长类型 */
    LyStack_t *srec = lystack_create(4, sizeof(Record_t));
    Record_t rin = { 7, "rec" };
    Record_t rout = { 0, { 0 } };
    lystack_push(srec, &rin);
    lystack_pop(srec, &rout);
    printf("结构体 {%d, \"%s\"} 入栈再出栈：{%d, \"%s\"}\n", rin.id, rin.tag, rout.id, rout.tag);
    lystack_delete(srec);

    LyStack_t *sd = lystack_create(4, sizeof(double));
    double din = -1.25, dout = 0;
    lystack_push(sd, &din);
    lystack_pop(sd, &dout);
    printf("double %.2f 入栈再出栈：%.2f\n", din, dout);
    lystack_delete(sd);

    return 0;
}

#endif /* LYDSS_STACK_ENABLED */
