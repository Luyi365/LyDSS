/**
 * @file simple.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief 
 * @date 2025-08-28
 * 
 * @copyright Copyright (c) 2025
 * 
 * @note 
 * Matters related to this document.
 */

#include <stdio.h>
#include "../lybkt.h"

int simple()
{
	int a = 5, b = 10;
	LyBKT_t *bucket = lybkt_create();
	
	lybkt_put(bucket, "TEST1", &a);
	
	printf("TEST1 = %d\n", *(int*)(lybkt_get(bucket, "TEST1")));
	
	lybkt_post(bucket, "TEST1", &b);
	
	printf("TEST1 = %d\n", *(int*)(lybkt_get(bucket, "TEST1")));
	
	return 0;
}
