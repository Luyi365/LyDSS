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

int stack_basic()
{
    LyStack_t *stack = lystack_create(8, sizeof(int));

    int a = 10, b = 20, c = 30, val = 0;
    lystack_push(stack, &a);
    lystack_push(stack, &b);
    lystack_push(stack, &c);
    printf("count  = %lu, full = %d\n",
        lystack_count(stack), lystack_is_full(stack));      // 3, 0

    /* 栈的特征：后进先出，取出顺序与写入顺序相反 */
    printf("pop    = ");
    while(lystack_is_empty(stack) == false) {
        lystack_pop(stack, &val);
        printf("%d ", val);                                 // 30 20 10
    }
    printf("\n");
    lystack_delete(stack);

    /* 成员大小由 item_size 决定，同一套接口可承载任意定长类型 */
    LyStack_t *srec = lystack_create(4, sizeof(Record_t));
    Record_t rin = { 7, "rec" };
    Record_t rout = { 0, { 0 } };
    lystack_push(srec, &rin);
    lystack_pop(srec, &rout);
    printf("struct = %d-%s\n", rout.id, rout.tag);           // 7-rec
    lystack_delete(srec);

    LyStack_t *sd = lystack_create(4, sizeof(double));
    double din = -1.25, dout = 0;
    lystack_push(sd, &din);
    lystack_pop(sd, &dout);
    printf("double = %.2f\n", dout);                         // -1.25
    lystack_delete(sd);

    return 0;
}

#endif /* LYDSS_STACK_ENABLED */
