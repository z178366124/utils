#ifndef __LIST__H
#define __LIST__H
#include <stdlib.h>
#include <pthread.h>

typedef struct list_node_s{
    void* data;
    struct list_node_s* next;
}list_node_t;

// 链表结构体，包含头指针和锁
typedef struct {
    list_node_t* head;
    size_t link_size;
    pthread_mutex_t lock;
    int (*link_cmp)(void*, void*);
    void (*node_data_free_fun)(void*);
} list_t;

list_t* createList(int (*link_cmp)(void*, void*), void (*node_data_free_fun)(void*));
void destroyList(list_t* list);
void* insertlist_node(list_t* list, void* data);
void deletelist_node(list_t* list, void* data);

#endif