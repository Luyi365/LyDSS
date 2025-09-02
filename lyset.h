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

typedef struct Node Node_t;
typedef struct LySET LySET_t;

typedef enum {
	MetaType_Null = 0,
	MetaType_Copy,
	MetaType_Attach,
} MetaType_t;

// 节点
struct Node {
	const char *name;	///< 索引名
	void *item;			///< 项指针
	/// @note 所有元数据均为拷贝存储
	struct Meta{
		MetaType_t type;///< 元数据类型
		void *data;		///< 元数据（项副本/ 附属数据）
		size_t size;	///< 元数据大小
	} meta;
	
	Node_t *prev;		///< 前驱节点
	Node_t *next;		///< 后驱节点
};

// 集合
struct LySET {
	uint8_t count;   	///< 集合中项的实际数量
	
	Node_t *head_item;	///< 指向集合首成员
	Node_t *tail_item;	///< 指向集合尾成员
};

/// @brief 模拟键值对存/取
#define LYSET_PUT_KV(set, name, item) \
	lyset_put(set, name, item, MetaType_Null, NULL, 0)
#define LYSET_GET_KV(set, name) \
	lyset_get(set, name)->item

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
 * @param name 索引名
 * @param item 项
 * @param mtype 元属性
 * @param mdata 元数据
 * @param msize 元大小
 * @return Node_t* 生成的节点指针
 * 
 * @note 有同名则替换，无同名则增加
 */
Node_t *lyset_put(LySET_t *set, const char *const name, void *item, MetaType_t mtype, void *mdata, size_t msize);

/**
 * @brief 获取集合中的节点
 *
 * @param set 集合
 * @param name 索引名
 * @return Node_t* 节点指针
 */
Node_t *lyset_get(LySET_t *set, const char *const name);

/**
 * @brief 修改集合中的索引名
 * 
 * @param set 集合
 * @param old_name 旧的索引名
 * @param new_name 新的索引名
 * @return bool 是否成功修改
 */
bool lyset_patch_name(LySET_t *set, const char *const old_name, const char *const new_name);

/**
 * @brief 修改集合中的项指针
 * 
 * @param set 集合
 * @param name 索引名
 * @param item 项
 * @return bool 是否成功修改
 */
bool lyset_patch_item(LySET_t *set, const char *const name, void *item);

/**
 * @brief 修改集合中的元数据
 * 
 * @param set 集合
 * @param name 索引名
 * @param meta 元数据
 * @return bool 是否成功修改
 */
bool lyset_patch_meta(LySET_t *set, const char *const name, struct Meta meta);

/**
 * @brief 从集合中移除项但不释放
 *
 * @param set 集合
 * @param name 索引名
 * @return Node_t* 被移除的项
 */
Node_t *lyset_remove(LySET_t *set, const char *const name);

/**
 * @brief 删除集合中的节点
 *
 * @param set 集合
 * @param name 索引名
 * @return bool 是否成功修改
 */
bool lyset_delete_note(LySET_t *set, const char *const name);

/**
 * @brief 销毁指定的集合
 *
 * @param set 集合
 * @param name 项的索引名
 */
void lyset_delete(LySET_t *set);

#endif        /* __LYSET__ */
