/**
 * @file simple.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief 
 * @date 2025-07-16
 * 
 * @copyright Copyright (c) 2025
 * 
 * @note 
 * Matters related to this document.
 */

#include <stdio.h>
#include "../lybox.h"

int simple()
{
    lybox_t *lybox = lybox_create(lyboxType_GENERAL, 6, sizeof(int));

    // lybox->read_mode = lyboxReadMode_FILO_CHUNK;

    int data1[6] = {1,2,3,4,5,6};

    lybox_send(lybox, &data1[0], 3);
    printf("r:%d,w:%d\n", lybox->read_index, lybox->write_index);
    lybox_send(lybox, &data1[3], 2);
    printf("r:%d,w:%d\n", lybox->read_index, lybox->write_index);
    lybox_send(lybox, &data1[5], 1);
    printf("r:%d,w:%d\n", lybox->read_index, lybox->write_index);
    printf("Box data: ");
    for(int i = 0; i < 6; i++) {
        printf("%d ", *(int*)(lybox->box + i * lybox->member_size));
    }
    printf("\n");

    int data2[6] = {0};

    lybox_receive(lybox, &data2[0], 3);
    printf("r:%d,w:%d\n", lybox->read_index, lybox->write_index);
    lybox_receive(lybox, &data2[3], 2);
    printf("r:%d,w:%d\n", lybox->read_index, lybox->write_index);
    lybox_receive(lybox, &data2[5], 1);
    printf("r:%d,w:%d\n", lybox->read_index, lybox->write_index);
    printf("Received data: ");
    for(int i = 0; i < 6; i++) {
        printf("%d ", data2[i]);
    }
    printf("\n");

    return 0;
}