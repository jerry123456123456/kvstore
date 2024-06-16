#include<stdlib.h>
#include<stdio.h>

#define MEM_SIZE 40960

typedef struct mp_node_s {
    char *free_ptr;
    char *end;
    struct mp_node_s *next;
    int is_free; // 标记内存块是否可重用
} mp_node_t;

typedef struct mp_pool_s {
    struct mp_node_s *first;
    struct mp_node_s *current;
    int max;
} mp_pool_t;

int mp_init(mp_pool_t *m, int size) {
    if (!m) return -1;
    void *addr = malloc(size);
    mp_node_t *node = (mp_node_t*)addr;

    node->free_ptr = (char*)addr + sizeof(mp_node_t);
    node->end = (char*)addr + size;
    node->next = NULL;
    node->is_free = 1; // 初始时标记为可重用

    m->first = node;
    m->current = node;
    m->max = size;

    return 0;
}

void mp_dest(mp_pool_t *m) {
    if (!m) return;

    while (m->first) {
        void *addr = m->first;
        mp_node_t *node = (mp_node_t*)addr;
        m->first = node->next;
        free(addr);
    }
}

void *mp_alloc(mp_pool_t *m, int size) {
    void *addr = m->current;
    mp_node_t *node = (mp_node_t*)addr;

    do {
        if (size <= (node->end - node->free_ptr) && node->is_free) {

			char *ptr = node->free_ptr;
            node->free_ptr += size;
            node->is_free = 0; // 标记为不可重用
            return ptr;
        }
        node = node->next;
    } while (node);

    addr = malloc(m->max);

    node = (mp_node_t*)addr;

    node->free_ptr = (char*)addr + sizeof(mp_node_t);
    node->end = (char*)addr + m->max;
    node->next = m->current;
    node->is_free = 0; // 标记为不可重用

    m->current = node;

    char *ptr = node->free_ptr;
    node->free_ptr += size;
    return ptr;
}

void mp_free(mp_pool_t *m, void *ptr) {
    mp_node_t *node = m->first;
    while (node) {
        if (ptr >= (void*)node && ptr < (void*)node->end) {
            node->is_free = 1; // 标记为可重用
            break;
        }
        node = node->next;
    }
}

mp_pool_t m;