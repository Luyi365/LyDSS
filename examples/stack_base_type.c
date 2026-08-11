/**
 * @file stack_basic.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief
 * @date 2025-08-06
 *
 * @copyright Copyright (c) 2026
 *
 * @note
 * Matters related to this document.
 */
/*
#include <stdio.h>
#include "../inc/lydds_api.h"

#define STACK_SIZE 10

int stack_basic()
{
    printf("Start stack api test...\n");

    int temp = 0;

    Stack_t *stack_handle = stack_create(STACK_SIZE, sizeof(int));

    temp = 0xa55a;
    printf("Push value1: 0x%x\n", temp);
    stack_push(stack_handle, &temp);
    temp = 0x5aa5;
    printf("Push value2: 0x%x\n", temp);
    stack_push(stack_handle, &temp);

    temp = 0;
    stack_pop(stack_handle, &temp);
    printf("Pop value1: 0x%x\n", temp);
    stack_pop(stack_handle, &temp);
    printf("Pop value2: 0x%x\n", temp);

    int push_buf[STACK_SIZE] = { 0 }, pop_buf[STACK_SIZE] = { 0 };
    for(int i = 0; i < STACK_SIZE; i++) {
        push_buf[i] = i;
    }

    printf("Push multi-value1: ");
    for(int i = 0; i < STACK_SIZE / 2; i++) {
        printf("%d ", push_buf[i]);
    }
    printf("\n");
    stack_push_multi(stack_handle, push_buf, STACK_SIZE / 2);
    printf("Push multi-value2: ");
    for(int i = 0; i < STACK_SIZE / 2; i++) {
        printf("%d ", push_buf[STACK_SIZE / 2 + i]);
    }
    printf("\n");
    stack_push_multi(stack_handle, &push_buf[STACK_SIZE / 2], STACK_SIZE / 2);

    stack_pop_multi(stack_handle, pop_buf, STACK_SIZE / 2);
    printf("Pop multi-value1: ");
    for(int i = 0; i < STACK_SIZE / 2; i++) {
        printf("%d ", pop_buf[i]);
    }
    printf("\n");
    stack_pop_multi(stack_handle, &pop_buf[STACK_SIZE / 2], STACK_SIZE / 2);
    printf("Pop multi-value2: ");
    for(int i = 0; i < STACK_SIZE / 2; i++) {
        printf("%d ", pop_buf[STACK_SIZE / 2 + i]);
    }
    printf("\n");

    for(int i = 0; i < STACK_SIZE / 2; i++){
        if(pop_buf[STACK_SIZE / 2 + i] != push_buf[i]) {
            printf("Stack api test failed!\n");
            return 0;
        }
    }
    for(int i = 0; i < STACK_SIZE / 2; i++){
        if(pop_buf[i] != push_buf[STACK_SIZE / 2 + i]) {
            printf("Stack api test failed!\n");
            return 0;
        }
    }
    printf("Stack api test success!\n");

    return 1;
}
*/