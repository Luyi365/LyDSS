/**
 * @file lypool.h
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

#ifndef __LYPOOL_
#define __LYPOOL_

#include <stdint.h> 
#include <stddef.h>

typedef struct {
    size_t length;      ///< 池大小
    struct lypool_item {
        const char *name;
        void *object;
    }items[];
}lypool_t;

/* ---------------------------------- 便捷接口 ---------------------------------- */


/* ---------------------------------- 基础功能 ---------------------------------- */
/**
 * @brief 创建一个池
 *
 * @param len 池容量
 * @return lypool_t*
 */
lypool_t *lypool_create(size_t len);

/**
 * @brief 将一个对象引入池
 *
 * @param pool 池
 * @param name 对象名称
 * @param obj 对象
 */
void lypool_post(lypool_t *pool, const char *const name, void *obj);

/**
 * @brief 获取池中的指定对象
 *
 * @param pool 池
 * @param name 对象名称
 * @return 对象
 */
void *lypool_get(lypool_t *pool, const char *const name);

/**
 * @brief 清除池中的指定对象
 *
 * @param pool 池
 * @param name 对象名称
 */
void lypool_clear(lypool_t *pool, const char *const name);

/**
 * @brief 销毁指定的池
 *
 * @param pool 池
 * @param name 池名称
 */
void lypool_delete(lypool_t *pool);

#endif        /* __LYPOOL_ */
