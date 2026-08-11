/**
 * @file lydds_api.h
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief
 * @date 2026-08-07
 *
 * @copyright Copyright (c) 2026
 *
 * @note
 * Matters related to this document.
 */

#ifndef __LYDDS_API__
#define __LYDDS_API__

#include <stdint.h> 
#include <stddef.h>
#include <stdbool.h>
#include "lydds_cfg.h"

#if LYDDS_ASSERT_ENABLED

#define LYDDS_PRINTF(...) printf(__VA_ARGS__)

#define LYDDS_ASSERT(EXPR)                                                       \
    do {                                                                         \
        if (!(EXPR)) {                                                           \
            LYDDS_PRINTF("(%s) has assert failed at %s.\n", #EXPR, __func__);    \
            while (1) { __asm("int3"); }                                         \
        }                                                                        \
    } while (0)

#else

#define LYDDS_ASSERT(EXPR) do { (void)(EXPR); } while(0)

#endif /* LYDDS_ASSERT_ENABLED */

/* ------------------------------------ 池 ----------------------------------- */
typedef struct LyPool LyPool_t;
typedef struct LyPoolItem LyPoolItem_t;

/**
 * @brief 创建一个池
 *
 * @param len 池容量
 * @return LyPool_t
 */
LyPool_t *lypool_create(size_t len);

/**
 * @brief 创建一个静态池
 *
 * @param pool 池
 * @param items 对象数组
 * @param size 数组大小
 */
// void lypool_create_static(LyPool_t *pool, LyPoolItem_t *items, size_t size);

/**
 * @brief 将一个对象引入池
 *
 * @param pool 池
 * @param name 对象名称
 * @param obj 对象
 */
void lypool_post(LyPool_t *pool, const char *const name, void *obj);

/**
 * @brief 获取池中的指定对象
 *
 * @param pool 池
 * @param name 对象名称
 */
void *lypool_get(LyPool_t *pool, const char *const name);

/**
 * @brief 清除池中的指定对象
 *
 * @param pool 池
 * @param name 对象名称
 */
void lypool_clear(LyPool_t *pool, const char *const name);

/**
 * @brief 销毁指定的池
 *
 * @param pool 池
 */
void lypool_delete(LyPool_t *pool);


/* ----------------------------------- 队列 ----------------------------------- */
typedef struct LyQueue LyQueue_t;

/**
 * @brief 创建一个队列
 *
 * @param length 队列长度
 * @param item_size 单个元素大小
 * @return LyQueue_t
 */
LyQueue_t *lyqueue_create(unsigned long length, unsigned long item_size);

/**
 * @brief 创建一个静态队列
 *
 * @param queue 队列
 * @param buffer 队列缓冲区
 * @param length 队列长度
 * @param item_size 单个元素大小
 */
void lyqueue_create_static(LyQueue_t *queue, void *buffer, unsigned long length, unsigned long item_size);

/**
 * @brief 数据入队
 *
 * @param queue 队列
 * @param item 数据项
 * @return true 入队成功，false 入队失败
 */
bool lyqueue_send(LyQueue_t *queue, void *item);

/**
 * @brief 数据出队
 *
 * @param queue 队列
 * @param buff 缓冲区
 * @return true 出队成功，false 出队失败
 */
bool lyqueue_receive(LyQueue_t *queue, void *buff);

/**
 * @brief 预读队列数据
 *
 * @param queue 队列
 * @param buff 缓冲区
 * @return true 预读成功，false 预读失败
 */
bool lyqueue_peek(LyQueue_t *queue, void *buff);

/**
 * @brief 清空队列
 *
 * @param queue 队列
 * @return true 清空成功，false 清空失败
 */
bool lyqueue_clear(LyQueue_t *queue);

/**
 * @brief 判断队列是否已满
 *
 * @param queue 队列
 * @return true 已满，false 未满
 */
bool lyqueue_is_full(LyQueue_t *queue);

/* ----------------------------------- 集合 ----------------------------------- */
enum LySetRelation {
    LySetRelation_NONE,        /* 完全不相交 */
    LySetRelation_INTER,       /* 有交集，但互不包含 */
    LySetRelation_PROSUB,      /* self 是 other 的真子集 */
    LySetRelation_PROSUP,      /* self 是 other 的真超集 */
    LySetRelation_SAME         /* self 与 other 相等 */
};
typedef enum LySetRelation LySetRelation_t;

typedef struct LySet LySet_t;
typedef unsigned (Hash_t)(const void *data, size_t len);
typedef bool (Equals_t)(const void *a, const void *b);

/**
 * @brief 初始化一个集合
 *
 * @param size 集合容量
 * @param elem_size 元素大小
 * @param hash 哈希函数
 * @param equals 比较函数
 * @return LySet_t
 */
LySet_t *lyset_init(size_t size, size_t elem_size, Hash_t *hash, Equals_t *equals);

/**
 * @brief 向集合中添加元素
 *
 * @param set 集合
 * @param elem 元素
 * @return true 添加成功，false 已存在
 */
bool lyset_add(LySet_t *set, const void *elem);

/**
 * @brief 若集合中存在该元素则移除
 *
 * @param set 集合
 * @param elem 元素
 * @return true 移除成功，false 不存在
 */
bool lyset_remove(LySet_t *set, const void *elem);

/**
 * @brief 从集合移除并返回任意一个元素
 *
 * @param set 集合
 * @param out 元素
 * @return true 移除成功，false 空集合
 */
bool lyset_pop(LySet_t *set, void *out);

/**
 * @brief 移除指定容器里所有的元素
 *
 * @param set 集合
 */
void lyset_erase(LySet_t *set);

/**
 * @brief 销毁指定的集合
 *
 * @param set 集合
 */
void lyset_destroy(LySet_t *set);

/**
 * @brief 得到集合的元素数量
 *
 * @param set 集合
 */
size_t lyset_count(const LySet_t *set);

/**
 * @brief 判断集合中是否包含指定的元素
 *
 * @param set 集合
 * @param elem 元素
 * @return true 包含，false 不包含
 */
bool lyset_contains(const LySet_t *set, const void *elem);

/**
 * @brief 判断两个集合的关系
 *
 * @param self 集合
 * @param other 集合
 * @return LySetRelation_t
 */
LySetRelation_t lyset_relation(const LySet_t *self, const LySet_t *other);

/* ------------------------------------ 桶 ----------------------------------- */
enum LyBucketMetaType {
    LyBucketMetaType_NULL = 0,
    LyBucketMetaType_COPY,
    LyBucketMetaType_ATTACH,
};
typedef enum LyBucketMetaType LyBucketMetaType_t;

struct LyBucketMeta {
    LyBucketMetaType_t type;///< 元数据类型
    void *data;		///< 元数据（项副本/ 附属数据）
    size_t size;	///< 元数据大小
};
typedef struct LyBucketMeta LyBucketMeta_t;

typedef struct LyBucketNode LyBucketNode_t;
typedef struct LyBucket LyBucket_t;

/* ---------------------------------- 检查索引名 --------------------------------- */
/**
 * @brief 创建一个桶
 *
 * @return LyBucket_t* 桶指针
 */
LyBucket_t *lybucket_create(void);

/**
 * @brief 将一个项放到桶中
 *
 * @param bucket 桶
 * @param name 索引名
 * @param item 项
 * @param meta 元数据
 * @return LyBucketNode_t* 生成的节点指针
 *
 * @note 有同名则替换，无同名则增加
 */
LyBucketNode_t *lybucket_put(LyBucket_t *bucket, const char *const name, void *item, LyBucketMeta_t meta);

/**
 * @brief 得到最近的一个节点
 *
 * @param bucket 桶
 * @param at_tail 是否从尾部节点检索（默认从头部检索）
 * @return LyBucketNode_t* 的节点指针
 */
LyBucketNode_t *lybucket_head(LyBucket_t *bucket, bool at_tail);

/**
 * @brief 获取桶中的节点
 *
 * @param bucket 桶
 * @param name 索引名
 * @return LyBucketNode_t* 节点指针
 */
LyBucketNode_t *lybucket_get(LyBucket_t *bucket, const char *const name);

/**
 * @brief 修改桶中的索引名
 *
 * @param bucket 桶
 * @param old_name 旧的索引名
 * @param new_name 新的索引名
 * @return bool 是否成功修改
 */
bool lybucket_patch_name(LyBucket_t *bucket, const char *const old_name, const char *const new_name);

/**
 * @brief 修改桶中的项指针
 *
 * @param bucket 桶
 * @param name 索引名
 * @param item 项
 * @return bool 是否成功修改
 */
bool lybucket_patch_item(LyBucket_t *bucket, const char *const name, void *item);

/**
 * @brief 修改桶中的元数据
 *
 * @param bucket 桶
 * @param name 索引名
 * @param meta 元数据
 * @return bool 是否成功修改
 */
bool lybucket_patch_meta(LyBucket_t *bucket, const char *const name, LyBucketMeta_t meta);

/**
 * @brief 从桶中移除项但不释放
 *
 * @param bucket 桶
 * @param name 索引名
 * @return LyBucketNode_t* 被移除的项
 */
LyBucketNode_t *lybucket_remove(LyBucket_t *bucket, const char *const name);

/**
 * @brief 从桶中移除一个节点
 *
 * @param bucket 桶
 * @param node 节点指针
 */
void lybucket_remove_node(LyBucket_t *bucket, LyBucketNode_t *node);

/**
 * @brief 删除桶中的节点
 *
 * @param bucket 桶
 * @param name 索引名
 * @return bool 是否成功修改
 */
bool lybucket_erase(LyBucket_t *bucket, const char *const name);

/**
 * @brief 销毁指定的桶
 *
 * @param bucket 桶
 * @param name 项的索引名
 */
void lybucket_delete(LyBucket_t *bucket);


/* -------------------------------- 不检查索引名，慎用 ------------------------------- */
/**
 * @brief 将一个项添加到桶
 *
 * @param bucket 桶
 * @param name 索引名
 * @param item 项
 * @param meta 元数据
 * @param at_head 是否从头部节点写入（默认尾部写入）
 * @return LyBucketNode_t* 生成的节点指针
 *
 * @note 可能造成索引名重复，不推荐用户直接使用该函数
 */
LyBucketNode_t *lybucket_post(LyBucket_t *bucket, const char *const name, void *item, LyBucketMeta_t meta, bool at_head);

/**
 * @brief 将一个项添加到桶
 *
 * @param bucket 桶
 * @param name 索引名
 * @param item 项
 * @param meta 元数据
 * @param at_name 定位索引名
 * @param at_before 是否在定位名之前写入（默认在之后写入）
 * @return LyBucketNode_t* 生成的节点指针
 *
 * @note 可能造成索引名重复，不推荐用户直接使用该函数
 */
LyBucketNode_t *lybucket_insert(LyBucket_t *bucket, const char *const name, void *item, LyBucketMeta_t meta, const char *const at_name, bool at_before);


#endif        /* __LYDDS_API__ */
