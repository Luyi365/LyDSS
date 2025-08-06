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

#include "lybox.h"


/* ----------------------------------- 队列 ----------------------------------- */
typedef lybox_t queue_t;
queue_t *queue_create(uint8_t queue_len, uint8_t item_size);
bool queue_send(queue_t *queue, void *pval);
uint8_t queue_send_multi(queue_t *queue, void *pval, size_t num);
bool queue_receive(queue_t *queue, void *pbuff);
uint8_t queue_receive_multi(queue_t *queue, void *pbuff, size_t num);
bool queue_peek(queue_t *queue, void *pbuff);
uint8_t queue_peek_multi(queue_t *queue, void *pbuff, size_t num);
void queue_delete(queue_t *queue);

/* ----------------------------------- 栈 ----------------------------------- */
typedef lybox_t stack_t;
stack_t *stack_create(uint8_t stack_len, uint8_t item_size);
bool stack_push(stack_t *stack, void *pval);
uint8_t stack_push_multi(stack_t *stack, void *pval, size_t num);
bool stack_pop(stack_t *stack, void *pbuff);
uint8_t stack_pop_multi(stack_t *stack, void *pbuff, size_t num);
void stack_delete(stack_t *stack);

/* ----------------------------------- 流 ----------------------------------- */
typedef lybox_t stream_t;
typedef uint8_t byte_t;
stream_t *stream_create(size_t bytes);   //默认成员大小为1字节
stream_t *stream_create_ex(uint8_t box_len, uint8_t mbr_size);
uint8_t stream_send(stream_t *stream, void *pval, size_t num);
uint8_t stream_receive(stream_t *stream, void *pbuff, size_t num);
void stream_delete(stream_t *stream);

/* --------------------------------- 事件/标志组 --------------------------------- */
#define EVENT_GROUP_USE_BITS    8
#if EVENT_GROUP_USE_BITS <= 8
typedef uint8_t event_bits_t;
#elif EVENT_GROUP_USE_BITS <= 16
typedef uint16_t event_bits_t;
#elif EVENT_GROUP_USE_BITS <= 32
typedef uint32_t event_bits_t;
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

event_group_t *event_group_create(void);
bool event_group_set(event_group_t *event_group, const event_bits_t bits);
bool event_group_clear(event_group_t *event_group, const event_bits_t bits);
event_bits_t event_group_get(event_group_t *event_group);
void event_group_delete(event_group_t *event_group);


#endif        /* __LYBOX_API__ */
