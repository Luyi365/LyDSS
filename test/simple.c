/**
 * @file simple.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief 栈用例，由 LyDSS_SimpleTest 运行
 * @date 2025-07-16
 *
 * @copyright Copyright (c) 2025
 */

#include "../inc/lydss_api.h"

#if LYDSS_STACK_ENABLED

#include <stdio.h>
#include <string.h>

int simple()
{
    LyStack_t *stack = lystack_create(8, sizeof(int));
    int a = 10, b = 20, c = 30;
    lystack_push(stack, &a);
    lystack_push(stack, &b);
    lystack_push(stack, &c);
    printf("count = %lu\n", lystack_count(stack));      // 3
    printf("empty = %d\n", lystack_is_empty(stack));    // 0
    printf("full  = %d\n", lystack_is_full(stack));     // 0
    int val;
    while(!lystack_is_empty(stack)) {
        lystack_pop(stack, &val);
        printf("pop = %d\n", val);                      // 30 20 10
    }
    printf("empty = %d\n", lystack_is_empty(stack));    // 1
    lystack_delete(stack);
    return 0;
}

#endif /* LYDSS_STACK_ENABLED */
