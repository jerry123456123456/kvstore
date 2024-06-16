#ifndef __KVSTORE_H__
#define __KVSTORE_H__

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#define ENABLE_MEM_POOL			1

#if ENABLE_MEM_POOL
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

int mp_init(mp_pool_t *m, int size);
void mp_dest(mp_pool_t *m);

void *mp_alloc(mp_pool_t *m,int size);
void mp_free(mp_pool_t *m, void *ptr);

extern mp_pool_t m;
#endif



#define BUFFER_LENGTH		512

typedef int (*RCALLBACK)(int fd);


struct conn_item {
	int fd;
	
	char rbuffer[BUFFER_LENGTH];
	int rlen;
	char wbuffer[BUFFER_LENGTH];
	int wlen;

	union {
		RCALLBACK accept_callback;
		RCALLBACK recv_callback;
	} recv_t;
	RCALLBACK send_callback;
};
// libevent --> 

int epoll_entry(void);
int ntyco_entry(void);


int kvstore_request(struct conn_item *item);

void *kvstore_malloc(int size);
void kvstore_free(void *ptr);

#define NETWORK_EPOLL		0
#define NETWORK_NTYCO		1
#define NETWORK_IOURING		2

#define ENABLE_NETWORK_SELECT	NETWORK_NTYCO

#define ENABLE_ARRAY_KVENGINE	1
#define ENABLE_RBTREE_KVENGINE		1
#define ENABLE_HASH_KVENGINE	1

#if ENABLE_HASH_KVENGINE

typedef struct hashtable_s hashtable_t;


extern hashtable_t Hash;

int kvstore_hash_create(hashtable_t *hash);
void kvstore_hash_destory(hashtable_t *hash);
int kvs_hash_set(hashtable_t *hash, char *key, char *value);
char *kvs_hash_get(hashtable_t *hash, char *key);
int kvs_hash_delete(hashtable_t *hash, char *key);
int kvs_hash_modify(hashtable_t *hash, char *key, char *value);
int kvs_hash_count(hashtable_t *hash);
#endif




#if ENABLE_ARRAY_KVENGINE

struct kvs_array_item {
	char *key;
	char *value;
};

#define KVS_ARRAY_SIZE		1024

typedef struct array_s {
	struct kvs_array_item *array_table;
	int array_idx;
} array_t;

extern array_t Array;


int kvstore_array_create(array_t *arr);
void kvstore_array_destory(array_t *arr); 
int kvs_array_set(array_t *arr, char *key, char *value);
char *kvs_array_get(array_t *arr, char *key);
int kvs_array_delete(array_t *arr, char *key);
int kvs_array_modify(array_t *arr, char *key, char *value);
int kvs_array_count(array_t *arr);


#endif


#if ENABLE_RBTREE_KVENGINE

typedef struct _rbtree rbtree_t;
extern rbtree_t Tree;

int kvstore_rbtree_create(rbtree_t *tree);
void kvstore_rbtree_destory(rbtree_t *tree);
int kvs_rbtree_set(rbtree_t *tree, char *key, char *value);
char* kvs_rbtree_get(rbtree_t *tree, char *key);
int kvs_rbtree_delete(rbtree_t *tree, char *key);
int kvs_rbtree_modify(rbtree_t *tree, char *key, char *value);
int kvs_rbtree_count(rbtree_t *tree);
#endif

#endif
