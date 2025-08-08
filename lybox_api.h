/**
 * @file lybox_api.h
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

#ifndef __LYBOX_API__
#define __LYBOX_API__

#include <stdint.h> 
#include <stdbool.h>
#include "lybox.h"


/* ----------------------------------- 队列 ----------------------------------- */
typedef lybox_t queue_t;

/**
 * @brief 创建一个队列
 * 
 * @param queue_len 队列长度（最大成员数）
 * @param item_size 目标大小
 * @return queue_t* 返回一个队列对象
 */
queue_t *queue_create(uint8_t queue_len, uint8_t item_size);

/**
 * @brief 销毁队列
 * 
 * @param queue 队列对象
 */
void queue_delete(queue_t *queue);

/**
 * @brief 向队列发送单个数据
 * 
 * @param queue 队列对象
 * @param pval 数据指针
 * @return bool 成功返回true，失败返回false
 */
bool queue_send(queue_t *queue, void *pval);

/**
 * @brief 向队列发送多个数据
 * 
 * @param queue 队列对象
 * @param pval 数据指针
 * @param num 数据个数
 * @return uint8_t 返回实际发送的数量
 */
uint8_t queue_send_multi(queue_t *queue, void *pval, size_t num);

/**
 * @brief 向队列接收单个数据
 * 
 * @param queue 队列对象
 * @param pbuff 缓存指针
 * @return bool 成功返回true，失败返回false
 */
bool queue_receive(queue_t *queue, void *pbuff);

/**
 * @brief 向队列接收多个数据
 * 
 * @param queue 队列对象
 * @param pbuff 缓存指针
 * @param num 数据个数
 * @return uint8_t 返回实际接收的数量
 */
uint8_t queue_receive_multi(queue_t *queue, void *pbuff, size_t num);

/**
 * @brief 预读队列单个数据
 * 
 * @param queue 队列对象
 * @param pbuff 缓存指针
 * @return bool 成功返回true，失败返回false
 */
bool queue_peek(queue_t *queue, void *pbuff);

/**
 * @brief 预读队列多个数据
 * 
 * @param queue 队列对象
 * @param pbuff 缓存指针
 * @param num 数据个数
 * @return uint8_t 返回实际读到的数量
 */
uint8_t queue_peek_multi(queue_t *queue, void *pbuff, size_t num);


/* ----------------------------------- 栈 ----------------------------------- */
typedef lybox_t stack_t;

/**
 * @brief 创建一个栈
 * 
 * @param stack_len 栈深度（最大成员数）
 * @param item_size 目标大小
 * @return stack_t* 返回一个栈对象
 */
stack_t *stack_create(uint8_t stack_len, uint8_t item_size);

/**
 * @brief 销毁队列
 * 
 * @param stack 栈对象
 */
void stack_delete(stack_t *stack);

/**
 * @brief 向栈压入单个数据
 * 
 * @param stack 栈对象
 * @param pval 数据指针
 * @return bool 成功返回true，失败返回false
 */
bool stack_push(stack_t *stack, void *pval);

/**
 * @brief 向栈压入多个数据
 * 
 * @param stack 栈对象
 * @param pval 数据指针
 * @param num 数据个数
 * @return uint8_t 返回实际压入的数量
 */
uint8_t stack_push_multi(stack_t *stack, void *pval, size_t num);

/**
 * @brief 向栈弹出单个数据
 * 
 * @param stack 栈对象
 * @param pbuff 缓存指针
 * @return bool 成功返回true，失败返回false
 */
bool stack_pop(stack_t *stack, void *pbuff);

/**
 * @brief 向栈弹出多个数据
 * 
 * @param stack 栈对象
 * @param pval 数据指针
 * @param num 数据个数
 * @return uint8_t 返回实际弹出的数量
 */
uint8_t stack_pop_multi(stack_t *stack, void *pbuff, size_t num);


/* ----------------------------------- 流 ----------------------------------- */
typedef lybox_t stream_t;
typedef uint8_t byte_t;

/**
 * @brief 创建一个流
 * 
 * @param bytes 可容纳的总字节数
 * @return stream_t* 返回一个流对象
 */
stream_t *stream_create(size_t bytes_size);

/**
 * @brief 销毁流
 * 
 * @param stream 流对象
 */
void stream_delete(stream_t *stream);

/**
 * @brief 向流发送多个字节
 * 
 * @param stream 流对象
 * @param pval 数据指针
 * @param num 字节数量
 * @return uint8_t 返回实际发送的数量
 */
uint8_t stream_send(stream_t *stream, void *pval, size_t num);

/**
 * @brief 向流接收多个字节
 * 
 * @param stream 流对象
 * @param pbuff 缓存指针
 * @param num 字节数量
 * @return uint8_t 返回实际接收的数量
 */
uint8_t stream_receive(stream_t *stream, void *pbuff, size_t num);


/* --------------------------------- 事件/标志组 --------------------------------- */
#define EVENT_GROUP_USE_BITS    8
#if EVENT_GROUP_USE_BITS <= 8
typedef uint8_t event_bits_t;
#elif EVENT_GROUP_USE_BITS <= 16
typedef uint16_t event_bits_t;
#elif EVENT_GROUP_USE_BITS <= 32
typedef uint32_t event_bits_t;
#elif EVENT_GROUP_USE_BITS <= 64
typedef uint64_t event_bits_t;
#else
#warn "EVENT_GROUP_USE_BITS too large"
#endif

typedef lybox_t event_group_t;

#define EVENT_BIT_0	( 1 << 0 )
#define EVENT_BIT_1	( 1 << 1 )
#define EVENT_BIT_2	( 1 << 2 )
#define EVENT_BIT_3	( 1 << 3 )
#define EVENT_BIT_4	( 1 << 4 )
#define EVENT_BIT_5	( 1 << 5 )
#define EVENT_BIT_6	( 1 << 6 )
#define EVENT_BIT_7	( 1 << 7 )
#define EVENT_BIT_x(n)	( 1 << n )

/**
 * @brief 创建一个事件组
 * 
 * @return event_group_t* 返回一个事件组对象
 */
event_group_t *event_group_create(void);

/**
 * @brief 销毁事件组
 * 
 * @brief event_group 事件组对象
 */
void event_group_delete(event_group_t *event_group);

/**
 * @brief 事件组中设置位（标志）
 * 
 * @brief event_group 事件组对象
 * @brief bits 按位值
 * @return event_bits_t 执行该函数后事件组的值
 */
event_bits_t event_group_set(event_group_t *event_group, const event_bits_t bits);

/**
 * @brief 事件组中清除位（标志）
 * 
 * @brief event_group 事件组对象
 * @brief bits 按位值
 * @return event_bits_t 执行该函数后事件组的值
 */
event_bits_t event_group_clear(event_group_t *event_group, const event_bits_t bits);

/**
 * @brief 得到事件组的当前值
 * 
 * @brief event_group 事件组对象
 * @return event_bits_t 事件组的值
 */
event_bits_t event_group_get(event_group_t *event_group);

#endif        /* __LYBOX_API__ */
