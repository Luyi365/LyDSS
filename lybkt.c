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
 *  ------------------
 * /  如果代码有帮助到你  \
 * \  也请作者喝杯咖啡吧  /
 *  ------------------
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

/// @brief 将一个项放入桶
/// @note 不检查重名
static LyItem_t *lybkt_post(LyBKT_t *bucket, const char *const name, void *item, size_t size, void *attach)
{
	if(bucket == NULL) {
		return 0;
	}
	
	LyItem_t *lyitem = malloc(sizeof(LyItem_t));
	*lyitem = (LyItem_t){.name = name, .item = item, .size = size, .attach = attach, .next = NULL};
	
	if(bucket->count == 0) {
		bucket->head_item = lyitem;
		bucket->tail_item = lyitem;
		bucket->count = 1;
	}
	else{
		bucket->tail_item->next = lyitem;
		bucket->tail_item = lyitem;
		bucket->count++;
	}
	
	return lyitem;
}

/// @brief 创建一个桶
LyBKT_t *lybkt_create(void)
{
	LyBKT_t *bucket = malloc(sizeof(LyBKT_t));
	
	bucket->head_item = NULL;
	bucket->tail_item = NULL;
	bucket->count = 0;
	
	return bucket;
}

/// @brief 将一个项放到桶中
/// @note 遇到同名则替换，无同名则增加
LyItem_t *lybkt_put(LyBKT_t *bucket, const char *const name, void *item, size_t size, void *attach)
{
	if(bucket == NULL) {
		return 0;
	}
	
	LyItem_t *cur = bucket->head_item;
	for(uint8_t i = 0; cur != NULL && i < bucket->count; cur = cur->next, i++) {
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
		cur = lybkt_post(bucket, name, item, size, attach);
	}
	
	return cur;
}

/// @brief 将一个项放到桶中
LyItem_t *lybkt_put_item(LyBKT_t *bucket, const char *const name, void *item)
{
	if(bucket == NULL) {
		return 0;
	}
	
	LyItem_t *cur = bucket->head_item;
	for(uint8_t i = 0; cur != NULL && i < bucket->count; cur = cur->next, i++) {
		if (cur->name != NULL && strcmp(cur->name, name) == 0) {
			break; // 找到匹配项，退出查找
		}
	}
	
	// 检查是否有匹配项
	if(cur != NULL){
		cur->item = item;
	}
	else{
		cur = lybkt_post(bucket, name, item, 0, NULL);
	}
	
	return cur;
}

/// @brief 修改项对象
bool lybkt_patch_item(LyBKT_t *bucket, const char *const name, void *item)
{
	if(bucket == NULL) {
		return false;
	}
	
	LyItem_t *cur = bucket->head_item;
	for(uint8_t i = 0; cur != NULL && i < bucket->count; cur = cur->next, i++) {
		if (cur->name != NULL && strcmp(cur->name, name) == 0) {
			break; // 找到匹配项，退出查找
		}
	}
	
	if(cur != NULL){
		cur->item = item;
		return true;
	}
	else{
		return false;
	}
}

/// @brief 修改项附加属性
bool lybkt_patch_attach(LyBKT_t *bucket, const char *const name, void *attach)
{
	if(bucket == NULL) {
		return false;
	}
	
	LyItem_t *cur = bucket->head_item;
	for(uint8_t i = 0; cur != NULL && i < bucket->count; cur = cur->next, i++) {
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

LyItem_t *lybkt_get(LyBKT_t *bucket, const char *const name)
{
	if(bucket == NULL || name == NULL || bucket->count == 0) {
		return NULL;
	}
	
	LyItem_t *cur = bucket->head_item;
	for(uint8_t i = 0; cur != NULL && i < bucket->count; cur = cur->next, i++) {
		if (cur->name != NULL && strcmp(cur->name, name) == 0) {
			break; // 找到匹配项，退出查找
		}
	}
	
	return cur;
}

/// @brief 获取桶中的指定项
void *lybkt_get_item(LyBKT_t *bucket, const char *const name)
{
	if(bucket == NULL || name == NULL || bucket->count == 0) {
		return NULL;
	}
	
	LyItem_t *cur = bucket->head_item;
	for(uint8_t i = 0; cur != NULL && i < bucket->count; cur = cur->next, i++) {
		if (cur->name != NULL && strcmp(cur->name, name) == 0) {
			break; // 找到匹配项，退出查找
		}
	}
	
	return cur->item;
}

/// @brief 清除桶中的指定项
void lybkt_clear(LyBKT_t *bucket, const char *const name)
{
	if(bucket == NULL || name == NULL || bucket->count == 0) {
		return;
	}
	
	LyItem_t *cur = bucket->head_item;
	LyItem_t *prev = NULL;
	
	for(uint32_t i = 0; cur != NULL && i < bucket->count; i++) {
		if (cur->name != NULL && strcmp(cur->name, name) == 0) {
			// 找到匹配项，进行删除
			if (prev == NULL) {
				// 删除头节点
				bucket->head_item = cur->next;
				if (cur->next == NULL) {
					bucket->tail_item = NULL;
				}
			} 
			else {
				prev->next = cur->next;
				if (cur->next == NULL) {
					bucket->tail_item = prev;
				}
			}
			
			free(cur);
			bucket->count--;
			return; // 找到一个就返回
		}
		
		prev = cur;
		cur = cur->next;
	}
}

/// @brief 销毁指定的桶
void lybkt_delete(LyBKT_t *bucket)
{
	if(bucket == NULL) {
		return;
	}
	
	LyItem_t *cur = bucket->head_item;
	
	for(uint32_t i = 0; cur != NULL && i < bucket->count; i++) {
		LyItem_t *next = cur->next;
		free(cur);
		cur = next;
	}
	
	bucket->head_item = NULL;
	bucket->tail_item = NULL;
	bucket->count = 0;
	
	free(bucket);
}


