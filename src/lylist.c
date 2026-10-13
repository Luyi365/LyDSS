/**
 * @file lylist.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief 双向链表容器
 * @date 2026-08-14
 *
 * @copyright Copyright (c) 2026
 *
 * @note destroy 只释放节点，不释放 data，也不释放 list 句柄本身（可能是栈分配的）。
 */

#include "../inc/lydss_api.h"

#if LYDSS_LIST_ENABLED

#include "../inc/lydss_priv.h"

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
    LYDLIST_LINK_FRONT(list->head, list->tail, new_node);

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
    LYDLIST_LINK_REAR(list->head, list->tail, new_node);

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
    LYDLIST_LINK_BEFORE(list->head, node, new_node);

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
    LYDLIST_LINK_AFTER(list->tail, node, new_node);

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

    LYDLIST_UNLINK(list->head, list->tail, node);
    free(node);

    return node_data;
}

#endif /* LYDSS_LIST_ENABLED */
