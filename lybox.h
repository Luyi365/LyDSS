/**
 * @file lybox.h
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief
 * @date 2025-03-18
 *
 * @copyright Copyright (c) 2024 Luyi365
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
 * 编程的本质即是数据的搬运
 *
 *
 *  ------------------
 * /  如果代码有帮助到你  \
 * \  也请作者喝杯咖啡吧  /
 *  ------------------
 *      \   (__)
 *       \  /oo|\_______
 *         (_"_)\  牛码  )\/\
 *  █████   U   ||-----j/  ███████████     ███████    █████ █████
 * ░░███    '   ||     || ░░███░░░░░███  ███░░░░░███ ░░███ ░░███ 
 *  ░███    '   █████ ████ ░███    ░███ ███     ░░███ ░░███ ███  
 *  ░███    ;  ░░███ ░███  ░██████████ ░███      ░███  ░░█████  
 *  ░███ ~~~^~~;░███ ░███  ░███░░░░░███░███      ░███   ███░███  
 *  ░███ ~~~~~█;░███ ░███  ░███    ░███░░███     ███   ███ ░░███ 
 *  ███████████;░░███████  ███████████  ░░░███████░   █████ █████
 * ░░░░░░░░░░░;; ░░░░░███ ░░░░░░░░░░░     ░░░░░░░    ░░░░░ ░░░░░ 
 *          ;;;  ███ ░███
 *          ;;  ░░██████
 *           ;   ░░░░░░                                       
 */


#ifndef __LYBOX_H__
#define __LYBOX_H__

#include <stdint.h> 
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

/* LyBOX */
typedef struct lybox lybox_t;
typedef int(__cdecl func_sort_t)(const void *, const void *);

enum lyboxType {
    lyboxType_QUEUE = 0,
    lyboxType_STACK,
    lyboxType_STREAM,

    lyboxType_CUSTOM,    ///< 自定义类型
    lyboxType_NUMBER,
};

struct lybox {
    enum lyboxType type;	///< 容器类型
    uint8_t len;    ///< 容器长度
    size_t count;    ///< 容器中成员实际数量

    uint8_t member_size;    ///< 单个成员的大小（单位：字节）
    // 注意：下面两个索引存在无效值UINT8_MAX
    uint8_t write_index;    ///< 写索引（指向下一个可写入的位置索引）
    uint8_t read_index;     ///< 读索引（指向下一个可读取的位置索引）


// 对于环形缓冲来说，追赶的方式不区分流和块，因为这两种方式最后得到的结果是一样的
// 但追赶的方式固定为FIFO，很多时候也需要FILO，因此需要引进栈的方式（但这个栈有点像是相向而行的环形缓冲），但同时也需要实现块的方式
    enum lyboxReadMode {
        lyboxReadMode_FIFO,
        lyboxReadMode_LIFO_STREAM,
        lyboxReadMode_LIFO_CHUNK,
    } read_mode;
    bool keep_fresh;    ///< 是否覆盖写入

    void *box;     ///< 容器
};

lybox_t *lybox_create(enum lyboxType type, uint8_t box_len, uint8_t mbr_size);
void lybox_delete(lybox_t *lybox);
void lybox_set_fresh(lybox_t *lybox, bool keep_fresh);
uint8_t lybox_send(lybox_t *lybox, void *pval, size_t num);
uint8_t lybox_receive(lybox_t *lybox, void *pbuff, size_t num);
uint8_t lybox_peek(lybox_t *lybox, void *pbuff, size_t num);

#endif /* __LYBOX_H__ */