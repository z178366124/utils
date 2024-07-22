#include "list.h"

// 创建一个新的链表
list_t* createList(int (*link_cmp)(void*, void*), void (*node_data_free_fun)(void*)) {
    list_t* list = (list_t*)malloc(sizeof(list_t));
    if(list == NULL) return NULL;
    list->head = NULL;
    list->link_size = 0;
    list->link_cmp = link_cmp;
    list->node_data_free_fun = node_data_free_fun;
    pthread_mutex_init(&list->lock, NULL);
    return list;
}

// 销毁链表
void destroyList(list_t* list) {
    pthread_mutex_lock(&list->lock);
    list_node_t* current = list->head;
    list_node_t* nextlist_node;

    while (current != NULL) {
        nextlist_node = current->next;
        list->node_data_free_fun(current->data);
        free(current);
        current = nextlist_node;
    }

    pthread_mutex_unlock(&list->lock);
    pthread_mutex_destroy(&list->lock);
    free(list);
}

// 插入一个新节点到链表头部
void* insertlist_node(list_t* list, void* data) {
    pthread_mutex_lock(&list->lock);
    
    list_node_t* newlist_node = (list_node_t*)malloc(sizeof(list_node_t));
    if(newlist_node == NULL){
        goto ERR;
    }
    newlist_node->data = data;
    newlist_node->next = list->head;
    list->head = newlist_node;
    list->link_size++;
ERR:
    pthread_mutex_unlock(&list->lock);
    return newlist_node;
}

// 删除链表中的一个节点
void deletelist_node(list_t* list, void* data) {
    pthread_mutex_lock(&list->lock);
    
    list_node_t* current = list->head;
    list_node_t* prev = NULL;
    
    // while (current != NULL && current->data != data) {
    while(current != NULL && list->link_cmp(current->data, data) != 0){
        prev = current;
        current = current->next;
    }
    
    if (current != NULL) {
        if (prev != NULL) {
            prev->next = current->next;
        } else {
            list->head = current->next;
        }
        list->link_size--;
        list->node_data_free_fun(current->data);
        free(current);
    }
    
    pthread_mutex_unlock(&list->lock);
}
