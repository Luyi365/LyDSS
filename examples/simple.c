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
	
	LYSET_PUT_KV(set, "TEST1", &a);
	
	printf("TEST1 = %d\n", *(int*)(LYSET_GET_KV(set, "TEST1")));
	
	LYSET_PUT_KV(set, "TEST1", &b);
	
	printf("TEST1 = %d\n", *(int*)(LYSET_GET_KV(set, "TEST1")));
	
	return 0;
}
