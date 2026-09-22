/**
 * @file queue_basic.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief
 * @date 2025-08-06
 *
 * @copyright Copyright (c) 2026
 */
/*
#include <stdio.h>
#include "../inc/lydss_api.h"

#define QUEUE_SIZE 10

int queue_basic()
{
    printf("Start queue api test...\n");

    int temp = 0;
    
    Queue_t *queue_handle = queue_create(QUEUE_SIZE, sizeof(int));

    temp = 0xa55a;
    printf("Send value1: 0x%x\n", temp);
    queue_send(queue_handle, &temp);
    temp = 0x5aa5;
    printf("Send value2: 0x%x\n", temp);
    queue_send(queue_handle, &temp);

    temp = 0;
    queue_receive(queue_handle, &temp);
    printf("Receive value1: 0x%x\n", temp);
    queue_receive(queue_handle, &temp);
    printf("Receive value2: 0x%x\n", temp);

    int send_buf[QUEUE_SIZE] = { 0 }, recv_buf[QUEUE_SIZE] = { 0 };
    for(int i = 0; i < QUEUE_SIZE; i++) {
        send_buf[i] = i;
    }

    printf("Send multi-value: ");
    for(int i = 0; i < QUEUE_SIZE; i++) {
        printf("%d ", send_buf[i]);
    }
    queue_send_multi(queue_handle, send_buf, QUEUE_SIZE);
    printf("\n");
    queue_receive_multi(queue_handle, recv_buf, QUEUE_SIZE);
    printf("Receive multi-value: ");
    for(int i = 0; i < QUEUE_SIZE; i++) {
        printf("%d ", recv_buf[i]);
    }
    printf("\n");

    for(int i = 0; i < QUEUE_SIZE; i++){
        if(recv_buf[i] != send_buf[i]) {
            printf("Queue api test failed!\n");
            return 0;
        }
    }
    printf("Queue api test success!\n");

    return 1;
}
*/