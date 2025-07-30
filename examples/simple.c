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
    event_group_t *eg = event_group_create();
    event_group_set(eg, EVENT_BIT_0 | EVENT_BIT_2 | EVENT_BIT_3);
    event_group_clear(eg, EVENT_BIT_2);
    printf("eg:%d\n", event_group_get(eg));
    event_group_delete(eg);

    // lybox_t *lybox = lybox_create(lyboxType_STREAM, 4, sizeof(int));

    // int data1[20] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
    // int data2[6] = {0};

    // lybox_send(lybox, &data1[0], 5);
    // // lybox_receive(lybox, &data2[0], 3);
    // printf("r:%d,w:%d\n", lybox->read_index, lybox->write_index);
    // printf("Box data: ");
    // for(int i = 0; i < 4; i++) {
    //     printf("%d ", *(int*)(lybox->box + i * lybox->member_size));
    // }
    // lybox_send(lybox, &data1[1], 2);
    // printf("r:%d,w:%d\n", lybox->read_index, lybox->write_index);
    // printf("Box data: ");
    // for(int i = 0; i < 4; i++) {
    //     printf("%d ", *(int*)(lybox->box + i * lybox->member_size));
    // }
    // lybox_send(lybox, &data1[0], 4);
    // printf("r:%d,w:%d\n", lybox->read_index, lybox->write_index);
    // printf("Box data: ");
    // for(int i = 0; i < 4; i++) {
    //     printf("%d ", *(int*)(lybox->box + i * lybox->member_size));
    // }
    // printf("\n");

    // lybox_receive(lybox, &data2[0], 2);
    // printf("r:%d,w:%d\n", lybox->read_index, lybox->write_index);
    // lybox_receive(lybox, &data2[2], 2);
    // printf("r:%d,w:%d\n", lybox->read_index, lybox->write_index);
    // lybox_receive(lybox, &data2[5], 1);
    // printf("r:%d,w:%d\n", lybox->read_index, lybox->write_index);
    // printf("Received data: ");
    // for(int i = 0; i < 6; i++) {
    //     printf("%d ", data2[i]);
    // }
    // printf("\n");

    return 0;
}