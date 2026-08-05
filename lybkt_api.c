/**
 * @file lybkt_api.c
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

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "lybkt_api.h"

/* ----------------------------------- 集合 ----------------------------------- */
/// @@brief 创建一个集合
Set_t *set_create(void)
{
	return lybkt_create();
}

/// @brief 添加元素
bool set_add(Set_t *set, void *elem, size_t size)
{
	if(elem == NULL){
		return false;
	}
	
	Node_t *note = lybkt_post(set, NULL, elem, (struct Meta){MetaType_Copy, NULL, size}, (uintptr_t)elem & 1);
	if(note == NULL) {
		return false;
	}
	else {
		note->item = note->meta.data;
		return true;
	}
}

/// @brief 取出元素
size_t set_take(Set_t *set, void *elem)
{
	Node_t *note = lybkt_head(set, (uintptr_t)elem & 1);
	if(note == NULL){
		return 0;
	}
	else{
		size_t size = note->meta.size;
		memcpy(elem, note->item, size);
		lybkt_remove_node(set, note);
		return size;
	}
}

/// @brief 集合中元素的数量
uint8_t set_count(Set_t *set)
{
	if(set == NULL){
		return 0;
	}
	else{
		return set->count;
	}
}


/* ----------------------------------- 链表 ----------------------------------- */


/* ------------------------------- 表/字典/映射 ------------------------------- */
