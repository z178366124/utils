#include "timelib.h"


int rb_trigger_time(struct timer_task_t *a, struct timer_task_t* b){
    if(a->timestamp > b->timestamp){
        return 1;
    }else if(a->timestamp == b->timestamp){
        return 0;
    }else{
        return -1;
    }
}

RB_HEAD(rb_timer_t, timer_task_t) rb_timer_t;
// struct rb_timer_t treeroot;

RB_PROTOTYPE(rb_timer_t, timer_task_t, node, rb_trigger_time)
RB_GENERATE(rb_timer_t, timer_task_t, node, rb_trigger_time)

struct rb_timer_t* create_rb_timer(){
    struct rb_timer_t* p_timer = (struct rb_timer_t*)malloc(sizeof(struct rb_timer_t));
    return p_timer;
}