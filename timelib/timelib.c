#include "timelib.h"


int rb_trigger_time(struct timer_task_s *a, struct timer_task_s* b){
    if(a->timestamp > b->timestamp){
        return 1;
    }else if(a->timestamp == b->timestamp){
        int ret = strncmp(a->task_name, b->task_name, sizeof(a->task_name));
        if( ret > 0){
            return 1;
        }else if(ret == 0){
            return 0;
        }else{
            return -1;
        }
    }else{
        return -1;
    }
}

int rb_trigger_name(struct timer_task_s *a, struct timer_task_s* b){
    int ret = strncmp(a->task_name, b->task_name, sizeof(a->task_name));
    if( ret > 0){
        return 1;
    }else if(ret == 0){
        return 0;
    }else{
        return -1;
    }
}

RB_PROTOTYPE(rb_timer_s, timer_task_s, node, rb_trigger_time)
RB_GENERATE(rb_timer_s, timer_task_s, node, rb_trigger_time)

RB_PROTOTYPE(rb_name_s, timer_task_s, node, rb_trigger_name)
RB_GENERATE(rb_name_s, timer_task_s, node, rb_trigger_name)

mytimer_t* create_rb_timer(){
    mytimer_t* p_timer = (mytimer_t*)malloc(sizeof(mytimer_t));
    if(p_timer == NULL){
        return NULL;
    }
    p_timer->timer_fd = timerfd_create(CLOCK_MONOTONIC, 0);
    if (p_timer->timer_fd == -1) {
        perror("timerfd_create");
        goto ERR;
    }
    p_timer->trigger_time = -1;

    return p_timer;
ERR:
    if(p_timer){
        free(p_timer);
    }
}

timer_task_t* timer_insert_task(mytimer_t *timer, timer_task_t* task){
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
        perror("clock_gettime");
        return NULL;
    }
    int64_t timestamp_ns = (int64_t)ts.tv_sec * 1000000000LL + ts.tv_nsec;

    timer_task_t *ret_name = RB_INSERT(rb_name_s, &timer->rb_name, task);
    if(ret_name){
        return ret_name;
    }
    timer_task_t *ret_task = RB_INSERT(rb_timer_s, &timer->rb_timer, task);
    
    if(timer->trigger_time < 0 || task->timestamp < timer->trigger_time){
        struct itimerspec new_value;
        memset(&new_value, 0, sizeof(struct itimerspec));
        switch (task->precision)
        {
        case TIMER_PRECISION_SECONDS:
            new_value.it_value.tv_sec = task->interval;
            task->timestamp = timestamp_ns + task->interval * 1000000000LL;
            break;
        case TIMER_PRECISION_MILLISECONDS:
            new_value.it_value.tv_nsec = task->interval * 1000LL;
            task->timestamp = timestamp_ns + task->interval * 1000LL;
            break;
        case TIMER_PRECISION_NANOSECONDS:
            new_value.it_value.tv_nsec = task->interval;
            task->timestamp = timestamp_ns + task->interval;
            break;
        default:
            return task;
        }
        if (timerfd_settime(timer->timer_fd, 0, &new_value, NULL) == -1) {
            perror("timerfd_settime");
            return task;
        }
    }
    
    // printf("0x%x\n", ret_name);
    return ret_name;
}

timer_task_t* create_timer_task(const char* task_name, const char *description, timer_precision_t precision, timer_mode_t mode, int interval,
                        time_task_funnc_t task_func){
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
        perror("clock_gettime");
        return NULL;
    }
    int64_t timestamp_ns = (int64_t)ts.tv_sec * 1000000000LL + ts.tv_nsec;

    timer_task_t* task = (timer_task_t*)malloc(sizeof(timer_task_t));
    if(task == NULL) return NULL;

    strncpy(task->task_name, task_name, sizeof(task->task_name));
    strncpy(task->description, description, sizeof(task->description));
    // TODO 未完待续
    task->precision = precision; // 时间精度
    task->mode = mode;           // 定时模式
    task->interval = interval;             // 时间间隔
    switch (precision)
    {
    case TIMER_PRECISION_SECONDS:
        /* code */
        task->timestamp = timestamp_ns + interval * 1000000000LL;
        break;
    case TIMER_PRECISION_MILLISECONDS:
        /* code */
        task->timestamp = timestamp_ns + interval * 1000LL;
        break;
    case TIMER_PRECISION_NANOSECONDS:
        task->timestamp = timestamp_ns + interval;
        break;
    default:
        free(task);
        return NULL;
    }
    task->task_func = task_func;    // 任务函数指针
    return task;
}

timer_task_t* timer_remove_elm(mytimer_t *timer, timer_task_t* task){
    timer_task_t* p_task= RB_REMOVE(rb_name_s, &timer->rb_name, task);
    p_task= RB_REMOVE(rb_timer_s, &timer->rb_timer, task);
    return p_task;
}

void timer_runs(mytimer_t *timer){
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
        perror("clock_gettime");
        return ;
    }
    int64_t timestamp_ns = (int64_t)ts.tv_sec * 1000000000LL + ts.tv_nsec;
    timer_task_t *p_ret_task = RB_MIN(rb_timer_s, &timer->rb_timer);
    printf("min:%s\n", p_ret_task->task_name);
    while(p_ret_task && p_ret_task->timestamp <= timestamp_ns){
        timer_task_t* p_task = timer_remove_elm(timer, p_ret_task);
        if(p_task){
            p_task->task_func(p_task->argv);
            if(p_task->mode == TIMER_MODE_REPEATING){
                timer_insert_task(timer, p_task);
            }else if(p_ret_task->mode == TIMER_MODE_SINGLE){
                free(p_ret_task);
            }
        }
        p_ret_task = RB_MIN(rb_timer_s, &timer->rb_timer);        
    }
}

void timer_print(mytimer_t *timer){
    struct timer_task_s *task;
    RB_FOREACH(task, rb_name_s, &timer->rb_name){
        printf("%s\t", task->task_name);
    }
    printf("\n");
}