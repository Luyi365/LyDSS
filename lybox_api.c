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
/**
 * @brief 创建一个队列
 * 
 * @param queue_len 队列长度（最大成员数）
 * @param item_size 目标大小
 * @return queue_t* 返回一个队列对象
 */
queue_t *queue_create(uint8_t queue_len, uint8_t item_size)
{
    return lybox_create(queue_len, item_size, LyboxReadMode_FIFO, false);
}

/**
 * @brief 销毁队列
 * 
 * @param queue 队列对象
 */
void queue_delete(queue_t *queue)
{
    lybox_delete(queue);
}

/**
 * @brief 向队列发送单个数据
 * 
 * @param queue 队列对象
 * @param pval 数据指针
 * @return bool 成功返回true，失败返回false
 */
bool queue_send(queue_t *queue, void *pval)
{
    return !!lybox_send(queue, pval, 1);
}

/**
 * @brief 向队列发送多个数据
 * 
 * @param queue 队列对象
 * @param pval 数据指针
 * @param num 数据个数
 * @return uint8_t 返回实际发送的数量
 */
uint8_t queue_send_multi(queue_t *queue, void *pval, size_t num)
{
    return lybox_send(queue, pval, num);
}

/**
 * @brief 向队列接收单个数据
 * 
 * @param queue 队列对象
 * @param pbuff 缓存指针
 * @return bool 成功返回true，失败返回false
 */
bool queue_receive(queue_t *queue, void *pbuff)
{
    return !!lybox_receive(queue, pbuff, 1);
}

/**
 * @brief 向队列接收多个数据
 * 
 * @param queue 队列对象
 * @param pbuff 缓存指针
 * @param num 数据个数
 * @return uint8_t 返回实际接收的数量
 */
uint8_t queue_receive_multi(queue_t *queue, void *pbuff, size_t num)
{
    return lybox_receive(queue, pbuff, num);
}

/**
 * @brief 预读队列单个数据
 * 
 * @param queue 队列对象
 * @param pbuff 缓存指针
 * @return bool 成功返回true，失败返回false
 */
bool queue_peek(queue_t *queue, void *pbuff)
{
    return !!lybox_peek(queue, pbuff, 1);
}

/**
 * @brief 预读队列多个数据
 * 
 * @param queue 队列对象
 * @param pbuff 缓存指针
 * @param num 数据个数
 * @return uint8_t 返回实际读到的数量
 */
uint8_t queue_peek_multi(queue_t *queue, void *pbuff, size_t num)
{
    return lybox_peek(queue, pbuff, num);
}


/* ------------------------------------ 栈 ----------------------------------- */
/**
 * @brief 创建一个栈
 * 
 * @param stack_len 栈深度（最大成员数）
 * @param item_size 目标大小
 * @return stack_t* 返回一个栈对象
 */
stack_t *stack_create(uint8_t stack_len, uint8_t item_size)
{
    return lybox_create(stack_len, item_size, LyboxReadMode_LIFO_BLOCK, false);
}

/**
 * @brief 销毁队列
 * 
 * @param stack 栈对象
 */
void stack_delete(stack_t *stack)
{
    lybox_delete(stack);
}

/**
 * @brief 向栈压入单个数据
 * 
 * @param stack 栈对象
 * @param pval 数据指针
 * @return bool 成功返回true，失败返回false
 */
bool stack_push(stack_t *stack, void *pval)
{
    return !!lybox_send(stack, pval, 1);
}

/**
 * @brief 向栈压入多个数据
 * 
 * @param stack 栈对象
 * @param pval 数据指针
 * @param num 数据个数
 * @return uint8_t 返回实际压入的数量
 */
uint8_t stack_push_multi(stack_t *stack, void *pval, size_t num)
{
    return lybox_send(stack, pval, num);
}

/**
 * @brief 向栈弹出单个数据
 * 
 * @param stack 栈对象
 * @param pbuff 缓存指针
 * @return bool 成功返回true，失败返回false
 */
bool stack_pop(stack_t *stack, void *pbuff)
{
    return !!lybox_receive(stack, pbuff, 1);
}

/**
 * @brief 向栈弹出多个数据
 * 
 * @param stack 栈对象
 * @param pval 数据指针
 * @param num 数据个数
 * @return uint8_t 返回实际弹出的数量
 */
uint8_t stack_pop_multi(stack_t *stack, void *pbuff, size_t num)
{
    return lybox_receive(stack, pbuff, num);
}


/* ------------------------------------ 流 ----------------------------------- */
/**
 * @brief 创建一个流
 * 
 * @param bytes 可容纳的总字节数
 * @return stream_t* 返回一个流对象
 */
stream_t *stream_create(size_t bytes_size)
{
    return lybox_create(bytes_size, sizeof(byte_t), LyboxReadMode_LIFO_BYTE, true);
}

/**
 * @brief 销毁流
 * 
 * @param stream 流对象
 */
void stream_delete(stream_t *stream)
{
    lybox_delete(stream);
}

/**
 * @brief 向流发送多个字节
 * 
 * @param stream 流对象
 * @param pval 数据指针
 * @param num 字节数量
 * @return uint8_t 返回实际发送的数量
 */
uint8_t stream_send(stream_t *stream, void *pval, size_t num)
{
    return lybox_send(stream, pval, num);
}

/**
 * @brief 向流接收多个字节
 * 
 * @param stream 流对象
 * @param pbuff 缓存指针
 * @param num 字节数量
 * @return uint8_t 返回实际接收的数量
 */
uint8_t stream_receive(stream_t *stream, void *pbuff, size_t num)
{
    return lybox_receive(stream, pbuff, num);
}


/* --------------------------------- 事件/标志组 --------------------------------- */
/**
 * @brief 创建一个事件组
 * 
 * @return event_group_t* 返回一个事件组对象
 */
event_group_t *event_group_create(void)
{
    return lybox_create(1, sizeof(event_bits_t), LyboxReadMode_FIFO, false);
}

/**
 * @brief 销毁事件组
 * 
 * @brief event_group 事件组对象
 */
void event_group_delete(event_group_t *event_group)
{
    lybox_delete(event_group);
}

/**
 * @brief 事件组中设置位（标志）
 * 
 * @brief event_group 事件组对象
 * @brief bits 按位值
 * @return event_bits_t 执行该函数后事件组的值
 */
event_bits_t event_group_set(event_group_t *event_group, const event_bits_t bits)
{
    return *(event_bits_t *)(event_group->box) |= bits;
}

/**
 * @brief 事件组中清除位（标志）
 * 
 * @brief event_group 事件组对象
 * @brief bits 按位值
 * @return event_bits_t 执行该函数后事件组的值
 */
event_bits_t event_group_clear(event_group_t *event_group, const event_bits_t bits)
{
    return *(event_bits_t *)(event_group->box) &= ~bits;
}

/**
 * @brief 得到事件组的当前值
 * 
 * @brief event_group 事件组对象
 * @return event_bits_t 事件组的值
 */
event_bits_t event_group_get(event_group_t *event_group)
{
    return *(event_bits_t *)(event_group->box);
}

