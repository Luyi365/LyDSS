/**
 * @file lybox.c
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


#include <string.h>
#include <malloc.h>
#include "lybox.h"

// box_len:可存储的最大项目数
lybox_t *lybox_create(enum lyboxType type, uint8_t box_len, uint8_t mbr_size)
{
    lybox_t *lybox = malloc(sizeof(lybox_t));

    lybox->type = type;
    lybox->len = box_len;
    lybox->count = 0;
    lybox->member_size = mbr_size;
    lybox->write_index = 0;
    lybox->read_index = 0;

    switch(type)
    {
    case lyboxType_QUEUE:
        lybox->read_mode = lyboxReadMode_FIFO;
        lybox->keep_fresh = false;
        break;
    case lyboxType_STACK:
        lybox->read_mode = lyboxReadMode_LIFO_CHUNK;
        lybox->keep_fresh = false;
        break;
    case lyboxType_STREAM:
        lybox->read_mode = lyboxReadMode_LIFO_STREAM;
        lybox->keep_fresh = true;
    default:
        break;
    }

    lybox->box = calloc(box_len, mbr_size);

    return lybox;
}

void lybox_delete(lybox_t *lybox)
{
    free(lybox->box);
    free(lybox);
    lybox = NULL;
}

void lybox_set_fresh(lybox_t *lybox, bool keep_fresh)
{
    lybox->keep_fresh = keep_fresh;
    lybox->type = lyboxType_CUSTOM;
}

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

    if(lybox->read_mode == lyboxReadMode_LIFO_STREAM || lybox->read_mode == lyboxReadMode_LIFO_CHUNK) {
        lybox->read_index = (uint8_t)(lybox->write_index + to_write - 1) % lybox->len;
    }
    else if(lybox->count + to_write > lybox->len) {
        lybox->read_index = (uint8_t)(lybox->write_index + to_write) % lybox->len;
    }
    lybox->write_index = (uint8_t)(lybox->write_index + to_write) % lybox->len;
    lybox->count = (to_write + lybox->count) > lybox->len ? lybox->len : to_write + lybox->count;

    return to_write;
}

uint8_t lybox_receive(lybox_t *lybox, void *pbuff, size_t num)
{
    uint8_t to_read = lybox_peek(lybox, pbuff, num);
    
    if(lybox->read_mode == lyboxReadMode_FIFO) {
        lybox->read_index = (uint8_t)(lybox->read_index + to_read) % lybox->len;
    }
    else if(lybox->read_mode == lyboxReadMode_LIFO_STREAM || lybox->read_mode == lyboxReadMode_LIFO_CHUNK) {
        lybox->write_index = (uint8_t)(lybox->read_index - to_read + 1) % lybox->len;
        lybox->read_index = (uint8_t)(lybox->read_index - to_read) % lybox->len;
    }

    return to_read;
}

// peek和receive的区别在于peek不会移动指针位置
// 返回实际读出的数据量
uint8_t lybox_peek(lybox_t *lybox, void *pbuff, size_t num)
{
    uint8_t to_read = (num > lybox->count) ? lybox->count : num;

    if(to_read <= 0 || num > lybox->count) return 0;

    if(lybox->read_mode == lyboxReadMode_FIFO) {
        // 分两种情况拷贝（是否跨越缓冲区末尾）
        uint8_t first_space = lybox->len - lybox->read_index; // 剩余可读空间
        if(to_read <= first_space) {
            memcpy(pbuff, lybox->box + lybox->read_index * lybox->member_size, to_read * lybox->member_size);
        }
        else {
            memcpy(pbuff, lybox->box + lybox->read_index * lybox->member_size, first_space * lybox->member_size);
            memcpy(pbuff + first_space * lybox->member_size, lybox->box, (to_read - first_space) * lybox->member_size);
        }
    }
    else if(lybox->read_mode == lyboxReadMode_LIFO_STREAM || lybox->read_mode == lyboxReadMode_LIFO_CHUNK) {
        // 分两种情况拷贝（是否跨越缓冲区末尾）
        uint8_t first_space = lybox->read_index + 1; // 剩余可读空间
        if(to_read <= first_space) {
            memcpy(pbuff, lybox->box + (lybox->read_index - (to_read - 1)) * lybox->member_size, to_read * lybox->member_size);
        }
        else {
            memcpy(pbuff, lybox->box + (lybox->len - to_read + first_space) * lybox->member_size, (to_read - first_space) * lybox->member_size);
            memcpy(pbuff + (to_read - first_space) * lybox->member_size, lybox->box, first_space * lybox->member_size);
        }
    }

    // 使用双指针交换成员顺序
    if(lybox->read_mode == lyboxReadMode_LIFO_STREAM) {
        void *left = pbuff;
        void *right = left + (num - 1) * lybox->member_size;
        void *temp = malloc(lybox->member_size);

        while(left < right) {
        // 交换左右指针指向的成员
            memcpy(temp, left, lybox->member_size);
            memcpy(left, right, lybox->member_size);
            memcpy(right, temp, lybox->member_size);

            // 移动指针
            left += lybox->member_size;
            right -= lybox->member_size;
        }
    }

    lybox->count -= to_read;

    return to_read;
}