/**
 * @file run_all_basic_test.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief
 * @date 2025-07-16
 *
 * @copyright Copyright (c) 2026
 *
 * @note 每个调用由对应功能开关保护，裁剪掉的容器连同其测试一起跳过。
 *       用例返回非 0 即视为失败，status 累积后作为进程退出码交给 ctest。
 */

#include "../inc/lydss_cfg.h"

#include <stdio.h>
#include <stdlib.h>

int main()
{
    int status = 0;

#if LYDSS_POOL_ENABLED
    /** ./test/pool_basic.c */
    printf("---- 池（Pool）：按名字登记与取用外部对象 ----\n");
    extern int pool_basic();
    status |= pool_basic();
    printf("\n");
#endif

#if LYDSS_ARRAY_ENABLED
    /** ./test/array_basic.c */
    printf("---- 数组（Array）：定长槽位，任意下标直接写入 ----\n");
    extern int array_basic();
    status |= array_basic();
    printf("\n");
#endif

#if LYDSS_VECTOR_ENABLED
    /** ./test/vector_basic.c */
    printf("---- 动态数组（Vector）：容量随追加倍增 ----\n");
    extern int vector_basic();
    status |= vector_basic();
    printf("\n");
#endif

#if LYDSS_STACK_ENABLED
    /** ./test/stack_basic.c */
    printf("---- 栈（Stack）：后进先出，可承载任意定长类型 ----\n");
    extern int stack_basic();
    status |= stack_basic();
    printf("\n");
#endif

#if LYDSS_QUEUE_ENABLED
    /** ./test/queue_basic.c */
    printf("---- 队列（Queue）：先进先出，满时拒收 ----\n");
    extern int queue_basic();
    status |= queue_basic();
    printf("\n");
#endif

#if LYDSS_LIST_ENABLED
    /** ./test/list_basic.c */
    printf("---- 链表（List）：可在任意节点前后插入 ----\n");
    extern int list_basic();
    status |= list_basic();
    printf("\n");
#endif

#if LYDSS_STREAM_ENABLED
    /** ./test/stream_basic.c */
    printf("---- 流（Stream）：连续数据可分段取出 ----\n");
    extern int stream_basic();
    status |= stream_basic();
    printf("\n");
#endif

#if LYDSS_TABLE_ENABLED
    /** ./test/table_basic.c */
    printf("---- 表（Table）：字符串键映射到值指针 ----\n");
    extern int table_basic();
    status |= table_basic();
    printf("\n");
#endif

#if LYDSS_SET_ENABLED
    /** ./test/set_basic.c */
    printf("---- 集合（Set）：元素唯一，重复添加被拒 ----\n");
    extern int set_basic();
    status |= set_basic();
    printf("\n");
#endif

#if LYDSS_BUCKET_ENABLED
    /** ./test/bucket_basic.c */
    printf("---- 桶（Bucket）：命名检索并保持顺序，可带元数据 ----\n");
    extern int bucket_basic();
    status |= bucket_basic();
    printf("\n");
#endif

    return status;
}
