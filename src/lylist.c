/**
 * @file lylist.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief
 * @date 2026-08-14
 *
 * @copyright Copyright (c) 2026
 *
 * @note
 * Matters related to this document.
 */

#include <stdlib.h>
#include <string.h>
#include "../inc/lydds_api.h"

/// @brief 创建链表
LyList_t *lylist_creatr(void)
{
    LyList_t *new_list = (LyList_t *)malloc(sizeof(LyList_t));
    new_list->head = NULL;
    new_list->tail = NULL;

    return new_list;
}

/// @brief 销毁链表
void lylist_destroy(LyList_t *list)
{
    LYDDS_ASSERT(list);

    LyListNode_t *cur_node = list->head;
    while(cur_node)
    {
        LyListNode_t *next_node = cur_node->next;
        free(cur_node);
        cur_node = next_node;
    }
    list->head = list->tail = NULL;
}

/// @brief 链表头部插入节点
LyListNode_t *lylist_push_front(LyList_t *list, void *data)
{
    LYDDS_ASSERT(list);

    LyListNode_t *new_node = calloc(1, sizeof(LyListNode_t));
    new_node->data = data;
    if(list->head == NULL) {
        list->head = list->tail = new_node;
    }
    else {
        new_node->next = list->head;
        list->head->prev = new_node;
        list->head = new_node;
    }

    return new_node;
}

/// @brief 链表尾部插入节点
LyListNode_t *lylist_push_rear(LyList_t *list, void *data)
{
    LYDDS_ASSERT(list);

    LyListNode_t *new_node = calloc(1, sizeof(LyListNode_t));
    new_node->data = data;
    if(list->tail == NULL) {
        list->head = list->tail = new_node;
    }
    else {
        new_node->prev = list->tail;
        list->tail->next = new_node;
        list->tail = new_node;
    }

    return new_node;
}

/// @brief 在节点前插入
LyListNode_t *lylist_insert_before(LyList_t *list, LyListNode_t *node, void *data)
{
    LYDDS_ASSERT(list);
    LYDDS_ASSERT(node);

    LyListNode_t *new_node = calloc(1, sizeof(LyListNode_t));
    new_node->data = data;
    new_node->next = node;
    new_node->prev = node->prev;
    if(node->prev)
        node->prev->next = new_node;
    else
        list->head = new_node;
    node->prev = new_node;

    return new_node;
}

/// @brief 在节点后插入
LyListNode_t *lylist_insert_after(LyList_t *list, LyListNode_t *node, void *data)
{
    LYDDS_ASSERT(list);
    LYDDS_ASSERT(node);

    LyListNode_t *new_node = calloc(1, sizeof(LyListNode_t));
    new_node->data = data;
    new_node->prev = node;
    new_node->next = node->next;
    if(node->next)
        node->next->prev = new_node;
    else
        list->tail = new_node;
    node->next = new_node;

    return new_node;
}

/// @brief 弹出链表头部节点
void *lylist_pop_front(LyList_t *list)
{
    LYDDS_ASSERT(list);

    return lylist_remove(list, list->head);
}

/// @brief 弹出链表尾部节点
void *lylist_pop_rear(LyList_t *list)
{
    LYDDS_ASSERT(list);

    return lylist_remove(list, list->tail);
}

/// @brief 移除节点
void *lylist_remove(LyList_t *list, LyListNode_t *node)
{
    LYDDS_ASSERT(list);
    LYDDS_ASSERT(node);

    void *node_data = node->data;
    if(node->prev)
        node->prev->next = node->next;
    else
        list->head = node->next;
    if(node->next)
        node->next->prev = node->prev;
    else
        list->tail = node->prev;
    free(node);

    return node_data;
}

/// @brief 交换节点
void lylist_swap(LyListNode_t *a, LyListNode_t *b)
{
    LYDDS_ASSERT(a);
    LYDDS_ASSERT(b);

    LyListNode_t *tmp = a->prev;
    a->prev = b->prev;
    b->prev = tmp;
    tmp = a->next;
    a->next = b->next;
    b->next = tmp;

    if(a->prev) a->prev->next = a;
    if(a->next) a->next->prev = a;
    if(b->prev) b->prev->next = b;
    if(b->next) b->next->prev = b;
}
