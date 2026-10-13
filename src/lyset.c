/**
 * @file lyset.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief 链式哈希集合
 * @date 2026-08-10
 *
 * @copyright Copyright (c) 2026
 *
 * @note 槽位数固定、不 rehash；类型语义由调用者的 hash/equals 回调提供。
 */

#include "../inc/lydss_api.h"

#if LYDSS_SET_ENABLED

#include "../inc/lydss_priv.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

struct LySet {
    size_t size;                    ///< 槽位数量
    size_t elem_size;               ///< 单个元素的大小（单位：字节）
    size_t count;                   ///< 当前元素个数
    Hash_t *hash;                   ///< 哈希函数
    Equals_t *equals;               ///< 比较函数

    struct ElemNode {
        void *element;              ///< 元素副本
        struct ElemNode *next;      ///< 同槽位的后继节点
    } *elem_node[];                 ///< 槽位数组
};

static unsigned set_hash_default(const void *data, size_t len);

/// @brief 初始化一个集合
LySet_t *lyset_init(size_t size, size_t elem_size, Hash_t *hash, Equals_t *equals)
{
    LYDSS_ASSERT(elem_size);
    LYDSS_ASSERT(equals);

    LySet_t *set = calloc(1, sizeof(LySet_t) + size * sizeof(struct ElemNode *));
    if(set == NULL) {
        return NULL;
    }

    set->size = size;
    set->elem_size = elem_size;
    set->count = 0;
    set->hash = hash != NULL ? hash : set_hash_default;
    set->equals = equals;

    return set;
}

/// @brief 向集合中添加元素
bool lyset_add(LySet_t *set, const void *elem)
{
    LYDSS_ASSERT(set);

    if(elem == NULL) {
        return false;
    }

    size_t idx = set->hash(elem, set->elem_size) % set->size;
    for(struct ElemNode *node = set->elem_node[idx]; node != NULL; node = node->next) {
        if(set->equals(node->element, elem)) {
            return false;
        }
    }

    struct ElemNode *node = malloc(sizeof(struct ElemNode));
    if(node == NULL) {
        return false;
    }

    node->element = malloc(set->elem_size);
    if(node->element == NULL) {
        free(node);
        return false;
    }

    memcpy(node->element, elem, set->elem_size);
    LYSLIST_LINK_HEAD(set->elem_node[idx], node);
    set->count++;

    return true;
}

/// @brief 若集合中存在该元素则移除
bool lyset_remove(LySet_t *set, const void *elem)
{
    LYDSS_ASSERT(set);

    if(elem == NULL) {
        return false;
    }

    size_t idx = set->hash(elem, set->elem_size) % set->size;
    struct ElemNode **pp_node = &set->elem_node[idx];
    while(*pp_node != NULL) {
        if(set->equals((*pp_node)->element, elem)) {
            struct ElemNode *del = *pp_node;
            LYSLIST_UNLINK(pp_node, del);
            free(del->element);
            free(del);
            set->count--;

            return true;
        }
        pp_node = &(*pp_node)->next;
    }

    return false;
}

/// @brief 从集合移除并返回任意一个元素
bool lyset_pop(LySet_t *set, void *out)
{
    LYDSS_ASSERT(set);
    LYDSS_ASSERT(out);

    for(size_t i = 0; i < set->size; i++) {
        if(set->elem_node[i] != NULL) {
            struct ElemNode *node = set->elem_node[i];
            memcpy(out, node->element, set->elem_size);
            LYSLIST_UNLINK(&set->elem_node[i], node);
            free(node->element);
            free(node);
            set->count--;

            return true;
        }
    }

    return false;
}

/// @brief 移除指定容器里所有的元素
void lyset_erase(LySet_t *set)
{
    LYDSS_ASSERT(set);

    for(size_t i = 0; i < set->size; i++) {
        struct ElemNode *node = set->elem_node[i];
        while(node != NULL) {
            struct ElemNode *next = node->next;
            free(node->element);
            free(node);
            node = next;
        }
        set->elem_node[i] = NULL;
    }
    set->count = 0;
}

/// @brief 销毁指定的集合
void lyset_destroy(LySet_t *set)
{
    LYDSS_ASSERT(set);

    for(size_t i = 0; i < set->size; i++) {
        struct ElemNode *node = set->elem_node[i];
        while(node != NULL) {
            struct ElemNode *next = node->next;
            free(node->element);
            free(node);
            node = next;
        }
    }
    free(set);
}

/// @brief 得到集合的元素数量
size_t lyset_count(const LySet_t *set)
{
    LYDSS_ASSERT(set);

    return set->count;
}

/// @brief 判断集合中是否包含指定的元素
bool lyset_contains(const LySet_t *set, const void *elem)
{
    LYDSS_ASSERT(set);

    if(elem == NULL) {
        return false;
    }

    size_t idx = set->hash(elem, set->elem_size) % set->size;
    for(struct ElemNode *node = set->elem_node[idx]; node != NULL; node = node->next) {
        if(set->equals(node->element, elem)) {
            return true;
        }
    }

    return false;
}

/* --------------------------------- static --------------------------------- */
/// @brief 默认哈希函数（FNV-1a），按字节处理元素
static unsigned set_hash_default(const void *data, size_t len)
{
    const unsigned char *byte = data;
    unsigned hash = 2166136261u;

    for(size_t i = 0; i < len; i++) {
        hash ^= byte[i];
        hash *= 16777619u;
    }

    return hash;
}

#endif /* LYDSS_SET_ENABLED */
