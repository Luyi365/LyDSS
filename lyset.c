/**
 * @file lyset.c
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
 * LySET的核心是list + table结构，并引入RESET接口请求方法
 *
 *
 *  ------------------
 * /  如果代码有帮助到你  \
 * \  也请作者喝杯咖啡吧  /
 *  ------------------
 *      \   (__)
 *       \  /oo|\_______
 *         (_"_)\  牛码  )\/\
 *  █████   U   ||-----j/   █████████  ██████████ ███████████
 * ░░███    '   ||     ||  ███░░░░░███░░███░░░░░█░█░░░███░░░█
 *  ░███    '   █████ ████░███    ░░░  ░███  █ ░ ░   ░███  ░ 
 *  ░███    ;  ░░███ ░███ ░░█████████  ░██████       ░███      
 *  ░███ ~~~^~~;░███ ░███  ░░░░░░░░███ ░███░░█       ░███    
 *  ░███ ~~~~~█;░███ ░███  ███    ░███ ░███ ░   █    ░███    
 *  ███████████;░░███████ ░░█████████  ██████████    █████   
 * ░░░░░░░░░░░;; ░░░░░███  ░░░░░░░░░  ░░░░░░░░░░    ░░░░░    
 *          ;;;  ███ ░███                                       
 *          ;;  ░░██████
 *           ;   ░░░░░░
 */


#include <stdlib.h>
#include <string.h>
#include "lyset.h"

static void *_memdup(const void *src, size_t size);
static void _lyset_remove_node(LySET_t *set, Node_t *node);
static Node_t *lyset_post(LySET_t *set, const char *const name, void *item, MetaType_t mtype, void *mdata, size_t msize);

/// @brief 将一个项添加到集合
/// @note 不检查索引名
static Node_t *lyset_post(LySET_t *set, const char *const name, void *item, MetaType_t mtype, void *mdata, size_t msize)
{
	if (set == NULL) {
		return NULL;
	}
	
	Node_t *node = malloc(sizeof(Node_t));
	if (node == NULL) {
		return NULL;
	}
	
	void *tmp = NULL;
	switch(mtype)
	{
	case MetaType_Null: tmp = NULL; break;
	case MetaType_Copy: tmp = _memdup(item, msize); break;
	case MetaType_Attach: tmp = _memdup(mdata, msize); break;
	}
	
	*node = (Node_t){
		.name = name, 
		.item = item, 
		.meta.type = mtype,
		.meta.data = tmp, 
		.meta.size = msize,
		.prev = set->tail_item, 
		.next = NULL
	};
	
	if (set->count == 0) {
		set->head_item = node;
		set->tail_item = node;
	} 
	else {
		set->tail_item->next = node;
		set->tail_item = node;
	}
	
	set->count++;
	return node;
}

/// @brief 创建一个集合
LySET_t *lyset_create(void)
{
	LySET_t *set = malloc(sizeof(LySET_t));
	if (set == NULL) {
		return NULL;
	}
	
	*set = (LySET_t){
		.head_item = NULL,
		.tail_item = NULL,
		.count = 0
	};
	
	return set;
}

/// @brief 将一个项放到集合中
/// @note 遇到同名则替换，无同名则增加
Node_t *lyset_put(LySET_t *set, const char *const name, void *item, MetaType_t mtype, void *mdata, size_t msize)
{
	if (set == NULL) {
		return NULL;
	}
	
	Node_t *cur = lyset_get(set, name);
	
	if (cur == NULL) {
		// 添加新项
		cur = lyset_post(set, name, item, mtype, mdata, msize);
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

/// @brief 获取集合中的节点
Node_t *lyset_get(LySET_t *set, const char *const name)
{
	if (set == NULL || name == NULL || set->count == 0) {
		return NULL;
	}
	
	Node_t *cur = set->head_item;
	for (uint32_t i = 0; cur != NULL && i < set->count; cur = cur->next, i++) {
		if (cur->name != NULL && strcmp(cur->name, name) == 0) {
			return cur; // 找到匹配项
		}
	}
	
	return NULL;
}

/// @brief 修改集合中的索引名
bool lyset_patch_name(LySET_t *set, const char *const old_name, const char *const new_name)
{
	if (set == NULL || old_name == NULL || new_name == NULL || set->count == 0) {
		return NULL;
	}	
	
	Node_t *cur = lyset_get(set, old_name);
	if (cur != NULL) {
		cur->name = new_name;
		return true;
	}
	
	return false;
}

/// @brief 修改集合中的项指针
bool lyset_patch_item(LySET_t *set, const char *const name, void *item)
{
	if (set == NULL || name == NULL || set->count == 0) {
		return NULL;
	}
	
	Node_t *cur = lyset_get(set, name);
	if (cur != NULL) {
		cur->item = item;
		return true;
	}
	
	return false;
}

/// @brief 修改集合中的元数据
bool lyset_patch_meta(LySET_t *set, const char *const name, struct Meta meta)
{
	if (set == NULL || name == NULL || set->count == 0) {
		return NULL;
	}
	
	Node_t *cur = lyset_get(set, name);
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

/// @brief 从集合中移除项但不释放（返回被移除的项）
Node_t *lyset_remove(LySET_t *set, const char *const name)
{
	if (set == NULL || name == NULL || set->count == 0) {
		return NULL;
	}
	
	Node_t *cur = lyset_get(set, name);
	if (cur != NULL) {
		_lyset_remove_node(set, cur);
	}
	
	return cur;
}

/// @brief 删除集合中的节点
bool lyset_delete_note(LySET_t *set, const char *const name)
{
	if (set == NULL || name == NULL || set->count == 0) {
		return false;
	}
	
	Node_t *cur = lyset_remove(set, name);
	if (cur != NULL) {
		if(cur->meta.data != NULL) {
			free(cur->meta.data);
		}
		free(cur);
		return true;
	}
	
	return false;
}

/// @brief 销毁指定的集合
void lyset_delete(LySET_t *set)
{
	if (set == NULL) {
		return;
	}
	
	Node_t *cur = set->head_item;
	while (cur != NULL) {
		Node_t *next = cur->next;
		free(cur);
		cur = next;
	}
	
	free(set);
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

/// @brief 从集合中移除一个节点
static void _lyset_remove_node(LySET_t *set, Node_t *node)
{
	if (set == NULL || node == NULL) {
		return;
	}
	
	// 更新前驱节点的next指针
	if (node->prev != NULL) {
		node->prev->next = node->next;
	} 
	else {
		set->head_item = node->next;
	}
	
	// 更新后继节点的prev指针
	if (node->next != NULL) {
		node->next->prev = node->prev;
	} 
	else {
		set->tail_item = node->prev;
	}
	
	// 断开与链表的连接
	node->prev = NULL;
	node->next = NULL;
	
	// 更新计数器
	set->count--;
}
