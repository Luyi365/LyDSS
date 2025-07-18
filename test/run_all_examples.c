/**
 * @file run_all_examples.c
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
#include <stdlib.h>

int main() 
{
    int status = 0;

    /** ./examples/simple.c */
extern int simple();
    status = simple();

    return 0;
}