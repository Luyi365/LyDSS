/**
 * @file lytable.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief
 * @date 2026-08-23
 *
 * @copyright Copyright (c) 2026
 *
 * @note
 * Matters related to this document.
 */

#include "../inc/lydss_api.h"

#if LYDSS_TABLE_ENABLED

#include "../inc/lydss_priv.h"

#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 16     ///< 槽位初始数量
#define LOAD_FACTOR 0.75        ///< 触发扩容的负载因子

struct LyTableNode {
    char *key;                  ///< 键的副本
    void *value;                ///< 值指针
    struct LyTableNode *next;   ///< 同槽位的后继节点
};

struct LyTable {
    struct LyTableNode **slots; ///< 槽位数组
    size_t capacity;            ///< 槽位数量
    size_t size;                ///< 键值对数量
    LyTableHash_t hash;         ///< 用户提供的哈希函数
};

static size_t table_hash_default(const char *key);
static int table_resize_slots(LyTable_t *table, size_t new_cap);

/// @brief 创建表
LyTable_t *lytable_create(LyTableHash_t hash)
{
    LyTable_t *table = malloc(sizeof(LyTable_t));
    if(table == NULL) {
        return NULL;
    }

    table->capacity = INITIAL_CAPACITY;
    table->size = 0;
    table->hash = hash ? hash : table_hash_default;

    table->slots = calloc(table->capacity, sizeof(struct LyTableNode *));
    if(table->slots == NULL) {
        free(table);
        return NULL;
    }

    return table;
}

/// @brief 插入或更新键值对
/// @note 负载达到 LOAD_FACTOR 时先扩容，键以副本存储，值只保留指针
int lytable_set(LyTable_t *table, const char *key, void *value)
{
    if(table == NULL
        || key == NULL) {
        return -1;
    }

    if((double)table->size / table->capacity >= LOAD_FACTOR) {
        if(table_resize_slots(table, table->capacity * 2) != 0) {
            return -1;
        }
    }

    size_t idx = table->hash(key) % table->capacity;
    for(struct LyTableNode *node = table->slots[idx]; node != NULL; node = node->next) {
        if(strcmp(node->key, key) == 0) {
            node->value = value;
            return 0;
        }
    }

    struct LyTableNode *new_node = malloc(sizeof(struct LyTableNode));
    if(new_node == NULL) {
        return -1;
    }

    new_node->key = malloc(strlen(key) + 1);
    if(new_node->key == NULL) {
        free(new_node);
        return -1;
    }

    strcpy(new_node->key, key);
    new_node->value = value;
    LYSLIST_LINK_HEAD(table->slots[idx], new_node);
    table->size++;

    return 0;
}

/// @brief 查找键值对
void *lytable_get(LyTable_t *table, const char *key)
{
    if(table == NULL
        || key == NULL) {
        return NULL;
    }

    size_t idx = table->hash(key) % table->capacity;
    for(struct LyTableNode *node = table->slots[idx]; node != NULL; node = node->next) {
        if(strcmp(node->key, key) == 0) {
            return node->value;
        }
    }

    return NULL;
}

/// @brief 删除键值对
void *lytable_remove(LyTable_t *table, const char *key)
{
    if(table == NULL
        || key == NULL) {
        return NULL;
    }

    size_t idx = table->hash(key) % table->capacity;
    struct LyTableNode **pp_node = &table->slots[idx];
    while(*pp_node != NULL) {
        struct LyTableNode *node = *pp_node;
        if(strcmp(node->key, key) == 0) {
            LYSLIST_UNLINK(pp_node, node);

            void *value = node->value;
            free(node->key);
            free(node);
            table->size--;

            return value;
        }
        pp_node = &node->next;
    }

    return NULL;
}

/// @brief 获取键值对数量
size_t lytable_size(LyTable_t *table)
{
    return table != NULL ? table->size : 0;
}

/// @brief 释放表
/// @note 不释放 value 指向的内存，由调用者负责
void lytable_destroy(LyTable_t *table)
{
    if(table == NULL) {
        return;
    }

    for(size_t i = 0; i < table->capacity; i++) {
        struct LyTableNode *node = table->slots[i];
        while(node != NULL) {
            struct LyTableNode *next = node->next;
            free(node->key);
            free(node);
            node = next;
        }
    }

    free(table->slots);
    free(table);
}

/* --------------------------------- static --------------------------------- */
/// @brief 默认哈希函数（FNV-1a）
static size_t table_hash_default(const char *key)
{
    size_t hash = 2166136261u;

    while(*key) {
        hash ^= (unsigned char)*key++;
        hash *= 16777619u;
    }

    return hash;
}

/// @brief 重建槽位数组并迁移全部节点
static int table_resize_slots(LyTable_t *table, size_t new_cap)
{
    struct LyTableNode **new_slots = calloc(new_cap, sizeof(struct LyTableNode *));
    if(new_slots == NULL) {
        return -1;
    }

    for(size_t i = 0; i < table->capacity; i++) {
        struct LyTableNode *node = table->slots[i];
        while(node != NULL) {
            struct LyTableNode *next = node->next;
            size_t idx = table->hash(node->key) % new_cap;
            LYSLIST_LINK_HEAD(new_slots[idx], node);
            node = next;
        }
    }

    free(table->slots);
    table->slots = new_slots;
    table->capacity = new_cap;

    return 0;
}

#endif /* LYDSS_TABLE_ENABLED */
