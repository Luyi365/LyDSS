/**
 * @file lybox.h
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief
 * @date 2025-03-18
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
 * 代码的本质即是数据的搬运
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
#include <stdbool.h>
#include <stdlib.h>

/* LyBOX */
typedef struct LyBOX LyBOX_t;

typedef enum LyboxWriteMode {
    LyboxWriteMode_Default,      ///< 满了则不能再写入
    LyboxWriteMode_Fresh,        ///< 满了则从头部以保持数据最新的方式写入
} LyboxWriteMode_t;

/** @note 对于环形缓冲来说，追赶的方式不区分流和块，因为这两种方式最后得到的结果是一样的；
 * 但追赶的方式固定为FIFO，很多时候也需要LIFO，因此需要引进栈的方式（这个栈是通过相向而行的环形缓冲来实现的）。
 */
typedef enum LyboxReadMode {
    LyboxReadMode_FIFO,         ///< 先出先进
    LyboxReadMode_LIFOUnit,     ///< 后进先出（以成员大小为单位）
    LyboxReadMode_LIFOBlock,    ///< 后进先出（以块大小为单位）
} LyboxReadMode_t;

struct LyBOX {
    uint8_t len;    ///< 容器长度
    size_t count;   ///< 容器中成员实际数量
    
    uint8_t member_size; ///< 单个成员的大小（单位：字节）
    uint8_t front_index;     ///< 队首索引
    uint8_t write_index;     ///< 写索引
    /// @note 这里为什么不是将 front & rear 或者 write & read 成对使用？
    ///       因为这个容器的写入永远是从尾部开始，但如果采用的是尾成员而不是写入位置，那么就会出现在初始化还没有写入数据时，尾成员是不存在的问题，导致找不到写入位置。
    
    void *box;      ///< 容器
};

/**
 * @brief 创建一个容器
 *
 * @param box_len 容器长度（最大成员个数）
 * @param member_size 单个成员大小
 * @param r_mode 读取模式
 * @return LyBOX_t* 返回一个容器对象
 */
LyBOX_t *lybox_create(uint8_t box_len, uint8_t mbr_size);

/**
 * @brief 销毁容器
 *
 * @param lybox 容器对象
 */
void lybox_delete(LyBOX_t *lybox);

/**
 * @brief 向容器中存入数据
 *
 * @param lybox 容器对象
 * @param pval 数据指针
 * @param num 数据个数
 * @param w_mode 写入模式
 * @return uint8_t 返回实际存入的数量
 */
uint8_t lybox_send(LyBOX_t *lybox, void *pval, size_t num, LyboxWriteMode_t w_mode);

#define LYBOX_SEND_DEFAULT(lybox, pval, num) \
lybox_send(lybox, pval, num, LyboxWriteMode_Default)
#define LYBOX_SEND_FRESH(lybox, pval, num) \
lybox_send(lybox, pval, num, LyboxWriteMode_Fresh)

/**
 * @brief 从容器中取出数据
 *
 * @param lybox 容器对象
 * @param pbuff 缓存指针
 * @param num 数据个数
 * @param r_mode 读取模式
 * @return uint8_t 返回实际取出的数量
 */
uint8_t lybox_receive(LyBOX_t *lybox, void *pbuff, size_t num, enum LyboxReadMode r_mode);

/**
 * @brief 预读容器中的数据
 *
 * @param lybox 容器对象
 * @param pbuff 缓存指针
 * @param num 数据个数
 * @param r_mode 读取模式
 * @return uint8_t 返回实际读到的数量
 *
 * @note peek和receive本质的区别在于peek不会移动指针位置
 */
uint8_t lybox_peek(LyBOX_t *lybox, void *pbuff, size_t num, enum LyboxReadMode r_mode);


#endif /* __LYBOX_H__ */
