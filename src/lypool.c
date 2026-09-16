/**
 * @file lypool.c
 * @author Luyi_2333 (luyi.hub@foxmail.com)
 * @brief
 * @date 2025-01-06
 *
 * @copyright Copyright (c) 2026 Luyi_2333
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


#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "../inc/lydss_api.h"

struct LyPool {
    size_t size;                    ///< 池大小，也就是存放的对象数量
    bool is_static;                 ///< 对象数组是否由调用者提供

    struct LyPoolItem {
        const char *name;           ///< 对象名称
        void *object;               ///< 对象指针
    } *items;                       ///< 对象数组
};

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
    pool->is_static = false;

    return pool;
}

/// @brief 创建一个静态池
void lypool_create_static(LyPool_t *pool, LyPoolItem_t *items, size_t size)
{
    LYDSS_ASSERT(pool);
    LYDSS_ASSERT(items);

    for(size_t i = 0; i < size; i++) {
        items[i].name = NULL;
        items[i].object = NULL;
    }

    pool->items = items;
    pool->size = size;
    pool->is_static = true;
}

/// @brief 将一个对象引入池
void lypool_post(LyPool_t *pool, const char *const name, void *obj)
{
    LYDSS_ASSERT(pool);
    LYDSS_ASSERT(name);

    for(size_t i = 0; i < pool->size; i++) {
        if(pool->items[i].name == NULL) {
            pool->items[i].name = name;
            pool->items[i].object = obj;
            return;
        }
    }
    // TODO: 满时，需要处理
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
    // TODO: 找不到？

    return NULL;
}

/// @brief 清除池中的指定对象
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
    // TODO: 找不到？
}

/// @brief 销毁指定的池
/// @note 静态池只解除对象数组绑定，不释放调用者提供的内存与句柄
void lypool_delete(LyPool_t *pool)
{
    LYDSS_ASSERT(pool);

    for(size_t i = 0; i < pool->size; i++) {
        pool->items[i].name = NULL;
        pool->items[i].object = NULL;
    }

    if(pool->is_static == true) {
        pool->items = NULL;
        pool->size = 0;
        return;
    }

    free(pool->items);
    free(pool);
}
