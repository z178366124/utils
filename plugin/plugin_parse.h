
#ifndef __PLUGIN_PARSE__H
#define __PLUGIN_PARSE__H
#include <stdio.h>
#include "plugin.h"
#include "yaml.h"

typedef enum {
    STATE_START,
    STATE_PLUGIN,
    STATE_PLUGIN_CONFIG,
    STATE_PLUGIN_CONFIG_FILE,
    STATE_PLUGIN_CONFIG_SQL,
    STATE_REGISTER_HOOKS,
    STATE_HOOKS,
    STATE_DONE
} State;

typedef struct {
    char *name;
    char *description;
    int open;
    char *file[10];
    int file_count;
    struct {
        char *database;
        char *sql_cmd;
        char *check_fun;
    } sql[10];
    int sql_count;
    struct {
        char *plugin_name;
        int plugin_use;
        char *hook_name;
        char *func_name;
        char *func_description;
    } register_hooks[10];
    int register_hooks_count;
    struct {
        char *hook_name;
        char *hook_description;
        int plugin_use;
    } hooks[10];
    int hooks_count;
} PluginaDta;

#endif