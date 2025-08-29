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


/// @brief 创建一个容器
LyBOX_t *lybox_create(uint8_t box_len, uint8_t mbr_size)
{
    LyBOX_t *lybox = malloc(sizeof(LyBOX_t));
    
    lybox->len = box_len;
    lybox->count = 0;
    lybox->member_size = mbr_size;
    lybox->begin_index = 0;
    lybox->end_index = 0;
    
    lybox->box = calloc(box_len, mbr_size);
    
    return lybox;
}

/// @brief 销毁容器
void lybox_delete(LyBOX_t *lybox)
{
    free(lybox->box);
    lybox->box = NULL;
    free(lybox);
    lybox = NULL;
}

/// @brief 向容器中存入数据
uint8_t lybox_send(LyBOX_t *lybox, void *pval, size_t num, LyboxWriteMode_t w_mode)
{
    uint8_t to_write = 0;
    uint8_t write_offset_idx = lybox->end_index; // 实际写入位置
    uint8_t pval_offset_idx = 0; // 读取pval数据起始偏移量，只用于FRESH模式，来避免数据重复计算
    // 例如：容器长度5，想写入9个成员
    // 因为容器实际只能装下5个成员，9个成员中4个是多余的，需要跳过
    // 此时offset_idx = 9 - 5 = 4   [4]就是写入数据的起始索引偏移量
    
    // 实际写入数量
    if(w_mode == LyboxWriteMode_Fresh) {   // 因为采用避免重复计算的方式，所以只有FRESH模式在写入时的写入位置与尾索引不同
        to_write = (num > lybox->len) ? lybox->len : num;
        if(num > lybox->len) {
            pval_offset_idx = num - to_write;
            // 这个时候已经假定num > lybox->len，所以一定会覆盖
            // 例如容器长度5，想写入19个成员
            // tail_index会在之前的基础上前进19个位置
            // 因为是环形的，所以最后需要%len
            write_offset_idx = (write_offset_idx + num) % lybox->len;
        }
    }
    else {
        to_write = (num > (lybox->len - lybox->count)) ? (lybox->len - lybox->count) : num;
    }
    
    if(to_write <= 0) return 0;
    
    // 分两种情况拷贝（是否跨越缓冲区末尾）
    uint8_t first_space = lybox->len - write_offset_idx;
    if(to_write <= first_space) {
        memcpy(lybox->box + write_offset_idx * lybox->member_size, pval + pval_offset_idx * lybox->member_size, to_write * lybox->member_size);
    }
    else {
        memcpy(lybox->box + write_offset_idx * lybox->member_size, pval + pval_offset_idx * lybox->member_size, first_space * lybox->member_size);
        memcpy(lybox->box, pval + (pval_offset_idx + first_space) * lybox->member_size, (to_write - first_space) * lybox->member_size);
    }
    
    lybox->end_index = (uint8_t)(write_offset_idx + to_write) % lybox->len; // 写入位置后移
    
    if(lybox->count + to_write > lybox->len && w_mode == LyboxWriteMode_Fresh) {
        lybox->begin_index = lybox->end_index;    // 在写满的情况下，追到脑袋了，始端索引等于末端索引
        lybox->count = lybox->len;
    }
    else {
        lybox->count += to_write;
    }
    
    return to_write;
}

/// @brief 设置容器中的数据
/// @note 该函数是在 lybox_send 功能的基础上增加了可以指定索引位置存入，可以任意覆盖，灵活性更高
uint8_t lybox_set(LyBOX_t *lybox, void *pval, size_t num, uint8_t idx, LyboxWriteMode_t w_mode)
{
    uint8_t to_write = 0; 
    uint8_t write_offset_idx = (lybox->begin_index + idx) % lybox->len; // 实际写入位置
    uint8_t pval_offset_idx = 0; // 读取pval数据起始偏移量，只用于FRESH模式，来避免数据重复计算
    // 例如：容器长度5，想写入9个成员
    // 因为容器实际只能装下5个成员，9个成员中4个是多余的，需要跳过
    // 此时offset_idx = 9 - 5 = 4   [4]就是写入数据pval的起始索引偏移量
    
    // 实际写入数量
    if(w_mode == LyboxWriteMode_Fresh) {   // 因为采用避免重复计算的方式，所以只有FRESH模式在写入时的写入位置与尾索引不同
        to_write = (num > lybox->len) ? lybox->len : num;
        if(num > lybox->len) {
            pval_offset_idx = num - to_write;
            // 这个时候已经假定num > lybox->len，所以一定会覆盖
            // 例如容器长度5，想写入19个成员
            // tail_index会在之前的基础上前进19个位置
            // 因为是环形的，所以最后需要%len
            write_offset_idx = (write_offset_idx + num) % lybox->len;
        }
    }
    else {
        to_write = (num > (lybox->len - lybox->count)) ? (lybox->len - lybox->count) : num;
    }
    
    if(to_write <= 0 || idx >= lybox->len) return 0;
    
    // 分两种情况拷贝（是否跨越缓冲区末尾）
    uint8_t first_space = lybox->len - write_offset_idx;
    if(to_write <= first_space) {
        memcpy(lybox->box + write_offset_idx * lybox->member_size, pval + pval_offset_idx * lybox->member_size, to_write * lybox->member_size);
    }
    else {
        memcpy(lybox->box + write_offset_idx * lybox->member_size, pval + pval_offset_idx * lybox->member_size, first_space * lybox->member_size);
        memcpy(lybox->box, pval + (pval_offset_idx + first_space) * lybox->member_size, (to_write - first_space) * lybox->member_size);
    }
    
    write_offset_idx = (uint8_t)(write_offset_idx + to_write) % lybox->len; // 写入位置后移
    
    if(lybox->begin_index + idx > lybox->count){
        lybox->end_index = write_offset_idx;
    }

    if(lybox->count + to_write > lybox->len && w_mode == LyboxWriteMode_Fresh) {
        lybox->begin_index = lybox->end_index;    // 在写满的情况下，追到脑袋了，始端索引等于末端索引
        lybox->count = lybox->len;
    }
    else {
        lybox->count += to_write;
    }
    
    return to_write;
}

/// @brief 从容器中取出数据
uint8_t lybox_receive(LyBOX_t *lybox, void *pbuff, size_t num, enum LyboxReadMode r_mode)
{
    uint8_t to_read = lybox_peek(lybox, pbuff, num, r_mode);
    
    if(r_mode == LyboxReadMode_FIFO) {    // FIFO模式下，读取位置从始端索引开始
        lybox->begin_index = (uint8_t)(lybox->begin_index + to_read) % lybox->len;
    }
    else if(r_mode == LyboxReadMode_LIFOUnit || r_mode == LyboxReadMode_LIFOBlock) {  // LIFO模式下，读取位置末端索引开始
        lybox->end_index = (uint8_t)(lybox->end_index - to_read + lybox->len) % lybox->len;
    }
    
    return to_read;
}

/// @brief 预读容器中的数据
/// @note peek和receive本质的区别在于peek不会移动指针位置
uint8_t lybox_peek(LyBOX_t *lybox, void *pbuff, size_t num, enum LyboxReadMode r_mode)
{
    uint8_t to_read = (num > lybox->count) ? lybox->count : num;
    uint8_t read_offset_idx = 0;
    
    if(to_read <= 0 || num > lybox->count) return 0;
    
    if(r_mode == LyboxReadMode_FIFO) {
        uint8_t first_space = lybox->len - lybox->begin_index;   // 首页剩余可读取区域
        read_offset_idx = lybox->begin_index;
        if(to_read <= first_space) {
            memcpy(pbuff, lybox->box + read_offset_idx * lybox->member_size, to_read * lybox->member_size);
        }
        else {
            memcpy(pbuff, lybox->box + read_offset_idx * lybox->member_size, first_space * lybox->member_size);
            memcpy(pbuff + first_space * lybox->member_size, lybox->box, (to_read - first_space) * lybox->member_size);
        }
    }
    else if(r_mode == LyboxReadMode_LIFOUnit || r_mode == LyboxReadMode_LIFOBlock) {
        uint8_t first_space = lybox->end_index;   // 首页剩余可读取区域
        read_offset_idx = lybox->end_index - to_read;
        if(to_read <= first_space) {
            memcpy(pbuff, lybox->box + read_offset_idx * lybox->member_size, to_read * lybox->member_size);
        }
        else {
            memcpy(pbuff, lybox->box + (lybox->len - to_read + first_space) * lybox->member_size, (to_read - first_space) * lybox->member_size);
            memcpy(pbuff + (to_read - first_space) * lybox->member_size, lybox->box, first_space * lybox->member_size);
        }
    }
    
    // 使用双指针交换成员顺序
    if(r_mode == LyboxReadMode_LIFOUnit) {
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

/// @brief 覆盖容器中的数据
/// @note 覆盖项必须存在
bool lybox_cover(LyBOX_t *lybox, void *pval, uint8_t idx)
{
    if(idx >= lybox->count) return false;

    uint8_t write_offset_idx = (lybox->begin_index + idx) % lybox->len;
    memcpy(lybox->box + write_offset_idx * lybox->member_size, pval, lybox->member_size);
    return true;
}