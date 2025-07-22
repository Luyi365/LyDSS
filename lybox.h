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

struct lybox {
    enum lyboxType type;	///< 容器类型
    uint8_t len;    ///< 容器长度
    size_t size;   	    ///< 容器大小（单位：字节）
    size_t count;    ///< 容器中成员实际数量

    uint8_t member_size;    ///< 单个成员的大小（单位：字节）
    // 注意：下面两个索引存在无效值UINT8_MAX
    uint8_t write_index;    ///< 写索引（指向下一个可写入的位置索引）
    uint8_t read_index;     ///< 读索引（指向下一个可读取的位置索引）


// 对于环形缓冲来说，追赶的方式不区分流和块，因为这两种方式最后得到的结果是一样的
// 但追赶的方式固定为FIFO，很多时候也需要FILO，因此需要引进栈的方式（但这个栈有点像是相向而行的环形缓冲），但同时也需要实现块的方式
    enum lyboxReadMode {
        lyboxReadMode_FIFO,
        lyboxReadMode_FILO_STREAM,
        lyboxReadMode_FILO_CHUNK,
    } read_mode;
    bool keep_fresh;    ///< 是否覆盖写入

    void *box;     ///< 容器
};

lybox_t *lybox_create(enum lyboxType type, uint8_t box_len, uint8_t mbr_size);
void lybox_delete(lybox_t *lybox);
bool lybox_send(lybox_t *lybox, void *pval, size_t num);
bool lybox_receive(lybox_t *lybox, void *pbuff, size_t num);

#endif /* __LYBOX_H__ */