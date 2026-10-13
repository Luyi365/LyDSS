/**
 * @file lydss_priv.h
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief LyDSS 内部底座层
 * @date 2026-09-16
 *
 * @copyright Copyright (c) 2026 Luyi365
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @note
 * 容器共用的内部底座，非公开 API：不要在库外包含，也不要移入 lydss_api.h。
 * 必须保持全部 static inline，否则容器间会产生跨库依赖，功能开关将无法自由组合。
 * 不做参数断言：调用方进入前已校验。
 */

#ifndef LYDSS_PRIV_H
#define LYDSS_PRIV_H

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* --------------------------------- 连续存储 -------------------------------- */
/// @note 由数组、动态数组、乒乓缓冲、栈、队列按值嵌入，成员名统一为 store。
///       不含 is_static，以免动态数组白占填充、改变句柄尺寸。
typedef struct LyStore {
    void *buffer;               ///< 成员区
    size_t length;              ///< 成员区可容纳的成员数量
    size_t item_size;           ///< 单个成员的大小（单位：字节）
} LyStore_t;

/// @brief 计算 count*item_size 字节数，溢出返回 false
static inline bool lystore_bytes(size_t count, size_t item_size, size_t *bytes) {
    if(item_size != 0 && count > SIZE_MAX / item_size) {
        return false;
    }

    *bytes = count * item_size;

    return true;
}

/// @brief 计算指定下标成员的地址
static inline void *lystore_address(LyStore_t *store, size_t index) {
    return (unsigned char *)store->buffer + index * store->item_size;
}

/// @brief 绑定调用者提供的成员区
static inline void lystore_bind(LyStore_t *store, void *buf, size_t len, size_t item_size) {
    store->buffer = buf;
    store->length = len;
    store->item_size = item_size;
}

/// @brief 分配自持的成员区，zeroed 为真则清零；失败时不改动 store
static inline bool lystore_alloc(LyStore_t *store, size_t len, size_t item_size, bool zeroed) {
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

/// @brief 重分配成员区并更新容量；失败时原区不变，增长策略归容器
static inline bool lystore_realloc(LyStore_t *store, size_t new_len) {
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
static inline void lystore_free(LyStore_t *store) {
    free(store->buffer);
}

/// @brief 解绑调用者提供的成员区，静态容器销毁时用
static inline void lystore_unbind(LyStore_t *store) {
    store->buffer = NULL;
    store->length = 0;
    store->item_size = 0;
}

/* --------------------------------- 环形游标 -------------------------------- */

/// @brief 游标递进一格并回绕
static inline size_t lyring_next(size_t index, size_t length) {
    return (index + 1) % length;
}

/* -------------------------------- 侵入式双向链 ------------------------------- */
/// @note 节点须含 prev/next 成员，head/tail 须为左值；参数会被多次求值，勿带副作用。

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

/// @note 不改动尾指针，故不取 tail
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

/// @note 不改动头指针，故不取 head
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

/// @note 摘链后清空节点自身 prev/next，使其可安全释放或重新链入别处
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
/// @note 用于哈希容器的冲突链，节点须含 next 成员。

#define LYSLIST_LINK_HEAD(head, node)                                   \
    do {                                                                     \
        (node)->next = (head);                                               \
        (head) = (node);                                                     \
    } while(0)

/// @note ref 指向前驱的 next 字段或槽位头指针
#define LYSLIST_UNLINK(ref, node)                                       \
    do {                                                                     \
        *(ref) = (node)->next;                                               \
        (node)->next = NULL;                                                 \
    } while(0)

#endif        /* LYDSS_PRIV_H */
