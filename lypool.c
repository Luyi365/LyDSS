/**
 * @file lypool.c
 * @author Luyi_2333 (luyi.hub@foxmail.com)
 * @brief
 * @date 2025-01-06
 *
 * @copyright Copyright (c) 2024 Luyi_2333
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
 * 池，可以容纳一切事物.
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
#include "lypool.h"

/* ---------------------------------- 核心代码 ---------------------------------- */
/// @brief 创建一个池
lypool_t *lypool_create(size_t len)
{
    lypool_t *pool = NULL;

    pool = calloc(1, sizeof(lypool_t) + sizeof(struct lypool_item) * len);
    pool->length = len;

    return pool;
}

/// @brief 将一个对象引入池
void lypool_post(lypool_t *pool, const char *const name, void *obj)
{
    if(pool == NULL || name == NULL) {

    }
    else {
        for(size_t i = 0; i < pool->length; i++) {
            if(pool->items[i].name == NULL) {
                pool->items[i].name = name;
                pool->items[i].object = obj;
                return;
            }
        }
        // TODO: 满时，需要处理
    }
}

/// @brief 获取池中的指定对象
void *lypool_get(lypool_t *pool, const char *const name)
{
    if(pool == NULL) {

    }
    else {
        for(size_t i = 0; i < pool->length; i++) {
            if(!strcmp(pool->items[i].name, name)) {
                return pool->items[i].object;
            }
        }
        // TODO: 找不到？
    }

    return NULL;
}

/// @brief 清除池中的指定对象
void lypool_clear(lypool_t *pool, const char *const name)
{
    if(pool == NULL) {

    }
    else {
        for(size_t i = 0; i < pool->length; i++) {
            if(pool->items[i].name == name) {
                pool->items[i].name = NULL;
                pool->items[i].object = NULL;
            }
        }
        // TODO: 找不到？
    }
}

/// @brief 销毁指定的池
void lypool_delete(lypool_t *pool)
{
    if(pool == NULL) {

    }
    else {
        for(size_t i = 0; i < pool->length; i++) {
            lypool_clear(pool, pool->items[i].name);
        }
        free(pool);
    }
}

/* ---------------------------------- 基本功能 ---------------------------------- */
