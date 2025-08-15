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
#include "../lybox_api.h"

int simple()
{
    lybox_t *handle = lybox_create(10, sizeof(int), LyboxReadMode_LIFO_BLOCK, false);

    int data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    lybox_send(handle, data, 8);

    lybox_receive(handle, data, 8);

    for(int i = 0; i < 8; i++)
    {
        printf("%d ", data[i]);
    }
    printf("\n");

    return 0;
}