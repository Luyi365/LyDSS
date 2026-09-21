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

#include "../inc/lydss_api.h"

#if LYDSS_LIST_ENABLED

#include <stdlib.h>

/// @brief 创建链表
LyList_t *lylist_create(void)
{
    LyList_t *new_list = (LyList_t *)malloc(sizeof(LyList_t));
    if(new_list == NULL) {
        return NULL;
    }

    new_list->head = NULL;
    new_list->tail = NULL;

    return new_list;
}

/// @brief 销毁链表
/// @note 只释放节点，不释放 data 指向的业务对象，也不释放 list 句柄本身
void lylist_destroy(LyList_t *list)
{
    LYDSS_ASSERT(list);

    LyListNode_t *cur_node = list->head;
    while(cur_node != NULL) {
        LyListNode_t *next_node = cur_node->next;
        free(cur_node);
        cur_node = next_node;
    }
    list->head = list->tail = NULL;
}

/// @brief 链表头部插入节点
LyListNode_t *lylist_push_front(LyList_t *list, void *data)
{
    LYDSS_ASSERT(list);

    LyListNode_t *new_node = calloc(1, sizeof(LyListNode_t));
    if(new_node == NULL) {
        return NULL;
    }

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
    LYDSS_ASSERT(list);

    LyListNode_t *new_node = calloc(1, sizeof(LyListNode_t));
    if(new_node == NULL) {
        return NULL;
    }

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
    LYDSS_ASSERT(list);
    LYDSS_ASSERT(node);

    LyListNode_t *new_node = calloc(1, sizeof(LyListNode_t));
    if(new_node == NULL) {
        return NULL;
    }

    new_node->data = data;
    new_node->next = node;
    new_node->prev = node->prev;
    if(node->prev != NULL) {
        node->prev->next = new_node;
    }
    else {
        list->head = new_node;
    }
    node->prev = new_node;

    return new_node;
}

/// @brief 在节点后插入
LyListNode_t *lylist_insert_after(LyList_t *list, LyListNode_t *node, void *data)
{
    LYDSS_ASSERT(list);
    LYDSS_ASSERT(node);

    LyListNode_t *new_node = calloc(1, sizeof(LyListNode_t));
    if(new_node == NULL) {
        return NULL;
    }

    new_node->data = data;
    new_node->prev = node;
    new_node->next = node->next;
    if(node->next != NULL) {
        node->next->prev = new_node;
    }
    else {
        list->tail = new_node;
    }
    node->next = new_node;

    return new_node;
}

/// @brief 弹出链表头部节点
void *lylist_pop_front(LyList_t *list)
{
    LYDSS_ASSERT(list);

    if(list->head == NULL) {
        return NULL;
    }

    return lylist_remove(list, list->head);
}

/// @brief 弹出链表尾部节点
void *lylist_pop_rear(LyList_t *list)
{
    LYDSS_ASSERT(list);

    if(list->tail == NULL) {
        return NULL;
    }

    return lylist_remove(list, list->tail);
}

/// @brief 移除节点
void *lylist_remove(LyList_t *list, LyListNode_t *node)
{
    LYDSS_ASSERT(list);
    LYDSS_ASSERT(node);

    void *node_data = node->data;
    if(node->prev != NULL) {
        node->prev->next = node->next;
    }
    else {
        list->head = node->next;
    }

    if(node->next != NULL) {
        node->next->prev = node->prev;
    }
    else {
        list->tail = node->prev;
    }
    free(node);

    return node_data;
}

/// @brief 交换节点
void lylist_swap(LyListNode_t *a, LyListNode_t *b)
{
    LYDSS_ASSERT(a);
    LYDSS_ASSERT(b);

    LyListNode_t *tmp = a->prev;
    a->prev = b->prev;
    b->prev = tmp;
    tmp = a->next;
    a->next = b->next;
    b->next = tmp;

    if(a->prev != NULL) {
        a->prev->next = a;
    }
    if(a->next != NULL) {
        a->next->prev = a;
    }
    if(b->prev != NULL) {
        b->prev->next = b;
    }
    if(b->next != NULL) {
        b->next->prev = b;
    }
}

#endif /* LYDSS_LIST_ENABLED */
