/**
 * @file run_all_examples.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief
 * @date 2025-07-16
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

    /** ./test/pool_basic.c */
    extern int pool_basic();
    status = pool_basic();
    printf("\n");

    /** ./test/queue_basic.c */
    // extern int queue_basic();
    // status = queue_basic();
    // printf("\n");

    /** ./test/stack_basic.c */
    // extern int stack_basic();
    // status = stack_basic();
    // printf("\n");

    return 0;
}