#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/timerfd.h>
#include <stdint.h>
#include <time.h>
#include <sys/epoll.h>

#include "timelib.h"
// #include "rbtree.h"

// typedef struct timer_event_s{
//     time_t trigger_time;
// }timer_event_t;

// struct tree_type_t{
//     RB_ENTRY(tree_type_t) node;
//     int time_a;
//     int value;
// };



// int rb_trigger_time(struct tree_type_t *a, struct tree_type_t* b){
//     if(a->time_a > b->time_a){
//         return 1;
//     }else if(a->time_a == b->time_a){
//         return 0;
//     }else{
//         return -1;
//     }
// }

// // struct date_tree_t{
// //     struct tree_type_t *rbh_root;
// // };

// RB_HEAD(date_tree_t, tree_type_t) date_tree_t;
// struct date_tree_t treeroot;

// RB_PROTOTYPE(date_tree_t, tree_type_t, node, rb_trigger_time)
// RB_GENERATE(date_tree_t, tree_type_t, node, rb_trigger_time)
// RB_GENERATE(session_tree, session_t, node, rbsession_comp)


void* func(void* args){
    printf("触发任务\n");
}

#define MAX_EVENTS 10

int main(){
    mytimer_t* timer = create_rb_timer();
    if(timer == NULL){
        printf("定时器错误\n");
        exit(1);
    }

    timer_task_t *p_task = create_timer_task("task1", "one task", TIMER_PRECISION_SECONDS, TIMER_MODE_REPEATING, 5, func);

    timer_task_t *p_ret = timer_insert_task(timer, p_task);
    timer_print(timer);


    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = timer->timer_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, timer->timer_fd, &ev) == -1) {
        perror("epoll_ctl: timer_fd");
        close(timer->timer_fd);
        close(epoll_fd);
        exit(EXIT_FAILURE);
    }

    // 事件循环
    struct epoll_event events[MAX_EVENTS];
    
    while (1) {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll_wait");
            close(epoll_fd);
            exit(EXIT_FAILURE);
        }

        for (int n = 0; n < nfds; ++n) {
            if (events[n].data.fd == timer->timer_fd) {
                uint64_t exp;
                ssize_t s = read(timer->timer_fd, &exp, sizeof(uint64_t));
                if (s < 0) {
                    perror("read");
                    close(timer->timer_fd);
                    close(epoll_fd);
                    exit(EXIT_FAILURE);
                }
                timer_runs(timer);
            }
        }
    }

    // 关闭文件描述符
    close(timer->timer_fd);
    close(epoll_fd);
    
    return 0;
}


