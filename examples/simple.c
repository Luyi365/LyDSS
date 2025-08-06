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
    event_group_t *eg = event_group_create();
    event_group_set(eg, EVENT_BIT_0 | EVENT_BIT_2 | EVENT_BIT_3);
    
    printf("eg:%d\n", event_group_clear(eg, EVENT_BIT_2));
    event_group_delete(eg);
    
    return 0;
}