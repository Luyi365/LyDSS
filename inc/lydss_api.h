/**
 * @file lydss_api.h
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief
 * @date 2026-08-07
 *
 * @copyright Copyright (c) 2026
 *
 * @note
 * 十一个容器的公开声明，分节顺序为：池、数组、动态数组、乒乓缓冲、栈、队列、链表、流、表、集合、桶。
 * 每节都由 lydss_cfg.h 里对应的 LYDSS_<名称>_ENABLED 包裹，容器被裁剪后该节不参与编译。
 * 新增容器时按抽象层级插入到对应位置，不要追加到末尾。
 *
 *
 *   ------------------
 * /  如果代码有帮助到你  \
 * \  也请作者喝杯咖啡吧  /
 *  ------------------
 *      \   (__)
 *       \  /oo|\_______
 *         (_"_)\  牛码  )\/\
 *  █████   U   ||-----j/  ██████████   ██████████    █████████
 * ░░███    '   ||     || ░░███░░░░███ ░░███░░░░███  ███░░░░░███
 *  ░███    '   █████ ████ ░███   ░░███ ░███   ░░███░███    ░░░
 *  ░███    ;  ░░███ ░███  ░███    ░███ ░███    ░███░░█████████
 *  ░███ ~~~^~~;░███ ░███  ░███    ░███ ░███    ░███ ░░░░░░░░███
 *  ░███ ~~~~~█;░███ ░███  ░███    ███  ░███    ███  ███    ░███
 *  ███████████;░░███████  ██████████   ██████████  ░░█████████
 * ░░░░░░░░░░░;; ░░░░░███ ░░░░░░░░░░   ░░░░░░░░░░    ░░░░░░░░░
 *          ;;;  ███ ░███
 *          ;;  ░░██████
 *           ;   ░░░░░░
 */

#ifndef LYDSS_API_H
#define LYDSS_API_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "lydss_cfg.h"

#if LYDSS_ASSERT_ENABLED

#define LYDSS_ASSERT(EXPR)                                                       \
    do {                                                                         \
        if(!(EXPR)) {                                                            \
            LYDSS_PRINTF("(%s) has assert failed at %s.\n", #EXPR, __func__);    \
            while(1)                                                             \
            {                                                                    \
                LYDSS_TRAP();                                                    \
            }                                                                    \
        }                                                                        \
    } while(0)

#else

#define LYDSS_ASSERT(EXPR)                                                       \
    do {                                                                         \
        (void)(EXPR);                                                            \
    } while(0)

#endif /* LYDSS_ASSERT_ENABLED */

/* ------------------------------------ 池 ----------------------------------- */
#if LYDSS_POOL_ENABLED

typedef struct LyPool LyPool_t;

/// @brief 池的槽位
typedef struct LyPoolItem {
    const char *name;           ///< 对象名称
    void *object;               ///< 对象指针
} LyPoolItem_t;

typedef struct LyPoolStatic {
    void *opaque[3];
} LyPoolStatic_t;

/**
 * @brief 创建一个池
 *
 * @param size 池容量
 * @return 池句柄，内存不足时返回 NULL
 */
LyPool_t *lypool_create(size_t size);

/**
 * @brief 创建一个静态池
 *
 * @param storage 句柄存储，生命周期须覆盖池的整个使用期
 * @param items 槽位数组，会被全部清空
 * @param size 槽位数量
 * @return 池句柄，即 storage 本身
 */
LyPool_t *lypool_create_static(LyPoolStatic_t *storage, LyPoolItem_t *items, size_t size);

/**
 * @brief 将一个对象引入池
 *
 * @param pool 池
 * @param name 对象名称
 * @param obj 对象
 * @return true 引入成功，false 池已满
 */
bool lypool_post(LyPool_t *pool, const char *const name, void *obj);

/**
 * @brief 获取池中的指定对象
 *
 * @param pool 池
 * @param name 对象名称
 */
void *lypool_get(LyPool_t *pool, const char *const name);

/**
 * @brief 清除池中的指定对象
 *
 * @param pool 池
 * @param name 对象名称
 */
void lypool_clear(LyPool_t *pool, const char *const name);

/**
 * @brief 销毁指定的池
 *
 * @param pool 池
 *
 * @note 静态池只解除对象数组绑定，不释放调用者提供的内存与句柄
 */
void lypool_delete(LyPool_t *pool);

#endif /* LYDSS_POOL_ENABLED */

/* ----------------------------------- 数组 ----------------------------------- */
#if LYDSS_ARRAY_ENABLED

/**
 * @note
 * 为 C 原生数组补齐运行时元数据（长度与成员大小）的定长存储原语。
 * 全部 length 个槽位自始存在，不维护已用数量。
 */
typedef struct LyArray LyArray_t;

typedef struct LyArrayStatic {
    void *opaque[4];
} LyArrayStatic_t;

/**
 * @brief 创建一个数组
 *
 * @param len 数组长度
 * @param item_size 单个成员大小
 * @return 数组句柄，len 与 item_size 相乘溢出或内存不足时返回 NULL
 *
 * @note 成员区清零后交付
 */
LyArray_t *lyarray_create(size_t len, size_t item_size);

/**
 * @brief 创建一个静态数组
 *
 * @param storage 句柄存储，生命周期须覆盖数组的整个使用期
 * @param buf 成员区，至少 len * item_size 字节
 * @param len 数组长度
 * @param item_size 单个成员大小
 * @return 数组句柄，即 storage 本身
 *
 * @note 不改动 buf 的既有内容，槽位初值由调用者负责
 */
LyArray_t *lyarray_create_static(LyArrayStatic_t *storage, void *buf, size_t len, size_t item_size);

/**
 * @brief 写入指定下标的成员
 *
 * @param array 数组
 * @param index 下标
 * @param item 数据项
 * @return true 写入成功，false 下标越界
 */
bool lyarray_set(LyArray_t *array, size_t index, const void *item);

/**
 * @brief 获取指定下标成员的地址
 *
 * @param array 数组
 * @param index 下标
 * @return void* 成员地址，下标越界返回 NULL
 *
 * @note 返回的是成员区内部地址，数组销毁后失效
 */
void *lyarray_get(LyArray_t *array, size_t index);

/**
 * @brief 读取指定下标的成员到缓冲区
 *
 * @param array 数组
 * @param index 下标
 * @param buf 缓冲区
 * @return true 读取成功，false 下标越界
 */
bool lyarray_read(LyArray_t *array, size_t index, void *buf);

/**
 * @brief 删除数组
 *
 * @param array 数组
 *
 * @note 静态数组只解除成员区绑定，不释放调用者提供的内存与句柄
 */
void lyarray_delete(LyArray_t *array);

/**
 * @brief 获取数组可容纳的成员数量
 *
 * @param array 数组
 * @return 数组长度
 */
size_t lyarray_length(LyArray_t *array);

/**
 * @brief 获取单个成员的大小
 *
 * @param array 数组
 * @return 成员大小（单位：字节）
 */
size_t lyarray_item_size(LyArray_t *array);

#endif /* LYDSS_ARRAY_ENABLED */

/* ---------------------------------- 动态数组 ---------------------------------- */
#if LYDSS_VECTOR_ENABLED

/**
 * @note
 * 自持成员区的动态数组，容量不足时按倍数增长。索引边界对 size 而非 capacity 判定。
 */
typedef struct LyVector LyVector_t;

/**
 * @brief 创建一个动态数组
 *
 * @param len 初始容量
 * @param item_size 单个成员大小
 * @return 动态数组句柄，len 与 item_size 相乘溢出或内存不足时返回 NULL
 */
LyVector_t *lyvector_create(size_t len, size_t item_size);

/**
 * @brief 预留容量
 *
 * @param vector 动态数组
 * @param len 期望容量
 * @return true 预留成功，false 内存不足
 *
 * @note 只增不减
 */
bool lyvector_reserve(LyVector_t *vector, size_t len);

/**
 * @brief 在尾部追加成员
 *
 * @param vector 动态数组
 * @param item 数据项
 * @return true 追加成功，false 内存不足
 *
 * @note 容量不足时触发重分配，此前取得的成员地址随之失效
 */
bool lyvector_push_rear(LyVector_t *vector, const void *item);

/**
 * @brief 写入指定下标的成员
 *
 * @param vector 动态数组
 * @param index 下标
 * @param item 数据项
 * @return true 写入成功，false 下标越界
 */
bool lyvector_set(LyVector_t *vector, size_t index, const void *item);

/**
 * @brief 获取指定下标成员的地址
 *
 * @param vector 动态数组
 * @param index 下标
 * @return void* 成员地址，下标越界返回 NULL
 *
 * @note 任何触发重分配的操作之后该地址失效
 */
void *lyvector_get(LyVector_t *vector, size_t index);

/**
 * @brief 读取指定下标的成员到缓冲区
 *
 * @param vector 动态数组
 * @param index 下标
 * @param buf 缓冲区
 * @return true 读取成功，false 下标越界
 */
bool lyvector_read(LyVector_t *vector, size_t index, void *buf);

/**
 * @brief 取出尾部的成员
 *
 * @param vector 动态数组
 * @param buf 缓冲区，为 NULL 时丢弃取出的成员
 * @return true 取出成功，false 为空
 *
 * @note 只减少 size，不收缩容量
 */
bool lyvector_pop_rear(LyVector_t *vector, void *buf);

/**
 * @brief 清空成员
 *
 * @param vector 动态数组
 * @return true 清空成功
 *
 * @note 只把 size 归零，容量保持不变
 */
bool lyvector_clear(LyVector_t *vector);

/**
 * @brief 将容量收缩到当前成员数量
 *
 * @param vector 动态数组
 * @return true 收缩成功，false 内存不足
 *
 * @note 为空时保留一个槽位
 */
bool lyvector_shrink(LyVector_t *vector);

/**
 * @brief 删除动态数组
 *
 * @param vector 动态数组
 */
void lyvector_delete(LyVector_t *vector);

/**
 * @brief 获取当前成员数量
 *
 * @param vector 动态数组
 * @return 成员数量
 */
size_t lyvector_size(LyVector_t *vector);

/**
 * @brief 获取当前容量
 *
 * @param vector 动态数组
 * @return 容量
 */
size_t lyvector_capacity(LyVector_t *vector);

/**
 * @brief 判断是否为空
 *
 * @param vector 动态数组
 * @return true 为空，false 不为空
 */
bool lyvector_is_empty(LyVector_t *vector);

#endif /* LYDSS_VECTOR_ENABLED */

/* ---------------------------------- 乒乓缓冲 ---------------------------------- */
#if LYDSS_PINGPONG_ENABLED

/**
 * @note
 * 两块等长缓冲轮换：填充块接受写入，就绪块整块交给消费方。
 * 成员区创建后不再迁移，基址可预先配置给 DMA 等外部写入方。
 * 不含同步措施，跨中断或线程使用时交换与释放须置于临界区内。
 */
typedef struct LyPingPong LyPingPong_t;

typedef struct LyPingPongStatic {
    void *opaque[9];
} LyPingPongStatic_t;

/**
 * @brief 创建一个乒乓缓冲
 *
 * @param len 单块可容纳的成员数量
 * @param item_size 单个成员大小
 * @return 乒乓缓冲句柄，len 与 item_size 相乘溢出或内存不足时返回 NULL
 *
 * @note 两块成员区清零后交付
 */
LyPingPong_t *lypingpong_create(size_t len, size_t item_size);

/**
 * @brief 创建一个静态乒乓缓冲
 *
 * @param storage 句柄存储，生命周期须覆盖乒乓缓冲的整个使用期
 * @param buf_a 第一块成员区，至少 len * item_size 字节
 * @param buf_b 第二块成员区，与第一块等长且互不重叠
 * @param len 单块可容纳的成员数量
 * @param item_size 单个成员大小
 * @return 乒乓缓冲句柄，即 storage 本身
 *
 * @note 不改动两块成员区的既有内容，初值由调用者负责
 */
LyPingPong_t *lypingpong_create_static(LyPingPongStatic_t *storage, void *buf_a, void *buf_b,
                                       size_t len, size_t item_size);

/**
 * @brief 向填充块写入一个成员
 *
 * @param pingpong 乒乓缓冲
 * @param item 数据项
 * @return true 写入成功，false 填充块已满
 */
bool lypingpong_write(LyPingPong_t *pingpong, const void *item);

/**
 * @brief 取得填充块的基址
 *
 * @param pingpong 乒乓缓冲
 * @return void* 填充块基址，可写入至多 length 个成员
 *
 * @note 直接写入后须调用 lypingpong_commit 报告数量，不可与 lypingpong_write 混用
 */
void *lypingpong_fill_data(LyPingPong_t *pingpong);

/**
 * @brief 设置填充块的已写入成员数量
 *
 * @param pingpong 乒乓缓冲
 * @param count 成员数量
 * @return true 设置成功，false count 超出单块容量
 *
 * @note 以绝对值覆盖，不与 lypingpong_write 的累加结果叠加
 */
bool lypingpong_commit(LyPingPong_t *pingpong, size_t count);

/**
 * @brief 判断填充块是否已满
 *
 * @param pingpong 乒乓缓冲
 * @return true 已满，false 未满
 */
bool lypingpong_is_full(LyPingPong_t *pingpong);

/**
 * @brief 交换两块缓冲的角色
 *
 * @param pingpong 乒乓缓冲
 * @return true 交换成功，false 就绪块尚未 lypingpong_release
 */
bool lypingpong_swap(LyPingPong_t *pingpong);

/**
 * @brief 取得就绪块的基址
 *
 * @param pingpong 乒乓缓冲
 * @return void* 就绪块基址，可原地处理
 */
void *lypingpong_ready_data(LyPingPong_t *pingpong);

/**
 * @brief 取得就绪块的成员数量
 *
 * @param pingpong 乒乓缓冲
 * @return 成员数量，为 0 表示无待消费数据
 */
size_t lypingpong_ready_count(LyPingPong_t *pingpong);

/**
 * @brief 释放就绪块
 *
 * @param pingpong 乒乓缓冲
 */
void lypingpong_release(LyPingPong_t *pingpong);

/**
 * @brief 删除乒乓缓冲
 *
 * @param pingpong 乒乓缓冲
 *
 * @note 静态乒乓缓冲只解除两块成员区的绑定，不释放调用者提供的内存与句柄
 */
void lypingpong_delete(LyPingPong_t *pingpong);

/**
 * @brief 获取单块可容纳的成员数量
 *
 * @param pingpong 乒乓缓冲
 * @return 单块容量
 */
size_t lypingpong_length(LyPingPong_t *pingpong);

/**
 * @brief 获取单个成员的大小
 *
 * @param pingpong 乒乓缓冲
 * @return 成员大小（单位：字节）
 */
size_t lypingpong_item_size(LyPingPong_t *pingpong);

#endif /* LYDSS_PINGPONG_ENABLED */

/* --------------------------------- 栈(FILO) -------------------------------- */
#if LYDSS_STACK_ENABLED

typedef struct LyStack LyStack_t;

typedef struct LyStackStatic {
    void *opaque[5];
} LyStackStatic_t;

/**
 * @brief 创建一个栈
 *
 * @param len 栈长度，未校验；传 0 会构造成功但栈永久同时为空且为满
 * @param item_size 单个成员大小
 * @return 栈句柄，len 与 item_size 相乘溢出或内存不足时返回 NULL
 */
LyStack_t *lystack_create(size_t len, size_t item_size);

/**
 * @brief 创建一个静态栈
 *
 * @param storage 句柄存储，生命周期须覆盖栈的整个使用期
 * @param buf 成员区，至少 len * item_size 字节
 * @param len 栈长度
 * @param item_size 单个成员大小
 * @return 栈句柄，即 storage 本身
 */
LyStack_t *lystack_create_static(LyStackStatic_t *storage, void *buf, size_t len, size_t item_size);

/**
 * @brief 数据入栈
 *
 * @param stack 栈
 * @param item 数据项
 * @return true 入栈成功，false 栈已满
 */
bool lystack_push(LyStack_t *stack, void *item);

/**
 * @brief 数据出栈
 *
 * @param stack 栈
 * @param buf 缓冲区
 * @return true 出栈成功，false 栈为空
 */
bool lystack_pop(LyStack_t *stack, void *buf);

/**
 * @brief 预读栈数据
 *
 * @param stack 栈
 * @param buf 缓冲区
 * @return true 预读成功，false 栈为空
 */
bool lystack_peek(LyStack_t *stack, void *buf);

/**
 * @brief 清空栈
 *
 * @param stack 栈
 * @return 恒为 true
 */
bool lystack_clear(LyStack_t *stack);

/**
 * @brief 删除栈
 *
 * @param stack 栈
 *
 * @note 静态栈只解除栈数据绑定，不释放调用者提供的内存与句柄
 */
void lystack_delete(LyStack_t *stack);

/**
 * @brief 获取栈中当前的成员个数
 *
 * @param stack 栈
 * @return 成员个数
 */
size_t lystack_count(LyStack_t *stack);

/**
 * @brief 判断栈是否为空
 *
 * @param stack 栈
 * @return true 为空，false 不为空
 */
bool lystack_is_empty(LyStack_t *stack);

/**
 * @brief 判断栈是否已满
 *
 * @param stack 栈
 * @return true 已满，false 未满
 */
bool lystack_is_full(LyStack_t *stack);

#endif /* LYDSS_STACK_ENABLED */

/* -------------------------------- 队列(FIFO) -------------------------------- */
#if LYDSS_QUEUE_ENABLED

typedef struct LyQueue LyQueue_t;

typedef struct LyQueueStatic {
    void *opaque[7];
} LyQueueStatic_t;

/**
 * @brief 创建一个队列
 *
 * @param len 队列长度，未校验；传 0 会构造成功但队列永久同时为空且为满
 * @param item_size 单个成员大小
 * @return 队列句柄，len 与 item_size 相乘溢出或内存不足时返回 NULL
 */
LyQueue_t *lyqueue_create(size_t len, size_t item_size);

/**
 * @brief 创建一个静态队列
 *
 * @param storage 句柄存储，生命周期须覆盖队列的整个使用期
 * @param buf 成员区，至少 len * item_size 字节
 * @param len 队列长度
 * @param item_size 单个成员大小
 * @return 队列句柄，即 storage 本身
 */
LyQueue_t *lyqueue_create_static(LyQueueStatic_t *storage, void *buf, size_t len, size_t item_size);

/**
 * @brief 数据入队
 *
 * @param queue 队列
 * @param item 数据项
 * @return true 入队成功，false 队列已满
 */
bool lyqueue_send(LyQueue_t *queue, void *item);

/**
 * @brief 数据出队
 *
 * @param queue 队列
 * @param buf 缓冲区
 * @return true 出队成功，false 队列为空
 */
bool lyqueue_receive(LyQueue_t *queue, void *buf);

/**
 * @brief 预读队列数据
 *
 * @param queue 队列
 * @param buf 缓冲区
 * @return true 预读成功，false 队列为空
 */
bool lyqueue_peek(LyQueue_t *queue, void *buf);

/**
 * @brief 清空队列
 *
 * @param queue 队列
 * @return 恒为 true
 */
bool lyqueue_clear(LyQueue_t *queue);

/**
 * @brief 删除队列
 *
 * @param queue 队列
 *
 * @note 静态队列只解除成员区绑定，不释放调用者提供的内存与句柄
 */
void lyqueue_delete(LyQueue_t *queue);

/**
 * @brief 获取队列中当前的成员个数
 *
 * @param queue 队列
 * @return 成员个数
 */
size_t lyqueue_count(LyQueue_t *queue);

/**
 * @brief 判断队列是否为空
 *
 * @param queue 队列
 * @return true 为空，false 不为空
 */
bool lyqueue_is_empty(LyQueue_t *queue);

/**
 * @brief 判断队列是否已满
 *
 * @param queue 队列
 * @return true 已满，false 未满
 */
bool lyqueue_is_full(LyQueue_t *queue);

#endif /* LYDSS_QUEUE_ENABLED */

/* ----------------------------------- 链表 ----------------------------------- */
#if LYDSS_LIST_ENABLED

typedef struct LyListNode {
    void *data;
    struct LyListNode *prev;
    struct LyListNode *next;
} LyListNode_t;

struct LyList {
    LyListNode_t *head;
    LyListNode_t *tail;
};
typedef struct LyList LyList_t;

/**
 * @brief 创建链表
 *
 * @return 链表句柄，内存不足时返回 NULL
 */
LyList_t *lylist_create(void);

/**
 * @brief 销毁链表
 *
 * @param list 链表
 *
 * @note 只释放节点，不释放 data 指向的业务对象，也不释放 list 句柄本身
 */
void lylist_destroy(LyList_t *list);

/**
 * @brief 链表头部插入
 *
 * @param list 链表
 * @param data 数据，不校验是否为 NULL
 * @return 生成的节点，内存不足时返回 NULL
 */
LyListNode_t *lylist_push_front(LyList_t *list, void *data);

/**
 * @brief 链表尾部插入
 *
 * @param list 链表
 * @param data 数据，不校验是否为 NULL
 * @return 生成的节点，内存不足时返回 NULL
 */
LyListNode_t *lylist_push_rear(LyList_t *list, void *data);

/**
 * @brief 在节点前插入
 *
 * @param list 链表
 * @param node 目标节点，必须已属于 list
 * @param data 数据，不校验是否为 NULL
 * @return 生成的节点，内存不足时返回 NULL
 */
LyListNode_t *lylist_insert_before(LyList_t *list, LyListNode_t *node, void *data);

/**
 * @brief 在节点后插入
 *
 * @param list 链表
 * @param node 目标节点，必须已属于 list
 * @param data 数据，不校验是否为 NULL
 * @return 生成的节点，内存不足时返回 NULL
 */
LyListNode_t *lylist_insert_after(LyList_t *list, LyListNode_t *node, void *data);

/**
 * @brief 弹出链表头部节点
 *
 * @param list 链表
 * @return 节点携带的 data；链表为空时返回 NULL
 *
 * @note 若存入的 data 本身是 NULL，返回值无法与「链表为空」区分。
 */
void *lylist_pop_front(LyList_t *list);

/**
 * @brief 弹出链表尾部节点
 *
 * @param list 链表
 * @return 节点携带的 data；链表为空时返回 NULL
 *
 * @note 若存入的 data 本身是 NULL，返回值无法与「链表为空」区分。
 */
void *lylist_pop_rear(LyList_t *list);

/**
 * @brief 移除节点
 *
 * @param list 链表
 * @param node 节点，必须属于 list
 * @return 节点携带的 data
 *
 * @note 节点随本调用一并释放，调用后不得再使用 node。
 */
void *lylist_remove(LyList_t *list, LyListNode_t *node);

/**
 * @brief 从头到尾遍历链表，node 为循环内可用的节点变量
 *
 * @note 循环体内**不得**删除当前节点：宏在执行完循环体后才读取 node->next，
 *       而 lylist_remove 已经释放了该节点，属于未定义行为。
 *       需要边遍历边删除时，请自行先缓存后继指针再删。
 */
#define LYLIST_FOREACH(list, node) \
    for(LyListNode_t *node = (list)->head; node; node = node->next)

#endif /* LYDSS_LIST_ENABLED */

/* ------------------------------------ 流 ----------------------------------- */
#if LYDSS_STREAM_ENABLED

typedef unsigned char cell_t;     ///< 默认以字节为单位
typedef struct LyStream LyStream_t;

typedef struct LyStreamStatic {
    void *opaque[6];
} LyStreamStatic_t;

/**
 * @brief 创建流
 *
 * @param size 缓冲区可容纳的单元数，单元为 cell_t
 * @return 流句柄，size 换算字节数溢出或内存不足时返回 NULL
 */
LyStream_t *lystream_create(size_t size);

/**
 * @brief 创建一个静态流
 *
 * @param storage 句柄存储，生命周期须覆盖流的整个使用期
 * @param buf 缓冲区，至少 size 个 cell_t
 * @param size 缓冲区可容纳的单元数
 * @return 流句柄，即 storage 本身
 */
LyStream_t *lystream_create_static(LyStreamStatic_t *storage, void *buf, size_t size);

/**
 * @brief 数据流入
 *
 * @param stream 流
 * @param data 数据
 * @param size 数据大小
 * @return 实际写入的数据大小
 */
size_t lystream_in(LyStream_t *stream, cell_t *data, size_t size);

/**
 * @brief 数据流出
 *
 * @param stream 流
 * @param buf 缓冲区
 * @param size 缓冲区大小
 * @return 实际读出的数据大小
 */
size_t lystream_out(LyStream_t *stream, cell_t *buf, size_t size);

/**
 * @brief 获取流中当前的数据个数
 *
 * @param stream 流
 * @return 数据个数
 */
size_t lystream_count(LyStream_t *stream);

/**
 * @brief 清空流
 *
 * @param stream 流
 * @return 清空的单元个数
 */
size_t lystream_clean(LyStream_t *stream);

/**
 * @brief 删除流
 *
 * @param stream 流
 *
 * @note 静态流只解除缓冲区绑定，不释放调用者提供的内存与句柄
 */
void lystream_delete(LyStream_t *stream);

#endif /* LYDSS_STREAM_ENABLED */

/* ------------------------------------ 表 ----------------------------------- */
#if LYDSS_TABLE_ENABLED

typedef struct LyTable LyTable_t;
typedef size_t (*LyTableHash_t)(const char *key);

/**
 * @brief 创建表
 *
 * @param hash 哈希函数，传 NULL 则使用默认实现
 * @return 表句柄，内存不足时返回 NULL
 */
LyTable_t *lytable_create(LyTableHash_t hash);

/**
 * @brief 插入或更新键值对
 *
 * @param table 表
 * @param key 键
 * @param value 值
 * @return 0 成功，-1 内存不足或 table/key 为 NULL
 */
int lytable_set(LyTable_t *table, const char *key, void *value);

/**
 * @brief 查找键值对
 *
 * @param table 表
 * @param key 键
 * @return 找到返回 value；未找到、或 table/key 为 NULL 时返回 NULL
 */
void *lytable_get(LyTable_t *table, const char *key);

/**
 * @brief 删除键值对
 *
 * @param table 表
 * @param key 键
 * @return 被删除的 value；未找到、或 table/key 为 NULL 时返回 NULL
 */
void *lytable_remove(LyTable_t *table, const char *key);

/**
 * @brief 获取键值对数量
 *
 * @param table 表
 * @return 键值对数量
 */
size_t lytable_size(LyTable_t *table);

/**
 * @brief 释放表
 *
 * @param table 表
 *
 * @note 不释放 value 指向的内存，由调用者负责
 */
void lytable_destroy(LyTable_t *table);

#endif /* LYDSS_TABLE_ENABLED */

/* ----------------------------------- 集合 ----------------------------------- */
#if LYDSS_SET_ENABLED

typedef struct LySet LySet_t;
typedef unsigned (Hash_t)(const void *data, size_t len);
typedef bool (Equals_t)(const void *a, const void *b);

/**
 * @brief 初始化一个集合
 *
 * @param size 槽位数量，不是元素数量上限；元素数远超它时会退化成长链表
 * @param elem_size 元素大小
 * @param hash 哈希函数，传 NULL 则使用内置的按字节 FNV-1a
 * @param equals 比较函数，不可为 NULL
 * @return 集合句柄，内存不足时返回 NULL
 *
 * @note size 传 0 会构造成功，但随后任何增删查都会对 0 取模，属于未定义行为。
 *       集合不会 rehash，因此槽位数要在这里按预估规模一次给足。
 */
LySet_t *lyset_init(size_t size, size_t elem_size, Hash_t *hash, Equals_t *equals);

/**
 * @brief 向集合中添加元素
 *
 * @param set 集合
 * @param elem 元素
 * @return true 添加成功；false 有三种原因不可区分：元素已存在、elem 为 NULL、内存不足
 */
bool lyset_add(LySet_t *set, const void *elem);

/**
 * @brief 若集合中存在该元素则移除
 *
 * @param set 集合
 * @param elem 元素
 * @return true 移除成功，false 元素不存在或 elem 为 NULL
 */
bool lyset_remove(LySet_t *set, const void *elem);

/**
 * @brief 从集合移除并返回任意一个元素
 *
 * @param set 集合
 * @param out 元素
 * @return true 移除成功，false 空集合
 */
bool lyset_pop(LySet_t *set, void *out);

/**
 * @brief 移除指定容器里所有的元素
 *
 * @param set 集合
 */
void lyset_erase(LySet_t *set);

/**
 * @brief 销毁指定的集合
 *
 * @param set 集合
 */
void lyset_destroy(LySet_t *set);

/**
 * @brief 得到集合的元素数量
 *
 * @param set 集合
 * @return 元素数量
 */
size_t lyset_count(const LySet_t *set);

/**
 * @brief 判断集合中是否包含指定的元素
 *
 * @param set 集合
 * @param elem 元素
 * @return true 包含，false 不包含或 elem 为 NULL
 */
bool lyset_contains(const LySet_t *set, const void *elem);

#endif /* LYDSS_SET_ENABLED */

/* ------------------------------------ 桶 ----------------------------------- */
#if LYDSS_BUCKET_ENABLED

enum LyBucketMetaType {
    LyBucketMetaType_NULL = 0,
    LyBucketMetaType_COPY,
    LyBucketMetaType_ATTACH,
};
typedef enum LyBucketMetaType LyBucketMetaType_t;

struct LyBucketMeta {
    LyBucketMetaType_t type;    ///< 元数据类型
    void *data;                 ///< 元数据（项副本 / 附属数据）
    size_t size;                ///< 元数据大小
};
typedef struct LyBucketMeta LyBucketMeta_t;

typedef struct LyBucketNode LyBucketNode_t;
typedef struct LyBucket LyBucket_t;

/* ---------------------------------- 检查索引名 --------------------------------- */
/**
 * @brief 创建一个桶
 *
 * @return 桶句柄，内存不足时返回 NULL
 */
LyBucket_t *lybucket_create(void);

/**
 * @brief 将一个项放到桶中
 *
 * @param bucket 桶
 * @param name 索引名
 * @param item 项
 * @param meta 元数据
 * @return 生成或被更新的节点，内存不足时返回 NULL
 *
 * @note 有同名则替换，无同名则增加
 */
LyBucketNode_t *lybucket_put(LyBucket_t *bucket, const char *const name, void *item, LyBucketMeta_t meta);

/**
 * @brief 得到最近的一个节点
 *
 * @param bucket 桶
 * @param at_tail 是否从尾部节点检索（默认从头部检索）
 * @return 头部或尾部节点，桶为空时返回 NULL
 */
LyBucketNode_t *lybucket_head(LyBucket_t *bucket, bool at_tail);

/**
 * @brief 获取桶中的节点
 *
 * @param bucket 桶
 * @param name 索引名
 * @return 匹配的节点，未找到返回 NULL
 *
 * @note 检索沿链表逐个 strcmp，代价 O(n)
 */
LyBucketNode_t *lybucket_get(LyBucket_t *bucket, const char *const name);

/**
 * @brief 修改桶中的索引名
 *
 * @param bucket 桶
 * @param old_name 旧的索引名
 * @param new_name 新的索引名
 * @return bool 是否成功修改
 */
bool lybucket_patch_name(LyBucket_t *bucket, const char *const old_name, const char *const new_name);

/**
 * @brief 修改桶中的项指针
 *
 * @param bucket 桶
 * @param name 索引名
 * @param item 项
 * @return bool 是否成功修改
 */
bool lybucket_patch_item(LyBucket_t *bucket, const char *const name, void *item);

/**
 * @brief 修改桶中的元数据
 *
 * @param bucket 桶
 * @param name 索引名
 * @param meta 元数据
 * @return bool 是否成功修改
 */
bool lybucket_patch_meta(LyBucket_t *bucket, const char *const name, LyBucketMeta_t meta);

/**
 * @brief 从桶中移除项但不释放
 *
 * @param bucket 桶
 * @param name 索引名
 * @return 被摘链但未释放的节点，未找到返回 NULL
 *
 * @note 不再使用时以 lybucket_free_node 释放
 */
LyBucketNode_t *lybucket_remove(LyBucket_t *bucket, const char *const name);

/**
 * @brief 从桶中移除一个节点
 *
 * @param bucket 桶
 * @param node 节点指针
 *
 * @note 只摘链不释放，不再使用时以 lybucket_free_node 释放
 */
void lybucket_remove_node(LyBucket_t *bucket, LyBucketNode_t *node);

/**
 * @brief 释放已从桶中摘下的节点
 *
 * @param node 由 lybucket_remove 或 lybucket_remove_node 摘下的节点
 *
 * @note 一并释放元数据副本，不释放 item 指向的业务对象；节点仍在桶中时不得调用
 */
void lybucket_free_node(LyBucketNode_t *node);

/**
 * @brief 删除桶中的节点
 *
 * @param bucket 桶
 * @param name 索引名
 * @return bool 是否成功删除
 */
bool lybucket_erase(LyBucket_t *bucket, const char *const name);

/**
 * @brief 销毁指定的桶
 *
 * @param bucket 桶
 *
 * @note 释放所有节点与元数据副本，不释放 item 指向的业务对象
 */
void lybucket_delete(LyBucket_t *bucket);


/* -------------------------------- 不检查索引名，慎用 ------------------------------- */
/**
 * @brief 将一个项添加到桶
 *
 * @param bucket 桶
 * @param name 索引名
 * @param item 项
 * @param meta 元数据
 * @param at_head 是否从头部节点写入（默认尾部写入）
 * @return 生成的节点，内存不足时返回 NULL
 *
 * @note 可能造成索引名重复，不推荐用户直接使用该函数
 */
LyBucketNode_t *lybucket_post(LyBucket_t *bucket, const char *const name, void *item, LyBucketMeta_t meta, bool at_head);

/**
 * @brief 将一个项添加到桶
 *
 * @param bucket 桶
 * @param name 索引名
 * @param item 项
 * @param meta 元数据
 * @param at_name 定位索引名
 * @param at_before 是否在定位名之前写入（默认在之后写入）
 * @return 生成的节点，内存不足时返回 NULL
 *
 * @note 可能造成索引名重复，不推荐用户直接使用该函数
 */
LyBucketNode_t *lybucket_insert(LyBucket_t *bucket, const char *const name, void *item, LyBucketMeta_t meta, const char *const at_name, bool at_before);

#endif /* LYDSS_BUCKET_ENABLED */

#endif        /* LYDSS_API_H */
