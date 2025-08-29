/**
 * @file lybkt.h
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

#ifndef __BYBKT__
#define __BYBKT__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct LyItem LyItem_t;
typedef struct LyBKT LyBKT_t;

// 包装项
struct LyItem {
	const char *name;	///< 名称
	void *item;			///< 项
	size_t size;		///< 项大小
	
	void* attach;	///< 附带其他属性
	
	LyItem_t *next;
};

// 桶
struct LyBKT {
	uint8_t count;   	///< 桶中项的实际数量
	
	LyItem_t *head_item;
	LyItem_t *tail_item;
};


/**
 * @brief 创建一个桶
 *
 * @return LyBKT_t* 桶指针
 */
LyBKT_t *lybkt_create(void);

/**
 * @brief 将一个项放到桶中
 *
 * @param bucket 桶
 * @param name 项名称
 * @param item 项
 * @param size 项大小（单位：字节）
 * @param attach 附带属性
 * @return LyItem_t* 生成的包装项
 * 
 * @note 有同名则替换，无同名则增加
 */
LyItem_t *lybkt_put(LyBKT_t *bucket, const char *const name, void *item, size_t size, void *attach);

/**
 * @brief 将一个项放到桶中（不带任何属性）
 * 
 * @param bucket 桶
 * @param name 项名称
 * @param item 项
 * 
 * @return LyItem_t* 生成的包装项
 * 
 * @note 这个函数就是实现很单纯的KV功能
 */
LyItem_t *lybkt_put_item(LyBKT_t *bucket, const char *const name, void *item);

/**
 * @brief 将一个项放到桶中（不带任何属性）
 * 
 * @param bucket 桶
 * @param name 项名称
 * @param item 项
 * @return bool 是否放入成功
 */
bool lybkt_patch_item(LyBKT_t *bucket, const char *const name, void *item);

/**
 * @brief 将一个项放到桶中（不带任何属性）
 * 
 * @param bucket 桶
 * @param name 项名称
 * @param attach 附加属性
 * @return bool 是否放入成功
 */
bool lybkt_patch_attach(LyBKT_t *bucket, const char *const name, void *attach);

/**
 * @brief 获取桶中的指定包装项
 *
 * @param bucket 桶
 * @param name 项名称
 * @return LyItem_t* 包装项
 */
LyItem_t *lybkt_get(LyBKT_t *bucket, const char *const name);

/**
 * @brief 获取桶中的指定项
 *
 * @param bucket 桶
 * @param name 项名称
 * @return void* 项
 */
void *lybkt_get_item(LyBKT_t *bucket, const char *const name);

/**
 * @brief 清除桶中的指定项
 *
 * @param bucket 桶
 * @param name 项
 */
void lybkt_clear(LyBKT_t *bucket, const char *const name);

/**
 * @brief 销毁指定的桶
 *
 * @param bucket 桶
 * @param name 项
 */
void lybkt_delete(LyBKT_t *bucket);

#endif        /* __LYBKT__ */
