/**
 * @file run_simple_test.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief
 * @date 2026-08-07
 *
 * @copyright Copyright (c) 2026
 *
 * @note
 */

#include "../inc/lydss_cfg.h"

#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int status = 0;

    /** ./test/simple.c */
    extern int simple(void);
    status |= simple();
    printf("\n");

    return status;
}
