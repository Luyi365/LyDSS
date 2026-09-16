/**
 * @file lystack.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief
 * @date 2026-08-18
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


struct LyStack {
    void *buffer;               ///< 栈数据
    unsigned long length;       ///< 栈可容纳的成员数量
    unsigned long item_size;    ///< 单个成员的大小（单位：字节）
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

    stack->buffer = calloc(len, item_size);
    if(stack->buffer == NULL) {
        free(stack);
        return NULL;
    }

    stack->length = len;
    stack->item_size = item_size;
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

    stack->length = len;
    stack->item_size = item_size;
    stack->top = 0;
    stack->is_static = true;
    stack->buffer = buf;
}

/// @brief 数据入栈
bool lystack_push(LyStack_t *stack, void *item)
{
    LYDSS_ASSERT(stack);
    LYDSS_ASSERT(item);

    if(stack->top >= stack->length) {
        return false;
    }
    memcpy((unsigned char *)stack->buffer + stack->top * stack->item_size, item, stack->item_size);
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
    memcpy(buf, (unsigned char *)stack->buffer + stack->top * stack->item_size, stack->item_size);
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
    memcpy(buf, (unsigned char *)stack->buffer + (stack->top - 1) * stack->item_size, stack->item_size);

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
/// @note 静态栈只解除栈数据绑定，不释放调用者提供的内存与句柄
void lystack_delete(LyStack_t *stack)
{
    LYDSS_ASSERT(stack);

    if(stack->is_static == true) {
        stack->buffer = NULL;
        stack->top = 0;
        return;
    }

    free(stack->buffer);
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

    return stack->top >= stack->length;
}