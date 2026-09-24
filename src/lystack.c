/**
 * @file lystack.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief 定长栈
 * @date 2026-08-18
 *
 * @copyright Copyright (c) 2026
 */

#include "../inc/lydss_api.h"

#if LYDSS_STACK_ENABLED

#include "../inc/lydss_priv.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

struct LyStack {
    LyStore_t store;            ///< 栈数据描述（地址、容量、成员大小）
    unsigned long top;          ///< 栈顶索引（当前成员数量）
    bool is_static;             ///< 栈数据是否由调用者提供
};

/// @brief 创建一个栈
LyStack_t *lystack_create(unsigned long len, unsigned long item_size)
{
    LYDSS_ASSERT(item_size);

    LyStack_t *stack = malloc(sizeof(LyStack_t));
    if(stack == NULL) {
        return NULL;
    }

    if(lystore_alloc(&stack->store, len, item_size, true) == false) {
        free(stack);
        return NULL;
    }

    stack->top = 0;
    stack->is_static = false;

    return stack;
}

/// @brief 创建一个静态栈
void lystack_create_static(LyStack_t *stack, void *buf, unsigned long len, unsigned long item_size)
{
    LYDSS_ASSERT(stack);
    LYDSS_ASSERT(buf);
    LYDSS_ASSERT(item_size);

    lystore_bind(&stack->store, buf, len, item_size);
    stack->top = 0;
    stack->is_static = true;
}

/// @brief 数据入栈
bool lystack_push(LyStack_t *stack, void *item)
{
    LYDSS_ASSERT(stack);
    LYDSS_ASSERT(item);

    if(stack->top >= stack->store.length) {
        return false;
    }
    memcpy(lystore_address(&stack->store, stack->top), item, stack->store.item_size);
    stack->top++;

    return true;
}

/// @brief 数据出栈
bool lystack_pop(LyStack_t *stack, void *buf)
{
    LYDSS_ASSERT(stack);
    LYDSS_ASSERT(buf);

    if(stack->top == 0) {
        return false;
    }
    stack->top--;
    memcpy(buf, lystore_address(&stack->store, stack->top), stack->store.item_size);
    return true;
}

/// @brief 预读栈顶数据
bool lystack_peek(LyStack_t *stack, void *buf)
{
    LYDSS_ASSERT(stack);
    LYDSS_ASSERT(buf);

    if(stack->top == 0) {
        return false;
    }
    memcpy(buf, lystore_address(&stack->store, stack->top - 1), stack->store.item_size);

    return true;
}

/// @brief 清空栈
bool lystack_clear(LyStack_t *stack)
{
    LYDSS_ASSERT(stack);

    stack->top = 0;

    return true;
}

/// @brief 删除栈
void lystack_delete(LyStack_t *stack)
{
    LYDSS_ASSERT(stack);

    if(stack->is_static == true) {
        lystore_unbind(&stack->store);
        stack->top = 0;
        return;
    }

    lystore_free(&stack->store);
    free(stack);
}

/// @brief 获取栈中当前的成员个数
unsigned long lystack_count(LyStack_t *stack)
{
    LYDSS_ASSERT(stack);

    return stack->top;
}

/// @brief 判断栈是否为空
bool lystack_is_empty(LyStack_t *stack)
{
    LYDSS_ASSERT(stack);
    return stack->top == 0;
}

/// @brief 判断栈是否已满
bool lystack_is_full(LyStack_t *stack)
{
    LYDSS_ASSERT(stack);

    return stack->top >= stack->store.length;
}

#endif /* LYDSS_STACK_ENABLED */
