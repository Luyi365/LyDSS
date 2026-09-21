/**
 * @file lybucket.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief
 * @date 2025-08-28
 *
 * @copyright Copyright (c) 2026 Luyi365
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
 * LyBucket的核心是list + table结构，并引入REST接口请求方法
 */


#include "../inc/lydss_api.h"

#if LYDSS_BUCKET_ENABLED

#include "../inc/lydss_priv.h"

#include <stdlib.h>
#include <string.h>

struct LyBucketNode {
    const char *name;               ///< 索引名
    void *item;                     ///< 项指针
    LyBucketMeta_t meta;            ///< 元数据（均为拷贝存储）
    LyBucketNode_t *prev;           ///< 前驱节点
    LyBucketNode_t *next;           ///< 后驱节点
};

struct LyBucket {
    uint8_t count;                  ///< 桶中项的实际数量
    LyBucketNode_t *head_item;      ///< 指向桶首成员
    LyBucketNode_t *tail_item;      ///< 指向桶尾成员
};

static void *bucket_dup_memory(const void *src, size_t size);

/* ---------------------------------- 检查索引名 --------------------------------- */
/// @brief 创建一个桶
LyBucket_t *lybucket_create(void)
{
    LyBucket_t *bucket = malloc(sizeof(LyBucket_t));
    if(bucket == NULL) {
        return NULL;
    }

    *bucket = (LyBucket_t){
        .head_item = NULL,
        .tail_item = NULL,
        .count = 0
    };

    return bucket;
}

/// @brief 将一个项放到桶中
/// @note 遇到同名则替换，无同名则增加
LyBucketNode_t *lybucket_put(LyBucket_t *bucket, const char *const name, void *item, LyBucketMeta_t meta)
{
    LYDSS_ASSERT(bucket);
    LYDSS_ASSERT(name);

    LyBucketNode_t *cur = lybucket_get(bucket, name);

    if(cur == NULL) {
        // 添加新项
        cur = lybucket_post(bucket, name, item, meta, 0);
    }
    else {
        // 更新现有项
        if(cur->meta.data != NULL) {
            free(cur->meta.data);
        }
        switch(meta.type)
        {
            case LyBucketMetaType_NULL: cur->meta.data = NULL; break;
            case LyBucketMetaType_COPY: cur->meta.data = bucket_dup_memory(item, meta.size); break;
            case LyBucketMetaType_ATTACH: cur->meta.data = bucket_dup_memory(meta.data, meta.size); break;
        }
        cur->item = item;
        cur->meta.type = meta.type;
        cur->meta.size = meta.size;
    }

    return cur;
}

/// @brief 得到最近的一个节点
LyBucketNode_t *lybucket_head(LyBucket_t *bucket, bool at_tail)
{
    LYDSS_ASSERT(bucket);

    if(at_tail == true) {
        return bucket->tail_item;
    }
    else {
        return bucket->head_item;
    }
}

/// @brief 获取桶中的节点
LyBucketNode_t *lybucket_get(LyBucket_t *bucket, const char *const name)
{
    LYDSS_ASSERT(bucket);
    LYDSS_ASSERT(name);

    LyBucketNode_t *cur = bucket->head_item;
    for(uint32_t i = 0; cur != NULL && i < bucket->count; cur = cur->next, i++) {
        if(cur->name != NULL && strcmp(cur->name, name) == 0) {
            return cur; // 找到匹配项
        }
    }

    return NULL;
}

/// @brief 修改桶中的索引名
bool lybucket_patch_name(LyBucket_t *bucket, const char *const old_name, const char *const new_name)
{
    LYDSS_ASSERT(bucket);
    LYDSS_ASSERT(old_name);
    LYDSS_ASSERT(new_name);

    LyBucketNode_t *cur = lybucket_get(bucket, old_name);
    if(cur != NULL) {
        cur->name = new_name;
        return true;
    }

    return false;
}

/// @brief 修改桶中的项指针
bool lybucket_patch_item(LyBucket_t *bucket, const char *const name, void *item)
{
    LYDSS_ASSERT(bucket);
    LYDSS_ASSERT(name);
    LYDSS_ASSERT(item);

    LyBucketNode_t *cur = lybucket_get(bucket, name);
    if(cur != NULL) {
        cur->item = item;
        return true;
    }

    return false;
}

/// @brief 修改桶中的元数据
bool lybucket_patch_meta(LyBucket_t *bucket, const char *const name, LyBucketMeta_t meta)
{
    LYDSS_ASSERT(bucket);
    LYDSS_ASSERT(name);

    LyBucketNode_t *cur = lybucket_get(bucket, name);
    if(cur != NULL) {
        if(cur->meta.data != NULL) {
            free(cur->meta.data);
        }
        switch(meta.type)
        {
            case LyBucketMetaType_NULL: meta.data = NULL; break;
            case LyBucketMetaType_COPY: meta.data = bucket_dup_memory(cur->item, meta.size); break;
            case LyBucketMetaType_ATTACH: meta.data = bucket_dup_memory(meta.data, meta.size); break;
        }
        cur->meta = meta;
        return true;
    }

    return false;
}

/// @brief 从桶中移除项但不释放（返回被移除的项）
LyBucketNode_t *lybucket_remove(LyBucket_t *bucket, const char *const name)
{
    LYDSS_ASSERT(bucket);
    LYDSS_ASSERT(name);

    LyBucketNode_t *cur = lybucket_get(bucket, name);
    if(cur != NULL) {
        lybucket_remove_node(bucket, cur);
    }

    return cur;
}

/// @brief 从桶中移除一个节点
void lybucket_remove_node(LyBucket_t *bucket, LyBucketNode_t *node)
{
    LYDSS_ASSERT(bucket);
    LYDSS_ASSERT(node);

    LYDLIST_UNLINK(bucket->head_item, bucket->tail_item, node);
    bucket->count--;
}

/// @brief 删除桶中的节点
bool lybucket_erase(LyBucket_t *bucket, const char *const name)
{
    LYDSS_ASSERT(bucket);
    LYDSS_ASSERT(name);

    LyBucketNode_t *cur = lybucket_remove(bucket, name);
    if(cur != NULL) {
        if(cur->meta.data != NULL) {
            free(cur->meta.data);
        }
        free(cur);
        return true;
    }

    return false;
}

/// @brief 销毁指定的桶
void lybucket_delete(LyBucket_t *bucket)
{
    LYDSS_ASSERT(bucket);

    LyBucketNode_t *cur = bucket->head_item;
    while(cur != NULL) {
        LyBucketNode_t *next = cur->next;
        if(cur->meta.data != NULL) {
            free(cur->meta.data);
        }
        free(cur);
        cur = next;
    }

    free(bucket);
}

/* -------------------------------- 不检查索引名，慎用 ------------------------------- */
/// @brief 将一个项添加到桶
LyBucketNode_t *lybucket_post(LyBucket_t *bucket, const char *const name, void *item, LyBucketMeta_t meta, bool at_head)
{
    LYDSS_ASSERT(bucket);

    LyBucketNode_t *node = malloc(sizeof(LyBucketNode_t));
    if(node == NULL) {
        return NULL;
    }

    void *tmp = NULL;
    switch(meta.type)
    {
        case LyBucketMetaType_NULL: tmp = NULL; break;
        case LyBucketMetaType_COPY: tmp = bucket_dup_memory(item, meta.size); break;
        case LyBucketMetaType_ATTACH: tmp = bucket_dup_memory(meta.data, meta.size); break;
    }

    *node = (LyBucketNode_t){
        .name = name,
        .item = item,
        .meta.type = meta.type,
        .meta.data = tmp,
        .meta.size = meta.size,
        .prev = NULL,
        .next = NULL,
    };

    if(at_head == true) {
        LYDLIST_LINK_FRONT(bucket->head_item, bucket->tail_item, node);
    }
    else {
        LYDLIST_LINK_REAR(bucket->head_item, bucket->tail_item, node);
    }

    bucket->count++;
    return node;
}

/// @brief 将一个项插入到桶指定位置
LyBucketNode_t *lybucket_insert(LyBucket_t *bucket, const char *const name, void *item, LyBucketMeta_t meta, const char *const at_name, bool at_before)
{
    LYDSS_ASSERT(bucket);
    LYDSS_ASSERT(at_name);

    LyBucketNode_t *cur = lybucket_get(bucket, at_name);
    if(cur != NULL) {
        LyBucketNode_t *node = malloc(sizeof(LyBucketNode_t));
        if(node == NULL) {
            return NULL;
        }

        void *tmp = NULL;
        switch(meta.type)
        {
            case LyBucketMetaType_NULL: tmp = NULL; break;
            case LyBucketMetaType_COPY: tmp = bucket_dup_memory(item, meta.size); break;
            case LyBucketMetaType_ATTACH: tmp = bucket_dup_memory(meta.data, meta.size); break;
        }

        *node = (LyBucketNode_t){
            .name = name,
            .item = item,
            .meta.type = meta.type,
            .meta.data = tmp,
            .meta.size = meta.size,
            .prev = NULL,
            .next = NULL,
        };

        if(at_before == true) {
            LYDLIST_LINK_BEFORE(bucket->head_item, cur, node);
        }
        else {
            LYDLIST_LINK_AFTER(bucket->tail_item, cur, node);
        }

        bucket->count++;
        return node;
    }

    return NULL;
}

/* --------------------------------- static --------------------------------- */
/// @brief 分配内存并复制内容（返回新分配的内存指针）
static void *bucket_dup_memory(const void *src, size_t size)
{
    LYDSS_ASSERT(src);
    LYDSS_ASSERT(size);

    void *dest = malloc(size);
    if(dest == NULL) {
        return NULL;
    }

    return memcpy(dest, src, size);
}

#endif /* LYDSS_BUCKET_ENABLED */
