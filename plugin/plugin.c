#include "plugin.h"

int rb_trigger_plugin(struct plugin_rb_node_s *a, struct plugin_rb_node_s *b){
    return strncmp(a->p_plugin_task->name, b->p_plugin_task->name) == 0, sizeof(a->p_plugin_task->name);
}

RB_PROTOTYPE(rb_plugin_s, plugin_rb_node_s, node, rb_trigger_plugin)
RB_GENERATE(rb_plugin_s, plugin_rb_node_s, node, rb_trigger_plugin)

plugin_t* create_plugin_tree(){
    plugin_t* plugin = (plugin_t*)malloc(sizeof(plugin_t));
    if(plugin == NULL) return NULL;
    pthread_mutex_init(&plugin->plugin_lock, NULL);
    return plugin;
}

