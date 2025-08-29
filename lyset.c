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

/// @brief 将一个项放入集合
/// @note 不检查索引名
static LyItem_t *lyset_post(LySET_t *set, const char *const name, void *item, size_t size, void *attach)
{
	if(set == NULL) {
		return 0;
	}
	
	LyItem_t *lyitem = malloc(sizeof(LyItem_t));
	*lyitem = (LyItem_t){.name = name, .item = item, .size = size, .attach = attach, .next = NULL};
	
	if(set->count == 0) {
		set->head_item = lyitem;
		set->tail_item = lyitem;
		set->count = 1;
	}
	else{
		set->tail_item->next = lyitem;
		set->tail_item = lyitem;
		set->count++;
	}
	
	return lyitem;
}

/// @brief 创建一个集合
LySET_t *lyset_create(void)
{
	LySET_t *set = malloc(sizeof(LySET_t));
	
	set->head_item = NULL;
	set->tail_item = NULL;
	set->count = 0;
	
	return set;
}

/// @brief 将一个项放到集合中
/// @note 遇到同名则替换，无同名则增加
LyItem_t *lyset_put(LySET_t *set, const char *const name, void *item, size_t size, void *attach)
{
	if(set == NULL) {
		return 0;
	}
	
	LyItem_t *cur = set->head_item;
	for(uint8_t i = 0; cur != NULL && i < set->count; cur = cur->next, i++) {
		if (cur->name != NULL && strcmp(cur->name, name) == 0) {
			break; // 找到匹配项，退出查找
		}
	}
	
	// 检查是否有匹配项
	if(cur != NULL){
		cur->item = item;
		cur->size = size;
		cur->attach = attach;
	}
	else{
		cur = lyset_post(set, name, item, size, attach);
	}
	
	return cur;
}

/// @brief 将一个项放到集合中（不带任何属性）
LyItem_t *lyset_put_simple(LySET_t *set, const char *const name, void *item)
{
	if(set == NULL) {
		return 0;
	}
	
	LyItem_t *cur = set->head_item;
	for(uint8_t i = 0; cur != NULL && i < set->count; cur = cur->next, i++) {
		if (cur->name != NULL && strcmp(cur->name, name) == 0) {
			break; // 找到匹配项，退出查找
		}
	}
	
	// 检查是否有匹配项
	if(cur != NULL){
		cur->item = item;
	}
	else{
		cur = lyset_post(set, name, item, 0, NULL);
	}
	
	return cur;
}

/// @brief 修改集合中的项
bool lyset_patch_item(LySET_t *set, const char *const name, void *item, size_t size)
{
	if(set == NULL) {
		return false;
	}
	
	LyItem_t *cur = set->head_item;
	for(uint8_t i = 0; cur != NULL && i < set->count; cur = cur->next, i++) {
		if (cur->name != NULL && strcmp(cur->name, name) == 0) {
			break; // 找到匹配项，退出查找
		}
	}
	
	if(cur != NULL){
		cur->item = item;
		cur->size = size;
		return true;
	}
	else{
		return false;
	}
}

/// @brief 修改集合中的项属性
bool lyset_patch_attach(LySET_t *set, const char *const name, void *attach)
{
	if(set == NULL) {
		return false;
	}
	
	LyItem_t *cur = set->head_item;
	for(uint8_t i = 0; cur != NULL && i < set->count; cur = cur->next, i++) {
		if (cur->name != NULL && strcmp(cur->name, name) == 0) {
			break; // 找到匹配项，退出查找
		}
	}
	
	if(cur != NULL){
		cur->attach = attach;
		return true;
	}
	else{
		return false;
	}
}

/// @brief 获取集合中的包装项
LyItem_t *lyset_get(LySET_t *set, const char *const name)
{
	if(set == NULL || name == NULL || set->count == 0) {
		return NULL;
	}
	
	LyItem_t *cur = set->head_item;
	for(uint8_t i = 0; cur != NULL && i < set->count; cur = cur->next, i++) {
		if (cur->name != NULL && strcmp(cur->name, name) == 0) {
			break; // 找到匹配项，退出查找
		}
	}
	
	return cur;
}

/// @brief 获取集合中的指定项
void *lyset_get_item(LySET_t *set, const char *const name)
{
	if(set == NULL || name == NULL || set->count == 0) {
		return NULL;
	}
	
	LyItem_t *cur = set->head_item;
	for(uint8_t i = 0; cur != NULL && i < set->count; cur = cur->next, i++) {
		if (cur->name != NULL && strcmp(cur->name, name) == 0) {
			break; // 找到匹配项，退出查找
		}
	}
	
	return cur->item;
}

/// @brief 清除集合中的指定项
void lyset_clear(LySET_t *set, const char *const name)
{
	if(set == NULL || name == NULL || set->count == 0) {
		return;
	}
	
	LyItem_t *cur = set->head_item;
	LyItem_t *prev = NULL;
	
	for(uint32_t i = 0; cur != NULL && i < set->count; i++) {
		if (cur->name != NULL && strcmp(cur->name, name) == 0) {
			// 找到匹配项，进行删除
			if (prev == NULL) {
				// 删除头节点
				set->head_item = cur->next;
				if (cur->next == NULL) {
					set->tail_item = NULL;
				}
			} 
			else {
				prev->next = cur->next;
				if (cur->next == NULL) {
					set->tail_item = prev;
				}
			}
			
			free(cur);
			set->count--;
			return; // 找到一个就返回
		}
		
		prev = cur;
		cur = cur->next;
	}
}

/// @brief 销毁指定的集合
void lyset_delete(LySET_t *set)
{
	if(set == NULL) {
		return;
	}
	
	LyItem_t *cur = set->head_item;
	
	for(uint32_t i = 0; cur != NULL && i < set->count; i++) {
		LyItem_t *next = cur->next;
		free(cur);
		cur = next;
	}
	
	set->head_item = NULL;
	set->tail_item = NULL;
	set->count = 0;
	
	free(set);
}


