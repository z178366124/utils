#include "plugin.h"

int rb_trigger_plugin(struct plugin_rb_node_s *a, struct plugin_rb_node_s *b){
    if(strncmp(a->p_plugin_task->name, b->p_plugin_task->name) == 0, sizeof(a->p_plugin_task->name)){
        return 0;
    }else{
        return 1;
    }
}

RB_PROTOTYPE(rb_plugin_s, plugin_rb_node_s, node, rb_trigger_plugin)
RB_GENERATE(rb_plugin_s, plugin_rb_node_s, node, rb_trigger_plugin)

