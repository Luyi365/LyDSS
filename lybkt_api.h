/**
 * @file lybkt_api.h
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief
 * @date 2025-09-02
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

#ifndef __LYBKT_API__
#define __LYBKT_API__

#include <stdint.h> 
#include <stdbool.h>
#include "lybkt.h"

/* ----------------------------------- 集合 ----------------------------------- */
typedef LyBKT_t Set_t;

/**
 * @brief 创建一个集合
 *
 * @return Set_t* 集合指针
 */
Set_t *set_create(void);

/**
 * @brief 添加元素
 *
 * @param set 集合
 * @param elem 元素
 * @param size 元素大小
 * @return bool 是否成功
 */
bool set_add(Set_t *set, void *elem, size_t size);
/// @brief 推荐使用该宏添加元素
#define SET_ADD(set, elem) \
	set_add(set, &elem, sizeof(elem))

/**
 * @brief 取出元素
 *
 * @param set 集合
 * @param elem 元素
 * @return size 元素大小
 */
size_t set_take(Set_t *set, void *elem);

/**
 * @brief 集合中元素的数量
 *
 * @param set 集合
 * @return uint8_t 元素数量
 */
uint8_t set_count(Set_t *set);

/* ----------------------------------- 链表 ----------------------------------- */
typedef LyBKT_t List_t;

/* ------------------------------- 表/字典/映射 ------------------------------- */
typedef LyBKT_t Map_t;

#endif        /* __LYBKT_API__ */
