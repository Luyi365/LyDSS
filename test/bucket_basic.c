/**
 * @file bucket_basic.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief 桶用例
 * @date 2026-08-11
 *
 * @copyright Copyright (c) 2026
 *
 * @note LyBucketNode_t 是不完整类型，用例只能校验检索结果与节点指针的同一性。
 */

#include "../inc/lydss_api.h"

#if LYDSS_BUCKET_ENABLED

#include <stdio.h>

int bucket_basic(void)
{
    int items[3] = { 10, 11, 12 };
    unsigned extra = 0xA55A;
    LyBucketMeta_t none = { LyBucketMetaType_NULL, NULL, 0 };
    LyBucketMeta_t attach = { LyBucketMetaType_ATTACH, &extra, sizeof(extra) };

    LyBucket_t *bucket = lybucket_create();

    /* 桶的特征：按名字检索，同时保持顺序，每项可携带元数据 */
    lybucket_put(bucket, "a", &items[0], none);
    lybucket_put(bucket, "b", &items[1], attach);
    lybucket_post(bucket, "c", &items[2], none, true);
    printf("放入 a = %d、b = %d（附带元数据 0x%X），再在头部放入 c = %d\n",
        items[0], items[1], extra, items[2]);

    printf("查 b：%s\n", lybucket_get(bucket, "b") != NULL ? "找到" : "未找到");
    printf("头部是 c：%s\n", lybucket_head(bucket, false) == lybucket_get(bucket, "c") ? "是" : "否");

    bool erased = lybucket_erase(bucket, "a");
    printf("删除 a：%s，再查 a：%s\n", erased ? "成功" : "失败",
        lybucket_get(bucket, "a") != NULL ? "找到" : "未找到");

    LyBucketNode_t *taken = lybucket_remove(bucket, "b");
    printf("摘下 b：%s，由调用者释放\n", taken != NULL ? "成功" : "失败");
    lybucket_free_node(taken);

    lybucket_delete(bucket);

    return 0;
}

#endif /* LYDSS_BUCKET_ENABLED */
