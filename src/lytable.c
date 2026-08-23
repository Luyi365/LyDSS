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
#include <stdlib.h>
#include <string.h>
#include "../inc/lydds_api.h"

#define INITIAL_CAPACITY 16
#define LOAD_FACTOR 0.75

typedef struct lytable_node {
    char *key;
    void *value;
    struct lytable_node *next;
} lytable_node_t;

/* 默认哈希（FNV-1a），用户不传时用这个 */
static size_t default_hash(const char *key) 
{
    size_t h = 2166136261u;
    while (*key) {
        h ^= (unsigned char)*key++;
        h *= 16777619u;
    }
    return h;
}

// lytable *lytable_config(INITIAL_CAPACITY, LOAD_FACTOR, fn);

lytable *lytable_create(lytable_hash_fn fn) 
{
    lytable *t = malloc(sizeof(*t));
    if (!t) return NULL;
    t->capacity = INITIAL_CAPACITY;
    t->size = 0;
    t->hash = fn ? fn : default_hash;   /* 传 NULL 则用默认 */
    t->slots = calloc(t->capacity, sizeof(lytable_node_t *));
    if (!t->slots) {
        free(t);
        return NULL;
    }
    return t;
}

static int resize(lytable *t, size_t new_cap) 
{
    lytable_node_t **ns = calloc(new_cap, sizeof(lytable_node_t *));
    if (!ns) return -1;
    for (size_t i = 0; i < t->capacity; i++) {
        lytable_node_t *n = t->slots[i];
        while (n) {
            lytable_node_t *next = n->next;
            size_t idx = t->hash(n->key) % new_cap;
            n->next = ns[idx];
            ns[idx] = n;
            n = next;
        }
    }
    free(t->slots);
    t->slots = ns;
    t->capacity = new_cap;
    return 0;
}

int lytable_set(lytable *t, const char *key, void *value) 
{
    if (!t || !key) return -1;
    /* 扩容检查 */
    if ((double)t->size / t->capacity >= LOAD_FACTOR) {
        if (resize(t, t->capacity * 2) != 0) return -1;
    }
    size_t idx = t->hash(key) % t->capacity;
    lytable_node_t *n = t->slots[idx];
    /* 已存在则更新 */
    while (n) {
        if (strcmp(n->key, key) == 0) {
            n->value = value;
            return 0;
        }
        n = n->next;
    }
    /* 新节点，头插 */
    lytable_node_t *nn = malloc(sizeof(*nn));
    if (!nn) return -1;
    nn->key = malloc(strlen(key) + 1);
    if (!nn->key) {
        free(nn);
        return -1;
    }
    strcpy(nn->key, key);
    nn->value = value;
    nn->next = t->slots[idx];
    t->slots[idx] = nn;
    t->size++;
    return 0;
}

void *lytable_get(lytable *t, const char *key) 
{
    if (!t || !key) return NULL;
    size_t idx = t->hash(key) % t->capacity;
    lytable_node_t *n = t->slots[idx];
    while (n) {
        if (strcmp(n->key, key) == 0) return n->value;
        n = n->next;
    }
    return NULL;
}

void *lytable_remove(lytable *t, const char *key) 
{
    if (!t || !key) return NULL;
    size_t idx = t->hash(key) % t->capacity;
    lytable_node_t *n = t->slots[idx];
    lytable_node_t *prev = NULL;
    while (n) {
        if (strcmp(n->key, key) == 0) {
            if (prev) prev->next = n->next;
            else t->slots[idx] = n->next;
            void *v = n->value;
            free(n->key);
            free(n);
            t->size--;
            return v;
        }
        prev = n;
        n = n->next;
    }
    return NULL;
}

size_t lytable_size(lytable *t) 
{
    return t ? t->size : 0;
}

void lytable_destroy(lytable *t) 
{
    if (!t) return;
    for (size_t i = 0; i < t->capacity; i++) {
        lytable_node_t *n = t->slots[i];
        while (n) {
            lytable_node_t *next = n->next;
            free(n->key);
            free(n);
            n = next;
        }
    }
    free(t->slots);
    free(t);
}