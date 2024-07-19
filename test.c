#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/timerfd.h>
#include <stdint.h>
#include <time.h>

#include "rbtree.h"

typedef struct timer_event_s{
    time_t trigger_time;
}timer_event_t;

struct tree_type_t{
    RB_ENTRY(tree_type_t) node;
    int time_a;
    int value;
};



int rb_trigger_time2(struct tree_type_t *a, struct tree_type_t* b){
    if(a->time_a > b->time_a){
        return 1;
    }else if(a->time_a == b->time_a){
        return 0;
    }else{
        return -1;
    }
}

// struct date_tree_t{
//     struct tree_type_t *rbh_root;
// };

RB_HEAD(date_tree_t, tree_type_t) date_tree_t;
struct date_tree_t treeroot;

RB_PROTOTYPE(date_tree_t, tree_type_t, node, rb_trigger_time2)
RB_GENERATE(date_tree_t, tree_type_t, node, rb_trigger_time2)
// RB_GENERATE(session_tree, session_t, node, rbsession_comp)


int main(){
    struct tree_type_t data1 = { .time_a = 10, .value = 20 };
    struct tree_type_t data2 = { .time_a = 20, .value = 10 };
    struct tree_type_t data3 = { .time_a = 5, .value = 25 };

    struct tree_type_t *ret = RB_INSERT(date_tree_t, &treeroot, &data1);
    printf("%x\n", ret);
    RB_INSERT(date_tree_t, &treeroot, &data2);
    RB_INSERT(date_tree_t, &treeroot, &data3);
    struct tree_type_t data = { .time_a = 20, .value = 0};
    struct tree_type_t * t = RB_FIND(date_tree_t, &treeroot, &data);
    
    printf("%d\n", t->value);

    return 0;
}