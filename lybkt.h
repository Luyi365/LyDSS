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

#ifndef __LYBKT__
#define __LYBKT__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct Node Node_t;
typedef struct LyBKT LyBKT_t;

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

// 桶
struct LyBKT {
	uint8_t count;   	///< 桶中项的实际数量
	
	Node_t *head_item;	///< 指向桶首成员
	Node_t *tail_item;	///< 指向桶尾成员
};

/* ---------------------------------- 检查索引名 --------------------------------- */
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
 * @param name 索引名
 * @param item 项
 * @param meta 元数据
 * @return Node_t* 生成的节点指针
 * 
 * @note 有同名则替换，无同名则增加
 */
Node_t *lybkt_put(LyBKT_t *bucket, const char *const name, void *item, struct Meta meta);

/**
 * @brief 得到最近的一个节点
 *
 * @param bucket 桶
 * @param at_tail 是否从尾部节点检索（默认从头部检索）
 * @return Node_t* 的节点指针
 */
Node_t *lybkt_head(LyBKT_t *bucket, bool at_tail);

/**
 * @brief 获取桶中的节点
 *
 * @param bucket 桶
 * @param name 索引名
 * @return Node_t* 节点指针
 */
Node_t *lybkt_get(LyBKT_t *bucket, const char *const name);

/**
 * @brief 修改桶中的索引名
 * 
 * @param bucket 桶
 * @param old_name 旧的索引名
 * @param new_name 新的索引名
 * @return bool 是否成功修改
 */
bool lybkt_patch_name(LyBKT_t *bucket, const char *const old_name, const char *const new_name);

/**
 * @brief 修改桶中的项指针
 * 
 * @param bucket 桶
 * @param name 索引名
 * @param item 项
 * @return bool 是否成功修改
 */
bool lybkt_patch_item(LyBKT_t *bucket, const char *const name, void *item);

/**
 * @brief 修改桶中的元数据
 * 
 * @param bucket 桶
 * @param name 索引名
 * @param meta 元数据
 * @return bool 是否成功修改
 */
bool lybkt_patch_meta(LyBKT_t *bucket, const char *const name, struct Meta meta);

/**
 * @brief 从桶中移除项但不释放
 *
 * @param bucket 桶
 * @param name 索引名
 * @return Node_t* 被移除的项
 */
Node_t *lybkt_remove(LyBKT_t *bucket, const char *const name);

/**
 * @brief 从桶中移除一个节点
 *
 * @param bucket 桶
 * @param node 节点指针
 */
void lybkt_remove_node(LyBKT_t *bucket, Node_t *node);

/**
 * @brief 删除桶中的节点
 *
 * @param bucket 桶
 * @param name 索引名
 * @return bool 是否成功修改
 */
bool lybkt_erase(LyBKT_t *bucket, const char *const name);

/**
 * @brief 销毁指定的桶
 *
 * @param bucket 桶
 * @param name 项的索引名
 */
void lybkt_delete(LyBKT_t *bucket);


/* -------------------------------- 不检查索引名，慎用 ------------------------------- */
/**
 * @brief 将一个项添加到桶
 *
 * @param bucket 桶
 * @param name 索引名
 * @param item 项
 * @param meta 元数据
 * @param at_head 是否从头部节点写入（默认尾部写入）
 * @return Node_t* 生成的节点指针
 * 
 * @note 可能造成索引名重复，不推荐用户直接使用该函数
 */
Node_t *lybkt_post(LyBKT_t *bucket, const char *const name, void *item, struct Meta meta, bool at_head);

/**
 * @brief 将一个项添加到桶
 *
 * @param bucket 桶
 * @param name 索引名
 * @param item 项
 * @param meta 元数据
 * @param at_name 定位索引名
 * @param at_before 是否在定位名之前写入（默认在之后写入）
 * @return Node_t* 生成的节点指针
 * 
 * @note 可能造成索引名重复，不推荐用户直接使用该函数
 */
Node_t *lybkt_insert(LyBKT_t *bucket, const char *const name, void *item, struct Meta meta, const char *const at_name, bool at_before);

#endif        /* __LYBKT__ */
