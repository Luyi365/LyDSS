#ifndef __LYBOX_H__
#define __LYBOX_H__

#include <stdint.h> 
#include <stddef.h>
#include <stdbool.h>

/* LyBOX */
typedef struct lybox lybox_t;
// typedef bool (lybox_single)(lybox_t *, void *);
// typedef bool (lybox_multi)(lybox_t *, void *, size_t);

enum lyboxType {
    lyboxType_GENERAL = 0,

    lyboxType_CUSTOM,    ///< 自定义类型
    lyboxType_NUMBER,
};

struct lybox {
    enum lyboxType type;	///< 容器类型
    size_t size;   	///< 容器大小（单位：字节）

    uint8_t member_size;    ///< 单个成员的大小（单位：字节）
    void *head;    ///< 指向首个成员的位置
    void *tail;    ///< 指向最后一个成员的位置
    size_t member_count;    ///< 容器中成员实际数量

    struct {
        uint8_t send_mode;          ///< 0为流方式（单成员），1为块方式
        uint8_t send_direction;     ///< 0为从前往后存数据（新数据在前排），1为从后往前存数据（新数据在后排）
        uint8_t receive_mode;       ///< 0为流方式（单成员），1为块方式
        uint8_t receive_direction;  ///< 0为从前往后取数据，1为从后往前取数据
    } queue_attribute;           ///< 队列化属性

    void *box;     ///< 容器
};

lybox_t *lybox_create(enum lyboxType type, uint8_t box_len, uint8_t mbr_size);
void lybox_delete(lybox_t *lybox);
bool lybox_send(lybox_t *lybox, void *pval, size_t num);
bool lybox_receive(lybox_t *lybox, void *pbuff, size_t num);

#endif /* __LYBOX_H__ */