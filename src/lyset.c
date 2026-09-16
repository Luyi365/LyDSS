/**
 * @file lyset.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief
 * @date 2026-08-10
 *
 * @copyright Copyright (c) 2026
 *
 * @note
 * Matters related to this document.
 */

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "../inc/lydss_api.h"

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

/// @brief 初始化一个集合
LySet_t *lyset_init(size_t size, size_t elem_size, Hash_t *hash, Equals_t *equals)
{
    LYDSS_ASSERT(elem_size);
    LYDSS_ASSERT(hash);
    LYDSS_ASSERT(equals);

    LySet_t *set = calloc(1, sizeof(LySet_t) + size * sizeof(struct ElemNode *));
    if(set == NULL) {
        return NULL;
    }

    set->size = size;
    set->elem_size = elem_size;
    set->count = 0;
    set->hash = hash;
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
    node->next = set->elem_node[idx];
    set->elem_node[idx] = node;
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
            *pp_node = del->next;
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
            set->elem_node[i] = node->next;
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

/// @brief 判断两个集合的关系
LySetRelation_t lyset_relation(const LySet_t *self, const LySet_t *other)
{
    LYDSS_ASSERT(self);
    LYDSS_ASSERT(other);

    bool self_in_other = true;
    bool other_in_self = true;
    bool has_inter = false;

    for(size_t i = 0; i < self->size; i++) {
        for(struct ElemNode *node = self->elem_node[i]; node != NULL; node = node->next) {
            if(lyset_contains(other, node->element)) {
                has_inter = true;
            }
            else {
                self_in_other = false;
            }
        }
    }

    for(size_t i = 0; i < other->size; i++) {
        for(struct ElemNode *node = other->elem_node[i]; node != NULL; node = node->next) {
            if(!lyset_contains(self, node->element)) {
                other_in_self = false;
                break;
            }
        }
    }

    if(self_in_other && other_in_self) {
        return LySetRelation_SAME;
    }
    if(self_in_other) {
        return LySetRelation_PROSUB;
    }
    if(other_in_self) {
        return LySetRelation_PROSUP;
    }
    if(has_inter) {
        return LySetRelation_INTER;
    }

    return LySetRelation_NONE;
}


