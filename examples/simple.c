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
    lybox_t *lybox = lybox_create(lyboxType_GENERAL, 4, sizeof(int));

    lybox_set_box_attribute(lybox, (struct box_attribute){
        .send_mode = 0,          
        .sort_function = 0,     
        .receive_mode = 0,       
        .receive_direction = 1   
    });

    int data1[6] = {1,2,3,4,5,6};

    lybox_send(lybox, &data1[0], 3);

    lybox_send(lybox, &data1[3], 2);

    lybox_send(lybox, &data1[5], 1);

    printf("Box data: ");
    for(int i = 0; i < 4; i++) {
        printf("%d ", *(int*)(lybox->box + i * lybox->member_size));
    }
    printf("\n");

    int data2[6] = {0};

    lybox_receive(lybox, &data2[0], 3);

    lybox_receive(lybox, &data2[3], 2);

    lybox_receive(lybox, &data2[5], 1);

    printf("Received data: ");
    for(int i = 0; i < 6; i++) {
        printf("%d ", data2[i]);
    }
    printf("\n");

    return 0;
}