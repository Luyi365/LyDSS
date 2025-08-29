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
#include "../lyset.h"

int simple()
{
	int a = 5, b = 10;
	LySET_t *set = lyset_create();
	
	lyset_put_simple(set, "TEST1", &a);
	
	printf("TEST1 = %d\n", *(int*)(lyset_get_item(set, "TEST1")));
	
	lyset_put_simple(set, "TEST1", &b);
	
	printf("TEST1 = %d\n", *(int*)(lyset_get_item(set, "TEST1")));
	
	return 0;
}
