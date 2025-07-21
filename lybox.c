#include <stdbool.h>
#include <string.h>
#include <malloc.h>
#include <stdint.h> 
#include "./lybox.h"

// box_len:可存储的最大项目数
lybox_t *lybox_create(enum lyboxType type, uint8_t box_len, uint8_t mbr_size)
{
    lybox_t *lybox = malloc(sizeof(lybox_t));

    lybox->type = type;
    lybox->len = box_len;
    lybox->size = box_len * mbr_size;
    lybox->count = 0;
    lybox->member_size = mbr_size;
    lybox->head = 0;
    lybox->tail = 0;

    switch(type) {
    case lyboxType_GENERAL:
        lybox->box_attribute.send_mode = 0;
        lybox->box_attribute.sort_function = 0;     // 从前往后存数据
        lybox->box_attribute.receive_mode = 0;
        lybox->box_attribute.receive_direction = 0;  // 从前往后取数据
        break;
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

// 检查剩余可写入的空间
uint8_t lybox_get_free_space(lybox_t *lybox)
{
    return lybox->len - lybox->count;
}

// 检查已存储的数据量
uint8_t lybox_get_used_space(lybox_t *lybox)
{
    return lybox->count;
}

bool lybox_send(lybox_t *lybox, void *pval, size_t num)
{
    // if((lybox->count + num) * lybox->member_size >= lybox->size) {
    //     return false;
    // }

    // void *p_src = pval;
    // void *p_dst = lybox->box;
    // uint8_t buff[num];
    // if(lybox->box_attribute.send_mode == 0) {
    //     if(lybox->box_attribute.sort_function == 0) { // 正序
    //         p_dst += lybox->count * lybox->member_size;
    //     }
    //     else if(lybox->box_attribute.sort_function == 1) { // 倒序
    //         memcpy(buff, pval, num * lybox->member_size);
    //         qsort(buff, num, lybox->member_size, lybox->box_attribute.sort_function);
    //         p_src = buff;
    //         p_dst += lybox->size - (lybox->count + num) * lybox->member_size;
    //     }
    // }
    // else if(lybox->box_attribute.send_mode == 1) {
    //     if(lybox->box_attribute.sort_function == 0) { // 正序
    //         p_dst += lybox->count * lybox->member_size;
    //     }
    //     else if(lybox->box_attribute.sort_function == 1) { // 倒序
    //         p_dst += lybox->size - (lybox->count + num) * lybox->member_size;
    //     }
    // }

    // memcpy(p_dst, p_src, num * lybox->member_size);
    // lybox->count += num;

    uint8_t write_num = (num > (lybox->len - lybox->count)) ? (lybox->len - lybox->count) : num;
    
    if (write_num <= 0) return 0;

    // 分两种情况拷贝（是否跨越缓冲区末尾）
    uint8_t first_num = lybox->len - lybox->head;
    if (write_num <= first_num) {
        memcpy(lybox->box + lybox->head * lybox->member_size, pval, write_num * lybox->member_size);
    } 
    else {
        memcpy(lybox->box + lybox->head * lybox->member_size, pval, first_num * lybox->member_size);
        memcpy(lybox->box, pval + first_num, (write_num - first_num) * lybox->member_size);
    }

    lybox->head = (lybox->head + write_num) % lybox->len;
    lybox->count += write_num;

    return true;
}

bool lybox_receive(lybox_t *lybox, void *pbuff, size_t num)
{
    // if(num > lybox->count) {
    //     return false;
    // }

    // void *p_src = lybox->box;
    // uint8_t buff[num];
    // if(lybox->box_attribute.receive_mode == 0) {
    //     if(lybox->box_attribute.sort_function == 0) { // 正序
    //         if(lybox->box_attribute.receive_direction == 0) { // 从前往后取数据
    //             memmove(lybox->box, lybox->box + num * lybox->member_size, (lybox->count - num) * lybox->member_size);
    //         }
    //         else if(lybox->box_attribute.receive_direction == 1) { // 从后往前取数据

    //         }
    //     }
    //     else if(lybox->box_attribute.sort_function == 1) {    // 倒序
    //         if(lybox->box_attribute.receive_direction == 0) { // 从前往后取数据

    //         }
    //         else if(lybox->box_attribute.receive_direction == 1) { // 从后往前取数据

    //         }
    //     }
    // }
    // else if(lybox->box_attribute.receive_mode == 1) {
    //     if(lybox->box_attribute.sort_function == 0) { // 正序
    //         if(lybox->box_attribute.receive_direction == 0) { // 从前往后取数据
    //             memmove(lybox->box, lybox->box + num * lybox->member_size, (lybox->count - num) * lybox->member_size);
    //         }
    //         else if(lybox->box_attribute.receive_direction == 1) { // 从后往前取数据
    //             p_src += (lybox->count - num) * lybox->member_size;
    //         }
    //     }
    //     else if(lybox->box_attribute.sort_function == 1) {    // 倒序
    //         if(lybox->box_attribute.receive_direction == 0) { // 从前往后取数据
    //             p_src += lybox->size - (lybox->count * lybox->member_size);
    //         }
    //         else if(lybox->box_attribute.receive_direction == 1) { // 从后往前取数据
    //             p_src += lybox->size - num * lybox->member_size;
    //         }
    //     }
    // }

    // memcpy(pbuff, p_src, num * lybox->member_size);
    // // 对数据进行检查，如果排序是正序，则检查数据是否置顶，不置顶则位移置顶
    // // 如果排序是逆序，则检查数据是否置底，不置底则位移置底
    // if(lybox->box_attribute.sort_function == 0 && lybox->box_attribute.receive_direction == 0) {
    //     memmove(lybox->box, lybox->box + num * lybox->member_size, (lybox->count - num) * lybox->member_size);
    // }
    // else if(lybox->box_attribute.sort_function == 1 && lybox->box_attribute.receive_direction == 1) {
    //     memmove(lybox->box + lybox->size - (lybox->count - num) * lybox->member_size, lybox->box + lybox->size - lybox->count * lybox->member_size, (lybox->count - num) * lybox->member_size);
    // }
    // lybox->count -= num;

    uint8_t read_num = (num > lybox->count) ? lybox->count : num;
    
    if (read_num <= 0) return 0;

    // 分两种情况拷贝（是否跨越缓冲区末尾）
    uint8_t first_num = lybox->len - lybox->tail; // 剩余可读空间
    if (read_num <= first_num) {
        memcpy(pbuff, lybox->box + lybox->tail * lybox->member_size, read_num * lybox->member_size);
    } 
    else {
        memcpy(pbuff, lybox->box + lybox->tail * lybox->member_size, first_num * lybox->member_size);
        memcpy(pbuff + first_num, lybox->box, (read_num - first_num) * lybox->member_size);
    }

    lybox->tail = (lybox->tail + read_num) % lybox->len;
    lybox->count -= read_num;

    return true;
}

void lybox_set_box_attribute(lybox_t *lybox, struct box_attribute attr)
{
    lybox->box_attribute.send_mode = attr.send_mode;
    lybox->box_attribute.sort_function = attr.sort_function;
    lybox->box_attribute.receive_mode = attr.receive_mode;
    lybox->box_attribute.receive_direction = attr.receive_direction;
    lybox->type = lyboxType_CUSTOM; // 设置为自定义类型
}

// 时间升序（即旧数据在前，新数据在后）
uint8_t sort_time_asc(const void *a, const void *b)
{
    // 数组里的数据本来就是按照时间顺序排列的，所以不需要做任何处理
    return 0;
}
// 时间降序（即新数据在前，旧数据在后）
uint8_t sort_time_desc(const void *a, const void *b)
{
    return (*(uint8_t *)b - *(uint8_t *)a);
}