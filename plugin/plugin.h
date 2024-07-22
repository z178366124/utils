#ifndef __PLUGIN__H
#define __PLUGIN__H

#include <pthread.h>
#include "list.h"
#include "rbtree.h"

typedef void * (*plugin_funnc_t)(void*);

typedef struct plugin_task_s{
    char name[64];  // 插件名 唯一
    char description[256];      // 插件描述

    int plugin_run_switch;  // 插件开关
    int plugin_reset_switch;    // 插件重置开关

    plugin_funnc_t init_fun;    // 插件初始化函数
    plugin_funnc_t run_fun;     // 插件执行函数
    plugin_funnc_t destory_fun; // 插件 销毁函数
    plugin_funnc_t reset_fun;   // 插件重启函数

    list_t *plugin_hook; // 插件钩子 链表
    pthread_mutex_t plugin_task_lock;
}plugin_task_t;

typedef struct plugin_rb_node_s{
    RB_ENTRY(plugin_rb_node_s) node;
    plugin_task_t* p_plugin_task;
}plugin_rb_node_t;

RB_HEAD(rb_plugin_s, plugin_rb_node_s) rb_plugin_s;
typedef struct rb_plugin_s rb_plugin_t;

typedef struct plugin_s{
    rb_plugin_t rb_plugin;
    pthread_mutex_t plugin_lock;
}plugin_t;



#endif