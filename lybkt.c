/**
 * @file lybkt.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief
 * @date 2025-08-28
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
 * LyBKT的核心是list + table结构，并引入RESET接口请求方法
 *
 *
 *  -------------------
 * /  如果代码有帮助到你  \
 * \  也请作者喝杯咖啡吧  /
 *  -------------------
 *      \   (__)
 *       \  /oo|\_______
 *         (_"_)\  牛码  )\/\
 *  █████   U   ||-----j/  ███████████  █████   ████ ███████████
 * ░░███    '   ||     || ░░███░░░░░███░░███   ███░ ░█░░░███░░░█
 *  ░███    '   █████ ████ ░███    ░███ ░███  ███   ░   ░███  ░ 
 *  ░███    ;  ░░███ ░███  ░██████████  ░███████        ░███    
 *  ░███ ~~~^~~;░███ ░███  ░███░░░░░███ ░███░░███       ░███   
 *  ░███ ~~~~~█;░███ ░███  ░███    ░███ ░███ ░░███      ░███    
 *  ███████████;░░███████  ███████████  █████ ░░████    █████   
 * ░░░░░░░░░░░;; ░░░░░███ ░░░░░░░░░░░  ░░░░░   ░░░░    ░░░░░    
 *          ;;;  ███ ░███                                       
 *          ;;  ░░██████
 *           ;   ░░░░░░
 */


#include <stdlib.h>
#include <string.h>
#include "lybkt.h"

static void *_memdup(const void *src, size_t size);
static void _lybkt_remove_node(LyBKT_t *bucket, Node_t *node);

/// @brief 创建一个桶
LyBKT_t *lybkt_create(void)
{
	LyBKT_t *bucket = malloc(sizeof(LyBKT_t));
	if (bucket == NULL) {
		return NULL;
	}
	
	*bucket = (LyBKT_t){
		.head_item = NULL,
		.tail_item = NULL,
		.count = 0
	};
	
	return bucket;
}

/// @brief 将一个项添加到桶
Node_t *lybkt_post(LyBKT_t *bucket, const char *const name, void *item, struct Meta meta, bool at_head)
{
	if (bucket == NULL) {
		return NULL;
	}
	
	Node_t *node = malloc(sizeof(Node_t));
	if (node == NULL) {
		return NULL;
	}
	
	void *tmp = NULL;
	switch(meta.type)
	{
		case MetaType_Null: tmp = NULL; break;
		case MetaType_Copy: tmp = _memdup(item, meta.size); break;
		case MetaType_Attach: tmp = _memdup(meta.data, meta.size); break;
	}
	
	*node = (Node_t){
		.name = name, 
		.item = item, 
		.meta.type = meta.type,
		.meta.data = tmp, 
		.meta.size = meta.size,
		.prev = NULL, 
		.next = NULL,
	};
	
	if (bucket->count == 0) {
		bucket->head_item = node;
		bucket->tail_item = node;
	} 
	else {
		if(at_head == true) {
			node->next = bucket->head_item;
			bucket->head_item->prev = node;
			bucket->head_item = node;
		}
		else {
			node->prev = bucket->tail_item;
			bucket->tail_item->next = node;
			bucket->tail_item = node;
		}
	}
	
	bucket->count++;
	return node;
}

/// @brief 将一个项放到桶中
/// @note 遇到同名则替换，无同名则增加
Node_t *lybkt_put(LyBKT_t *bucket, const char *const name, void *item, MetaType_t mtype, void *mdata, size_t msize)
{
	if (bucket == NULL) {
		return NULL;
	}
	
	Node_t *cur = lybkt_get(bucket, name);
	
	if (cur == NULL) {
		// 添加新项
		cur = lybkt_post(bucket, name, item, (struct Meta){mtype, mdata, msize}, 0);
	} 
	else {
		// 更新现有项
		if(cur->meta.data != NULL) {
			free(cur->meta.data);
		}
		switch(mtype)
		{
			case MetaType_Null: cur->meta.data = NULL; break;
			case MetaType_Copy: cur->meta.data = _memdup(item, msize); break;
			case MetaType_Attach: cur->meta.data = _memdup(mdata, msize); break;
		}
		cur->item = item;
		cur->meta.type = mtype;
		cur->meta.size = msize;
	}
	
	return cur;
}

/// @brief 将一个项插入到桶指定位置
Node_t *lybkt_insert(LyBKT_t *bucket, const char *const name, void *item, struct Meta meta, const char *const at_name, bool at_before)
{
	if (bucket == NULL || at_name == NULL || bucket->count == 0) {
		return NULL;
	}	
	
	Node_t *cur = lybkt_get(bucket, at_name);
	if (cur != NULL) {
		Node_t *node = malloc(sizeof(Node_t));
		if (node == NULL) {
			return NULL;
		}
		
		void *tmp = NULL;
		switch(meta.type)
		{
			case MetaType_Null: tmp = NULL; break;
			case MetaType_Copy: tmp = _memdup(item, meta.size); break;
			case MetaType_Attach: tmp = _memdup(meta.data, meta.size); break;
		}
		
		*node = (Node_t){
			.name = name, 
			.item = item, 
			.meta.type = meta.type,
			.meta.data = tmp, 
			.meta.size = meta.size,
			.prev = NULL, 
			.next = NULL,
		};
		
		if (at_before == true) {
			if(cur->prev == NULL || cur == bucket->head_item) {
				bucket->head_item = node;
			}
			else {
				node->prev = cur->prev;
				cur->prev->next = node;
			}
			node->next = cur;
			cur->prev = node;
		}
		else {
			if(cur->next == NULL || cur == bucket->tail_item) {
				bucket->tail_item = node;
			}
			else {
				node->next = cur->next;
				cur->next->prev = node;
			}
			node->prev = cur;
			cur->next = node;
		}
		
		bucket->count++;
		return node;
	}
	
	return NULL;
}

/// @brief 获取桶中的节点
Node_t *lybkt_get(LyBKT_t *bucket, const char *const name)
{
	if (bucket == NULL || name == NULL || bucket->count == 0) {
		return NULL;
	}
	
	Node_t *cur = bucket->head_item;
	for (uint32_t i = 0; cur != NULL && i < bucket->count; cur = cur->next, i++) {
		if (cur->name != NULL && strcmp(cur->name, name) == 0) {
			return cur; // 找到匹配项
		}
	}
	
	return NULL;
}

/// @brief 修改桶中的索引名
bool lybkt_patch_name(LyBKT_t *bucket, const char *const old_name, const char *const new_name)
{
	if (bucket == NULL || old_name == NULL || new_name == NULL || bucket->count == 0) {
		return NULL;
	}	
	
	Node_t *cur = lybkt_get(bucket, old_name);
	if (cur != NULL) {
		cur->name = new_name;
		return true;
	}
	
	return false;
}

/// @brief 修改桶中的项指针
bool lybkt_patch_item(LyBKT_t *bucket, const char *const name, void *item)
{
	if (bucket == NULL || name == NULL || bucket->count == 0) {
		return NULL;
	}
	
	Node_t *cur = lybkt_get(bucket, name);
	if (cur != NULL) {
		cur->item = item;
		return true;
	}
	
	return false;
}

/// @brief 修改桶中的元数据
bool lybkt_patch_meta(LyBKT_t *bucket, const char *const name, struct Meta meta)
{
	if (bucket == NULL || name == NULL || bucket->count == 0) {
		return NULL;
	}
	
	Node_t *cur = lybkt_get(bucket, name);
	if (cur != NULL) {
		if(cur->meta.data != NULL) {
			free(cur->meta.data);
		}
		switch(meta.type)
		{
			case MetaType_Null: meta.data = NULL; break;
			case MetaType_Copy: meta.data = _memdup(cur->item, meta.size); break;
			case MetaType_Attach: meta.data = _memdup(meta.data, meta.size); break;
		}
		cur->meta = meta;
		return true;
	}
	
	return false;
}

/// @brief 从桶中移除项但不释放（返回被移除的项）
Node_t *lybkt_remove(LyBKT_t *bucket, const char *const name)
{
	if (bucket == NULL || name == NULL || bucket->count == 0) {
		return NULL;
	}
	
	Node_t *cur = lybkt_get(bucket, name);
	if (cur != NULL) {
		_lybkt_remove_node(bucket, cur);
		// 断开与链表的连接
		cur->prev = NULL;
		cur->next = NULL;
	}
	
	return cur;
}

/// @brief 删除桶中的节点
bool lybkt_delete_note(LyBKT_t *bucket, const char *const name)
{
	if (bucket == NULL || name == NULL || bucket->count == 0) {
		return false;
	}
	
	Node_t *cur = lybkt_remove(bucket, name);
	if (cur != NULL) {
		if(cur->meta.data != NULL) {
			free(cur->meta.data);
		}
		free(cur);
		return true;
	}
	
	return false;
}

/// @brief 销毁指定的桶
void lybkt_delete(LyBKT_t *bucket)
{
	if (bucket == NULL) {
		return;
	}
	
	Node_t *cur = bucket->head_item;
	while (cur != NULL) {
		Node_t *next = cur->next;
		free(cur);
		cur = next;
	}
	
	free(bucket);
}

/// @brief 分配内存并复制内容（返回新分配的内存指针）
static void *_memdup(const void *src, size_t size)
{
	if (src == NULL || size == 0) {
		return NULL;
	}
	
	void *dest = malloc(size);
	if (dest == NULL) {
		return NULL;
	}
	
	return memcpy(dest, src, size);
}

/// @brief 从桶中移除一个节点
static void _lybkt_remove_node(LyBKT_t *bucket, Node_t *node)
{
	if (bucket == NULL || node == NULL) {
		return;
	}
	
	// 更新前驱节点的next指针
	if (node->prev != NULL) {
		node->prev->next = node->next;
	} 
	else {
		bucket->head_item = node->next;
	}
	
	// 更新后继节点的prev指针
	if (node->next != NULL) {
		node->next->prev = node->prev;
	} 
	else {
		bucket->tail_item = node->prev;
	}
	
	// 更新计数器
	bucket->count--;
}


