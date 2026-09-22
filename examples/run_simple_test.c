/**
 * @file run_simple_test.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief
 * @date 2026-08-07
 *
 * @copyright Copyright (c) 2026
 *
 * @note simple() 用栈，调用点的开关须与 test/simple.c 的门控成对出现。
 */

#include "../inc/lydss_cfg.h"

#include <stdio.h>
#include <stdlib.h>

int main()
{
    int status = 0;

#if LYDSS_STACK_ENABLED
    /** ./test/simple.c */
    extern int simple();
    status |= simple();
    printf("\n");
#endif

    return status;
}
