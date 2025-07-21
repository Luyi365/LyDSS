#ifndef __LYBOX_H__
#define __LYBOX_H__

#include <stdint.h> 
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

/* LyBOX */
typedef struct lybox lybox_t;
// typedef bool (lybox_single)(lybox_t *, void *);
// typedef bool (lybox_multi)(lybox_t *, void *, size_t);
typedef int(__cdecl func_sort_t)(const void *, const void *);

enum lyboxType {
    lyboxType_GENERAL = 0,

    lyboxType_CUSTOM,    ///< 自定义类型
    lyboxType_NUMBER,
};

// 追赶的方式
struct lybox {
    enum lyboxType type;	///< 容器类型
    uint8_t len;    ///< 容器长度
    size_t size;   	    ///< 容器大小（单位：字节）
    size_t count;    ///< 容器中成员实际数量

    uint8_t member_size;    ///< 单个成员的大小（单位：字节）
    uint8_t head;    ///< 写索引（指向下一个可写入的位置索引）
    uint8_t tail;    ///< 读索引（指向下一个可读取的位置索引）

    struct box_attribute {
        func_sort_t *sort_function;          ///< 排序函数
        uint8_t send_mode;          ///< 0为流方式（单成员），1为块方式
        uint8_t receive_mode;       ///< 0为流方式（单成员），1为块方式
        uint8_t receive_direction;  ///< 0为从前往后取数据，1为从后往前取数据
    } box_attribute;           ///< 队列化属性(决定了数据的流动方式)

    void *box;     ///< 容器
};

lybox_t *lybox_create(enum lyboxType type, uint8_t box_len, uint8_t mbr_size);
void lybox_delete(lybox_t *lybox);
bool lybox_send(lybox_t *lybox, void *pval, size_t num);
bool lybox_receive(lybox_t *lybox, void *pbuff, size_t num);
void lybox_set_box_attribute(lybox_t *lybox, struct box_attribute attr);

#endif /* __LYBOX_H__ */