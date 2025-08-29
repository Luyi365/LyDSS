/**
 * @file lyset.h
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

#ifndef __LYSET__
#define __LYSET__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct LyItem LyItem_t;
typedef struct LySET LySET_t;

// 包装项
struct LyItem {
	void *item;			///< 项
	const char *name;	///< 名称
	size_t size;		///< 项大小（0为无效值）
	void* attach;		///< 附带其他属性
	
	LyItem_t *next;		///< 指向下一个项
};

// 集合
struct LySET {
	uint8_t count;   	///< 集合中项的实际数量
	
	LyItem_t *head_item;///< 指向集合首成员
	LyItem_t *tail_item;///< 指向集合尾成员
};


/**
 * @brief 创建一个集合
 *
 * @return LySET_t* 集合指针
 */
LySET_t *lyset_create(void);

/**
 * @brief 将一个项放到集合中
 *
 * @param set 集合
 * @param name 项的索引名
 * @param item 项
 * @param size 项大小（单位：字节）
 * @param attach 附带属性
 * @return LyItem_t* 生成的包装项
 * 
 * @note 有同名则替换，无同名则增加
 */
LyItem_t *lyset_put(LySET_t *set, const char *const name, void *item, size_t size, void *attach);

/**
 * @brief 将一个项放到集合中（不带任何属性）
 * 
 * @param set 集合
 * @param name 项的索引名
 * @param item 项
 * @return LyItem_t* 生成的包装项
 * 
 * @note 这个函数就是实现单纯的键值对功能
 */
LyItem_t *lyset_put_simple(LySET_t *set, const char *const name, void *item);

/**
 * @brief 修改集合中的项
 * 
 * @param set 集合
 * @param name 项的索引名
 * @param item 项
 * @param size 项大小（单位：字节）
 * @return bool 是否成功修改
 */
bool lyset_patch_item(LySET_t *set, const char *const name, void *item, size_t size);

/**
 * @brief 修改集合中的项属性
 * 
 * @param set 集合
 * @param name 项的索引名
 * @param attach 附加属性
 * @return bool 是否成功修改
 */
bool lyset_patch_attach(LySET_t *set, const char *const name, void *attach);

/**
 * @brief 获取集合中的包装项
 *
 * @param set 集合
 * @param name 项的索引名
 * @return LyItem_t* 包装项
 */
LyItem_t *lyset_get(LySET_t *set, const char *const name);

/**
 * @brief 获取集合中的指定项
 *
 * @param set 集合
 * @param name 项的索引名
 * @return void* 项
 */
void *lyset_get_item(LySET_t *set, const char *const name);

/**
 * @brief 清除集合中的指定项
 *
 * @param set 集合
 * @param name 项的索引名
 */
void lyset_clear(LySET_t *set, const char *const name);

/**
 * @brief 销毁指定的集合
 *
 * @param set 集合
 * @param name 项的索引名
 */
void lyset_delete(LySET_t *set);

#endif        /* __LYSET__ */
