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
    lybox_t *handle = lybox_create(10, sizeof(int));

    int data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int data2[10] = {0};
    lybox_send(handle, data, 2, LyboxWriteMode_DEFAULT);
    lybox_receive(handle, data, 2, LyboxReadMode_FIFO);

    lybox_send(handle, data, 7, LyboxWriteMode_DEFAULT);
    lybox_send(handle, data, 8, LyboxWriteMode_DEFAULT);

    lybox_receive(handle, data2, handle->count, LyboxReadMode_FIFO);

    for(int i = 0; i < 10; i++)
    {
        printf("%d ", data2[i]);
    }
    printf("\n");

    return 0;
}
