#ifndef __TIMELIB__H
#define __TIMELIB__H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/timerfd.h>
#include <stdint.h>
#include <time.h>

#include "rbtree.h"

// 定义时间精度的枚举类型
typedef enum {
    TIMER_PRECISION_SECONDS,    // 秒
    TIMER_PRECISION_MILLISECONDS,   // 毫秒
} timer_precision_t;

// 定义定时模式的枚举类型
typedef enum {
    TIMER_MODE_SINGLE,     // 单次定时
    TIMER_MODE_REPEATING   // 循环定时
} timer_mode_t;

typedef void * (*time_task_funnc_t)(void*);

struct timer_task_t{
    RB_ENTRY(timer_task_t) node;
    timer_precision_t precision; // 时间精度
    timer_mode_t mode;           // 定时模式
    int interval;             // 时间间隔
    long long timestamp;      // 时间戳，精确到毫秒
    time_task_funnc_t task_func;    // 任务函数指针
    char task_name[64]; // 任务名
    char description[256];      // 任务描述
};

struct rb_timer_t* create_rb_timer();

#endif
