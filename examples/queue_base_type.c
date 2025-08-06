/**
 * @file queue_base_type.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief
 * @date 2025-08-06
 *
 * @copyright Copyright (c) 2025
 *
 * @note
 * Matters related to this document.
 */

#include <stdio.h>
#include "../lybox_api.h"

#define QUEUE_SIZE 10

int queue_base_type()
{
    int temp = 0;
    
    queue_t *queue_handle = queue_create(QUEUE_SIZE, sizeof(int));

    printf("Start queue api test...\n");
    temp = 0xa55a;
    printf("Send value: 0x%x\n", temp);
    queue_send(queue_handle, &temp);
    temp = 0x5aa5;
    printf("Send value: 0x%x\n", temp);
    queue_send(queue_handle, &temp);

    temp = 0;
    queue_receive(queue_handle, &temp);
    printf("Receive value: 0x%x\n", temp);
    queue_receive(queue_handle, &temp);
    printf("Receive value: 0x%x\n", temp);

    int send_buff[QUEUE_SIZE] = { 0 }, recv_buff[QUEUE_SIZE] = { 0 };
    for(int i = 0; i < QUEUE_SIZE; i++) {
        send_buff[i] = i;
    }

    printf("Send mult-value: ");
    for(int i = 0; i < QUEUE_SIZE; i++) {
        printf("%d ", send_buff[i]);
    }
    queue_send_multi(queue_handle, send_buff, QUEUE_SIZE);
    printf("\n");
    queue_receive_multi(queue_handle, recv_buff, QUEUE_SIZE);
    printf("Receive mult-value: ");
    for(int i = 0; i < QUEUE_SIZE; i++) {
        printf("%d ", recv_buff[i]);
    }
    printf("\n");

    for(int i = 0; i < QUEUE_SIZE; i++){
        if(recv_buff[i] != send_buff[i]) {
            printf("Queue api test failed!\n");
            return 0;
        }
    }
    printf("Queue api test success!\n");

    return 1;
}