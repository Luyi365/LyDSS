/**
 * @file lybox.c
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


#include <stdint.h>
#include <string.h>
#include <malloc.h>
#include <stdbool.h>
#include "lybox.h"

/**
 * @brief 创建一个容器
 * 
 * @param box_len 容器长度（最大成员数）
 * @param mbr_size 单个成员大小
 * @param r_mode 读取模式
 * @param fresh 是否保持数据新鲜
 * @return lybox_t* 返回一个容器对象
 */
lybox_t *lybox_create(uint8_t box_len, uint8_t mbr_size, enum LyboxReadMode r_mode, bool fresh)
{
    lybox_t *lybox = malloc(sizeof(lybox_t));

    lybox->len = box_len;
    lybox->count = 0;
    lybox->member_size = mbr_size;
    lybox->write_index = 0;
    lybox->read_index = 0;

    lybox->read_mode = r_mode;
    lybox->keep_fresh = fresh;

    lybox->box = calloc(box_len, mbr_size);

    return lybox;
}

/**
 * @brief 销毁容器
 * 
 * @param lybox 容器对象
 */
void lybox_delete(lybox_t *lybox)
{
    free(lybox->box);
    free(lybox);
    lybox = NULL;
}

/**
 * @brief 设置容器数据覆盖方式
 * 
 * @param lybox 容器对象
 * @param fresh 是否保持数据新鲜
 */
void lybox_set_fresh(lybox_t *lybox, bool fresh)
{
    lybox->keep_fresh = fresh;
}

/**
 * @brief 向容器中存入数据
 * 
 * @param lybox 容器对象
 * @param pval 数据指针
 * @param num 数据个数
 * @return uint8_t 返回实际存入的数量
 */
uint8_t lybox_send(lybox_t *lybox, void *pval, size_t num)
{
    uint8_t to_write = 0;
    uint8_t offset = 0;

    // 实际写入数量
    if(lybox->keep_fresh) {
        to_write = (num > lybox->len) ? lybox->len : num;
        if(num > lybox->len) {
            offset = (num - to_write) * lybox->member_size;
            lybox->write_index = (lybox->write_index - (lybox->len - (num % lybox->len))) % lybox->len;
        }
    }
    else {
        to_write = (num > (lybox->len - lybox->count)) ? (lybox->len - lybox->count) : num;
    }

    if(to_write <= 0) return 0;

    // 分两种情况拷贝（是否跨越缓冲区末尾）
    uint8_t first_space = lybox->len - lybox->write_index;
    if(to_write <= first_space) {
        memcpy(lybox->box + lybox->write_index * lybox->member_size, pval + offset, to_write * lybox->member_size);
    }
    else {
        memcpy(lybox->box + lybox->write_index * lybox->member_size, pval + offset, first_space * lybox->member_size);
        memcpy(lybox->box, pval + offset + first_space * lybox->member_size, (to_write - first_space) * lybox->member_size);
    }

    if(lybox->read_mode == LyboxReadMode_LIFO_BYTE || lybox->read_mode == LyboxReadMode_LIFO_BLOCK) {
        lybox->read_index = (uint8_t)(lybox->write_index + to_write - 1) % lybox->len;
    }
    else if(lybox->count + to_write > lybox->len) {
        lybox->read_index = (uint8_t)(lybox->write_index + to_write) % lybox->len;
    }
    lybox->write_index = (uint8_t)(lybox->write_index + to_write) % lybox->len;
    lybox->count = (to_write + lybox->count) > lybox->len ? lybox->len : to_write + lybox->count;

    return to_write;
}

/**
 * @brief 从容器中取出数据
 * 
 * @param lybox 容器对象
 * @param pbuff 缓存指针
 * @param num 数据个数
 * @return uint8_t 返回实际取出的数量
 */
uint8_t lybox_receive(lybox_t *lybox, void *pbuff, size_t num)
{
    uint8_t to_read = lybox_peek(lybox, pbuff, num);

    if(lybox->read_mode == LyboxReadMode_FIFO) {
        lybox->read_index = (uint8_t)(lybox->read_index + to_read) % lybox->len;
    }
    else if(lybox->read_mode == LyboxReadMode_LIFO_BYTE || lybox->read_mode == LyboxReadMode_LIFO_BLOCK) {
        lybox->write_index = (uint8_t)(lybox->read_index - to_read + 1) % lybox->len;
        lybox->read_index = (uint8_t)(lybox->read_index - to_read) % lybox->len;
    }

    return to_read;
}

/**
 * @brief 预读容器中的数据
 * 
 * @param lybox 容器对象
 * @param pbuff 缓存指针
 * @param num 数据个数
 * @return uint8_t 返回实际读到的数量
 * 
 * @note peek和receive本质的区别在于peek不会移动指针位置
 */
uint8_t lybox_peek(lybox_t *lybox, void *pbuff, size_t num)
{
    uint8_t to_read = (num > lybox->count) ? lybox->count : num;

    if(to_read <= 0 || num > lybox->count) return 0;

    if(lybox->read_mode == LyboxReadMode_FIFO) {
        uint8_t first_space = lybox->len - lybox->read_index;
        if(to_read <= first_space) {
            memcpy(pbuff, lybox->box + lybox->read_index * lybox->member_size, to_read * lybox->member_size);
        }
        else {
            memcpy(pbuff, lybox->box + lybox->read_index * lybox->member_size, first_space * lybox->member_size);
            memcpy(pbuff + first_space * lybox->member_size, lybox->box, (to_read - first_space) * lybox->member_size);
        }
    }
    else if(lybox->read_mode == LyboxReadMode_LIFO_BYTE || lybox->read_mode == LyboxReadMode_LIFO_BLOCK) {
        uint8_t first_space = lybox->read_index + 1;
        if(to_read <= first_space) {
            memcpy(pbuff, lybox->box + (lybox->read_index - (to_read - 1)) * lybox->member_size, to_read * lybox->member_size);
        }
        else {
            memcpy(pbuff, lybox->box + (lybox->len - to_read + first_space) * lybox->member_size, (to_read - first_space) * lybox->member_size);
            memcpy(pbuff + (to_read - first_space) * lybox->member_size, lybox->box, first_space * lybox->member_size);
        }
    }

    // 使用双指针交换成员顺序
    if(lybox->read_mode == LyboxReadMode_LIFO_BYTE) {
        void *left = pbuff;
        void *right = left + (num - 1) * lybox->member_size;
        void *temp = malloc(lybox->member_size);

        while(left < right) {
            memcpy(temp, left, lybox->member_size);
            memcpy(left, right, lybox->member_size);
            memcpy(right, temp, lybox->member_size);

            left += lybox->member_size;
            right -= lybox->member_size;
        }
    }

    lybox->count -= to_read;

    return to_read;
}