/**
 * @file lydss_priv.h
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief LyDSS 内部底座层
 * @date 2026-09-16
 *
 * @copyright Copyright (c) 2026
 *
 * @note
 * 容器之间共用的底层操作。文件名中的 priv 表示它只供 src/ 内部使用，
 * **不是公开 API** —— 与 lydss_api.h 同处 inc/ 只是为了保持单一头文件目录的布局。
 * 不要把本文件的任何内容搬进 lydss_api.h，也不要在库外包含它；
 * 底座要保留随时重构的自由。
 *
 * 将来若为外部消费者声明公开包含目录，必须只导出 lydss_api.h 与 lydss_cfg.h，
 * 或把本文件移入 inc/internal/，否则底座会随之变成对外承诺。
 *
 * 全部实现为 static inline 函数或宏，因此：
 * 1. 不产生跨库依赖，十个容器仍然互不链接，功能开关可以自由组合；
 * 2. 编译期完全内联，热路径上没有多余的函数调用与冗余校验；
 * 3. 链式操作采用侵入式设计，底座只改字段，不分配、不持有、不校验业务不变式。
 *
 * 底座刻意不做参数断言。调用方在进入底座之前已经校验过句柄与边界，
 * 重复校验在嵌入式热路径上是纯开销。
 */

#ifndef LYDSS_PRIV_H
#define LYDSS_PRIV_H

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* --------------------------------- 连续存储 -------------------------------- */
/**
 * @note
 * 定长连续存储的描述符 LyStore_t，由数组、动态数组、栈、队列按值嵌入共用，
 * 容器内的成员名统一为 store。
 *
 * 刻意不含 is_static —— 那是所有权标志，不是存储描述。放进来会让
 * 永远不支持静态构造的动态数组白占一个字节加填充（16 涨到 20）。
 * 排除之后四个容器的句柄尺寸与接入前完全一致。
 *
 * 流不接入：它的单元大小固定为 cell_t，item_size 会是纯浪费（24 涨到 28），
 * 而且它的成员区与句柄是一次分配，销毁路径也不同。
 * 池不接入：它的成员是定型数组，按下标直接访问。
 */
typedef struct LyStore {
    void *buffer;               ///< 成员区
    unsigned long length;       ///< 成员区可容纳的成员数量
    unsigned long item_size;    ///< 单个成员的大小（单位：字节）
} LyStore_t;

/**
 * @brief 计算 count 个成员所需的字节数，并拦截乘法溢出
 *
 * @param count 成员数量
 * @param item_size 单个成员大小
 * @param bytes 输出字节数，仅在返回 true 时有效
 * @return true 计算有效，false 会溢出
 *
 * @note 以 SIZE_MAX 为界。在 32 位目标上与 ULONG_MAX 等价，
 *       但在 unsigned long 比 size_t 窄的 ABI 上不会误判。
 */
static inline bool lystore_bytes(size_t count, size_t item_size, size_t *bytes) {
    if(item_size != 0 && count > SIZE_MAX / item_size) {
        return false;
    }

    *bytes = count * item_size;

    return true;
}

/// @brief 计算指定下标成员的地址
static inline void *lystore_address(LyStore_t *store, unsigned long index) {
    return (unsigned char *)store->buffer + index * store->item_size;
}

/// @brief 绑定调用者提供的成员区，不改动其既有内容
static inline void lystore_bind(LyStore_t *store, void *buf, unsigned long len, unsigned long item_size) {
    store->buffer = buf;
    store->length = len;
    store->item_size = item_size;
}

/**
 * @brief 分配自持的成员区
 *
 * @param zeroed true 则清零后交付
 * @return true 分配成功，false 字节数溢出或内存不足
 *
 * @note 失败时不改动 store，调用者只需释放自己的句柄
 */
static inline bool lystore_alloc(LyStore_t *store, unsigned long len, unsigned long item_size, bool zeroed) {
    size_t bytes = 0;
    if(lystore_bytes(len, item_size, &bytes) == false) {
        return false;
    }

    void *buffer = zeroed ? calloc(len, item_size) : malloc(bytes);
    if(buffer == NULL) {
        return false;
    }

    store->buffer = buffer;
    store->length = len;
    store->item_size = item_size;

    return true;
}

/**
 * @brief 重新分配成员区并更新容量
 *
 * @return true 成功，false 字节数溢出或内存不足
 *
 * @note 失败时原成员区与容量保持不变，容器仍然可用。
 *       增长策略（按多少倍扩）属于容器自身，不在底座。
 */
static inline bool lystore_realloc(LyStore_t *store, unsigned long new_len) {
    size_t bytes = 0;
    if(lystore_bytes(new_len, store->item_size, &bytes) == false) {
        return false;
    }

    void *new_buffer = realloc(store->buffer, bytes);
    if(new_buffer == NULL) {
        return false;
    }

    store->buffer = new_buffer;
    store->length = new_len;

    return true;
}

/// @brief 释放自持的成员区
/// @note 只释放不清零，调用方紧接着就会释放句柄本身
static inline void lystore_free(LyStore_t *store) {
    free(store->buffer);
}

/// @brief 解除与调用者成员区的绑定（静态构造的容器销毁时用）
static inline void lystore_unbind(LyStore_t *store) {
    store->buffer = NULL;
    store->length = 0;
    store->item_size = 0;
}

/* --------------------------------- 环形游标 -------------------------------- */
/**
 * @note
 * 队列与流都以环形缓冲为底，游标递进的算术相同，故共用。
 * 与 LyStore_t 无关：流并不持有存储描述符，仍然可以用它。
 */

/// @brief 游标递进一格并回绕
static inline size_t lyring_next(size_t index, size_t length) {
    return (index + 1) % length;
}

/* -------------------------------- 侵入式双向链 ------------------------------- */
/**
 * @note
 * 参与的节点类型必须含有名为 prev 与 next 的自指针成员。
 * head 与 tail 须为可赋值的左值（如 list->head、bucket->tail_item）。
 * 宏内多次引用参数，不要传入带副作用的表达式。
 */

/// @brief 链入头部
#define LYDLIST_LINK_FRONT(head, tail, node)                            \
    do {                                                                     \
        (node)->prev = NULL;                                                 \
        (node)->next = (head);                                               \
        if((head) != NULL) {                                                 \
            (head)->prev = (node);                                           \
        }                                                                    \
        else {                                                               \
            (tail) = (node);                                                 \
        }                                                                    \
        (head) = (node);                                                     \
    } while(0)

/// @brief 链入尾部
#define LYDLIST_LINK_REAR(head, tail, node)                             \
    do {                                                                     \
        (node)->next = NULL;                                                 \
        (node)->prev = (tail);                                               \
        if((tail) != NULL) {                                                 \
            (tail)->next = (node);                                           \
        }                                                                    \
        else {                                                               \
            (head) = (node);                                                 \
        }                                                                    \
        (tail) = (node);                                                     \
    } while(0)

/// @brief 链入 pos 之前（不会改变尾指针，故不需要 tail）
#define LYDLIST_LINK_BEFORE(head, pos, node)                            \
    do {                                                                     \
        (node)->prev = (pos)->prev;                                          \
        (node)->next = (pos);                                                \
        if((pos)->prev != NULL) {                                            \
            (pos)->prev->next = (node);                                      \
        }                                                                    \
        else {                                                               \
            (head) = (node);                                                 \
        }                                                                    \
        (pos)->prev = (node);                                                \
    } while(0)

/// @brief 链入 pos 之后（不会改变头指针，故不需要 head）
#define LYDLIST_LINK_AFTER(tail, pos, node)                             \
    do {                                                                     \
        (node)->next = (pos)->next;                                          \
        (node)->prev = (pos);                                                \
        if((pos)->next != NULL) {                                            \
            (pos)->next->prev = (node);                                      \
        }                                                                    \
        else {                                                               \
            (tail) = (node);                                                 \
        }                                                                    \
        (pos)->next = (node);                                                \
    } while(0)

/// @brief 摘链，并把节点自身的 prev/next 清空
/// @note 摘下的节点不再引用链表，可以安全地释放或重新链入别处
#define LYDLIST_UNLINK(head, tail, node)                                \
    do {                                                                     \
        if((node)->prev != NULL) {                                           \
            (node)->prev->next = (node)->next;                               \
        }                                                                    \
        else {                                                               \
            (head) = (node)->next;                                           \
        }                                                                    \
        if((node)->next != NULL) {                                           \
            (node)->next->prev = (node)->prev;                               \
        }                                                                    \
        else {                                                               \
            (tail) = (node)->prev;                                           \
        }                                                                    \
        (node)->prev = NULL;                                                 \
        (node)->next = NULL;                                                 \
    } while(0)

/* -------------------------------- 侵入式单向链 ------------------------------- */
/**
 * @note
 * 参与的节点类型必须含有名为 next 的自指针成员，用于哈希容器的冲突链。
 * 摘除采用「指向前驱 next 字段的指针」写法，省掉单独的 prev 变量与分支。
 */

/// @brief 链入槽位头部
#define LYSLIST_LINK_HEAD(head, node)                                   \
    do {                                                                     \
        (node)->next = (head);                                               \
        (head) = (node);                                                     \
    } while(0)

/// @brief 摘链，ref 为指向「前驱的 next 字段」或「槽位头指针」的指针
#define LYSLIST_UNLINK(ref, node)                                       \
    do {                                                                     \
        *(ref) = (node)->next;                                               \
        (node)->next = NULL;                                                 \
    } while(0)

#endif        /* LYDSS_PRIV_H */
