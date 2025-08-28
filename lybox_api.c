/**
 * @file lybox_api.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief
 * @date 2025-07-30
 *
 * @copyright Copyright (c) 2025 Luyi365
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @note
 * 
 *                                
 */

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "lybox_api.h"

/* ----------------------------------- 队列 ----------------------------------- */
/// @brief 创建一个队列
queue_t *queue_create(uint8_t queue_len, uint8_t item_size)
{
    return lybox_create(queue_len, item_size);
}

/// @brief 销毁队列
void queue_delete(queue_t *queue)
{
    lybox_delete(queue);
}

/// @brief 向队列发送单个数据
bool queue_send(queue_t *queue, void *pval)
{
    return !!lybox_send(queue, pval, 1, LyboxWriteMode_Default);
}

/// @brief 向队列发送多个数据
uint8_t queue_send_multi(queue_t *queue, void *pval, size_t num)
{
    return lybox_send(queue, pval, num, LyboxWriteMode_Default);
}

/// @brief 从队列接收单个数据
bool queue_receive(queue_t *queue, void *pbuff)
{
    return !!lybox_receive(queue, pbuff, 1, LyboxReadMode_FIFO);
}

/// @brief 从队列接收多个数据
uint8_t queue_receive_multi(queue_t *queue, void *pbuff, size_t num)
{
    return lybox_receive(queue, pbuff, num, LyboxReadMode_FIFO);
}

/// @brief 预读队列单个数据
bool queue_peek(queue_t *queue, void *pbuff)
{
    return !!lybox_peek(queue, pbuff, 1, LyboxReadMode_FIFO);
}

/// @brief 预读队列多个数据
uint8_t queue_peek_multi(queue_t *queue, void *pbuff, size_t num)
{
    return lybox_peek(queue, pbuff, num, LyboxReadMode_FIFO);
}


/* ------------------------------------ 栈 ----------------------------------- */
/// @brief 创建一个栈
stack_t *stack_create(uint8_t stack_len, uint8_t item_size)
{
    return lybox_create(stack_len, item_size);
}

/// @brief 销毁队列
void stack_delete(stack_t *stack)
{
    lybox_delete(stack);
}

/// @brief 向栈压入单个数据
bool stack_push(stack_t *stack, void *pval)
{
    return !!lybox_send(stack, pval, 1, LyboxWriteMode_Default);
}

/// @brief 向栈压入多个数据
uint8_t stack_push_multi(stack_t *stack, void *pval, size_t num)
{
    return lybox_send(stack, pval, num, LyboxWriteMode_Default);
}

/// @brief 从栈弹出单个数据
bool stack_pop(stack_t *stack, void *pbuff)
{
    return !!lybox_receive(stack, pbuff, 1, LyboxReadMode_LIFOBlock);
}

/// @brief 从栈弹出多个数据
uint8_t stack_pop_multi(stack_t *stack, void *pbuff, size_t num)
{
    return lybox_receive(stack, pbuff, num, LyboxReadMode_LIFOBlock);
}


/* ------------------------------------ 流 ----------------------------------- */
/// @brief 创建一个流
stream_t *stream_create(size_t bytes_size)
{
    return lybox_create(bytes_size, sizeof(byte_t));
}

/// @brief 销毁流
void stream_delete(stream_t *stream)
{
    lybox_delete(stream);
}

/// @brief 向流发送多个字节
uint8_t stream_send(stream_t *stream, void *pval, size_t num)
{
    return lybox_send(stream, pval, num, LyboxWriteMode_Fresh);
}

/// @brief 向流接收多个字节
uint8_t stream_receive(stream_t *stream, void *pbuff, size_t num)
{
    return lybox_receive(stream, pbuff, num, LyboxReadMode_LIFOUnit);
}


/* --------------------------------- 事件/标志组 --------------------------------- */
/// @brief 创建一个事件组
event_group_t *event_group_create(void)
{
    return lybox_create(1, sizeof(event_bits_t));
}

/// @brief 销毁事件组
void event_group_delete(event_group_t *event_group)
{
    lybox_delete(event_group);
}

/// @brief 事件组中设置位（标志）
event_bits_t event_group_set(event_group_t *event_group, const event_bits_t bits)
{
    return *(event_bits_t *)(event_group->box) |= bits;
}

/// @brief 事件组中清除位（标志）
event_bits_t event_group_clear(event_group_t *event_group, const event_bits_t bits)
{
    return *(event_bits_t *)(event_group->box) &= ~bits;
}

/// @brief 得到事件组的当前值
event_bits_t event_group_get(event_group_t *event_group)
{
    return *(event_bits_t *)(event_group->box);
}

