/**
 * @file lypool.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief
 * @date 2025-01-06
 *
 * @copyright Copyright (c) 2026 Luyi365
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @note
 *
 *
 *  *  ------------------
 * /  如果代码有帮助到你  \
 * \  也请作者喝杯咖啡吧  /
 *  ------------------
 *      \   (__)
 *       \  /oo|\_______
 *         (_"_)\  牛码  )\/\
 *  █████   U   ||-----j/  ███████████     ███████       ███████    █████
 * ░░███    '   ||     || ░░███░░░░░███  ███░░░░░███   ███░░░░░███ ░░███
 *  ░███    '   █████ ████ ░███    ░███ ███     ░░███ ███     ░░███ ░███
 *  ░███    ;  ░░███ ░███  ░██████████ ░███      ░███░███      ░███ ░███
 *  ░███ ~~~^~~;░███ ░███  ░███░░░░░░  ░███      ░███░███      ░███ ░███
 *  ░███ ~~~~~█;░███ ░███  ░███        ░░███     ███ ░░███     ███  ░███      █
 *  ███████████;░░███████  █████        ░░░███████░   ░░░███████░   ███████████
 * ░░░░░░░░░░░;; ░░░░░███  ░░░░░           ░░░░░░░       ░░░░░░░    ░░░░░░░░░░░
 *          ;;;  ███ ░███
 *          ;;  ░░██████
 *           ;   ░░░░░░
 */


#include "../inc/lydss_api.h"

#if LYDSS_POOL_ENABLED

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

struct LyPool {
    size_t size;                    ///< 池大小，也就是存放的对象数量
#if LYDSS_POOL_STATIC_ENABLED
    bool is_static;                 ///< 对象数组是否由调用者提供
#endif
    LyPoolItem_t *items;            ///< 对象数组
};

#if LYDSS_POOL_STATIC_ENABLED
_Static_assert(sizeof(struct LyPool) <= sizeof(LyPoolStatic_t)
               && _Alignof(struct LyPool) <= _Alignof(LyPoolStatic_t),
               "LyPoolStatic_t is too small or under-aligned for struct LyPool");
#endif

/// @brief 创建一个池
LyPool_t *lypool_create(size_t size)
{
    LyPool_t *pool = NULL;

    pool = malloc(sizeof(LyPool_t));
    if(pool == NULL) {
        return NULL;
    }

    pool->items = calloc(size, sizeof(LyPoolItem_t));
    if(pool->items == NULL) {
        free(pool);
        return NULL;
    }

    pool->size = size;
#if LYDSS_POOL_STATIC_ENABLED
    pool->is_static = false;
#endif

    return pool;
}

#if LYDSS_POOL_STATIC_ENABLED
/// @brief 创建一个静态池
LyPool_t *lypool_create_static(LyPoolStatic_t *storage, LyPoolItem_t *items, size_t size)
{
    LYDSS_ASSERT(storage);
    LYDSS_ASSERT(items);

    for(size_t i = 0; i < size; i++) {
        items[i].name = NULL;
        items[i].object = NULL;
    }

    LyPool_t *pool = (LyPool_t *)storage;
    pool->items = items;
    pool->size = size;
    pool->is_static = true;

    return pool;
}
#endif

/// @brief 将一个对象引入池
bool lypool_post(LyPool_t *pool, const char *const name, void *obj)
{
    LYDSS_ASSERT(pool);
    LYDSS_ASSERT(name);

    for(size_t i = 0; i < pool->size; i++) {
        if(pool->items[i].name == NULL) {
            pool->items[i].name = name;
            pool->items[i].object = obj;
            return true;
        }
    }

    return false;
}

/// @brief 获取池中的指定对象
void *lypool_get(LyPool_t *pool, const char *const name)
{
    LYDSS_ASSERT(pool);
    LYDSS_ASSERT(name);

    for(size_t i = 0; i < pool->size; i++) {
        if(pool->items[i].name != NULL
            && strcmp(pool->items[i].name, name) == 0) {
            return pool->items[i].object;
        }
    }

    return NULL;
}

/// @brief 清除池中的指定对象，同名槽位会被全部清掉
/// @note post 不查重、get 只取第一个、本函数清全部，三者对重名的处理不一致。
void lypool_clear(LyPool_t *pool, const char *const name)
{
    LYDSS_ASSERT(pool);
    LYDSS_ASSERT(name);

    for(size_t i = 0; i < pool->size; i++) {
        if(pool->items[i].name != NULL
            && strcmp(pool->items[i].name, name) == 0) {
            pool->items[i].name = NULL;
            pool->items[i].object = NULL;
        }
    }
}

/// @brief 销毁指定的池
void lypool_delete(LyPool_t *pool)
{
    LYDSS_ASSERT(pool);

    for(size_t i = 0; i < pool->size; i++) {
        pool->items[i].name = NULL;
        pool->items[i].object = NULL;
    }

#if LYDSS_POOL_STATIC_ENABLED
    if(pool->is_static == true) {
        pool->items = NULL;
        pool->size = 0;
        return;
    }
#endif

    free(pool->items);
    free(pool);
}

#endif /* LYDSS_POOL_ENABLED */
