/**
 * @file run_simple_examples.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief 
 * @date 2026-08-07
 * 
 * @copyright Copyright (c) 2026
 * 
 * @note 
 * Matters related to this document.
 */

#include <stdio.h>
#include <stdlib.h>

int main()
{
    int status = 0;

    /** ./examples/simple.c */
    extern int simple();
    status = simple();
    printf("\n");

    return 0;
}