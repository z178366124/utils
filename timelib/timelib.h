#ifndef __TIMELIB__H
#define __TIMELIB__H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/timerfd.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>

#include "rbtree.h"

// 定义时间精度的枚举类型
typedef enum {
    TIMER_PRECISION_SECONDS,    // 秒
    TIMER_PRECISION_MILLISECONDS,   // 毫秒
    TIMER_PRECISION_NANOSECONDS, // 纳秒
} timer_precision_t;

// 定义定时模式的枚举类型
typedef enum {
    TIMER_MODE_SINGLE,     // 单次定时
    TIMER_MODE_REPEATING   // 循环定时
} timer_mode_t;

typedef void * (*time_task_funnc_t)(void*);

typedef enum timer_state_s{
    TIMER_OK,
    TIMER_ERR,
}timer_state_t;

typedef struct timer_task_s{
    RB_ENTRY(timer_task_s) node;
    timer_precision_t precision; // 时间精度
    timer_mode_t mode;           // 定时模式
    int interval;             // 时间间隔
    long long timestamp;      // 时间戳，精确到纳秒
    time_task_funnc_t task_func;    // 任务函数指针
    void *argv;
    char task_name[64]; // 任务名
    char description[256];      // 任务描述
}timer_task_t;

RB_HEAD(rb_timer_s, timer_task_s) rb_timer_s;
typedef struct rb_timer_s rb_timer_t;


// typedef struct timer_name_s{
//     RB_ENTRY(timer_name_s) node;
//     timer_task_t *elm;
// }timer_name_t;

RB_HEAD(rb_name_s, timer_task_s) rb_name_s;
typedef struct rb_name_s rb_name_t;

typedef struct mytimer_s{
    rb_timer_t rb_timer;
    rb_name_t rb_name;
    time_t trigger_time;
    int timer_fd;
}mytimer_t;


mytimer_t* create_rb_timer();
timer_task_t* timer_insert_task(mytimer_t *timer, timer_task_t* task);
timer_task_t* create_timer_task(const char* task_name, const char *description, timer_precision_t precision, timer_mode_t mode, int interval,
                        time_task_funnc_t task_func);
void timer_print(mytimer_t *timer);
void timer_runs(mytimer_t *timer);
#endif
