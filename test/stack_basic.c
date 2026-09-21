/**
 * @file stack_basic.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief
 * @date 2026-08-18
 *
 * @copyright Copyright (c) 2026
 *
 * @note
 * Matters related to this document.
 */

#include "../inc/lydss_api.h"

#if LYDSS_STACK_ENABLED

#include <stdio.h>

int stack_basic()
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
