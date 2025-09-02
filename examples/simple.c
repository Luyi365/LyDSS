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
	
	LYBKT_KV_PUT(bucket, "TEST1", &a);
	
	printf("TEST1 = %d\n", *(int*)(LYBKT_KV_GET(bucket, "TEST1")));
	
	LYBKT_KV_PUT(bucket, "TEST1", &b);
	
	printf("TEST1 = %d\n", *(int*)(LYBKT_KV_GET(bucket, "TEST1")));
	
	return 0;
}
