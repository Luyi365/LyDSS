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
    lybox->size = box_len * mbr_size;
    lybox->member_size = mbr_size;
    lybox->member_count = 0;

    switch(type) {
    case lyboxType_GENERAL:
        lybox->queue_attribute.send_mode = 0;
        lybox->queue_attribute.send_direction = 0;     // 从前往后存数据
        lybox->queue_attribute.receive_mode = 0;
        lybox->queue_attribute.receive_direction = 0;  // 从前往后取数据
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

bool lybox_send(lybox_t *lybox, void *pval, size_t num)
{
    if((lybox->member_count + num) * lybox->member_size >= lybox->size) {
        return false;
    }

    void *p_dst = lybox->box;
    if(lybox->queue_attribute.send_direction == 0) { // 正序
        p_dst += lybox->size - (lybox->member_count + num) * lybox->member_size;
    }
    else if(lybox->queue_attribute.send_direction == 1) { // 倒序
        p_dst += lybox->member_count * lybox->member_size;
    }

    memcpy(p_dst, pval, num * lybox->member_size);
    lybox->member_count += num;

    return true;
}

bool lybox_receive(lybox_t *lybox, void *pbuff, size_t num)
{
    if(num > lybox->member_count) {
        return false;
    }

    void *p_src = lybox->box;
    if(lybox->queue_attribute.send_direction == 0) { // 正序
        if(lybox->queue_attribute.receive_direction == 0) { // 从前往后取数据
            p_src += lybox->size - (lybox->member_count * lybox->member_size);
            memcpy(pbuff, p_src, num * lybox->member_size);
            lybox->member_count -= num;
        }
        else if(lybox->queue_attribute.receive_direction == 1) { // 从后往前取数据
            p_src += lybox->size - num * lybox->member_size;
            memcpy(pbuff, p_src, num * lybox->member_size);
            lybox->member_count -= num;
            memmove(p_src + num * lybox->member_size, p_src, lybox->member_count * lybox->member_size);
        }
    }
    else if(lybox->queue_attribute.send_direction == 1) {    // 倒序
        if(lybox->queue_attribute.receive_direction == 0) { // 从前往后取数据
        }
        else if(lybox->queue_attribute.receive_direction == 1) { // 从后往前取数据
            p_src += (lybox->member_count - num) * lybox->member_size;
        }
    }



    // 对数据进行检查，如果排序是正序，则检查数据是否置顶，不置顶则位移置顶
    // 如果排序是逆序，则检查数据是否置底，不置底则位移置底

    return true;
}

