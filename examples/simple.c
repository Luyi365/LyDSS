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
#include "../lybkt_api.h"

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
