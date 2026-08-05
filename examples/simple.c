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
    LyBOX_t *handle = lybox_create(10, sizeof(int));

    int data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int data2[10] = {0};
    lybox_send(handle, data, 2, LyboxWriteMode_Default);
    lybox_receive(handle, data, 2, LyboxReadMode_FIFO);

    lybox_send(handle, data, 7, LyboxWriteMode_Default);
    lybox_send(handle, data, 8, LyboxWriteMode_Default);
    lybox_cover(handle, &data2[0], 3);
    lybox_set(handle, data2, 3, 0, LyboxWriteMode_Default);

    lybox_receive(handle, data2, handle->count, LyboxReadMode_FIFO);

    for(int i = 0; i < 10; i++)
    {
        printf("%d ", data2[i]);
    }
    printf("\n");

    return 0;
}


#include "../lybkt.h"

int simple()
{
	int a = 5, b = 10;
	int c = 0, d = 0;
	LyBKT_t *bucket = set_create();
	
	SET_ADD(bucket, a);
	SET_ADD(bucket, b);
	set_take(bucket, &c);
	set_take(bucket, &d);
	
	printf("TEST1 = %d\n", c);
	printf("TEST1 = %d\n", d);
	
	return 0;
}


#include "../lypool.h"

int simple()
{
    int a = 5;
    lypool_t *pool = lypool_create(10);

    lypool_post(pool, "TEST1", &a);

    printf("a = %d\n", *(int*)(lypool_get(pool, "TEST1")));

    return 0;
}