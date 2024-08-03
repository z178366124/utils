#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <yaml.h>

typedef enum {
    STATE_START,
    STATE_MAPPING,
    STATE_BLOCK,
    STATE_SEQ,
    STATE_PLUGIN,
    STATE_PLUGIN_READ_KEY,
    STATE_PLUGIN_READ_VALUE,
    STATE_PLUGIN_CONFIG,
    STATE_PLUGIN_CONFIG_FILE,
    STATE_PLUGIN_CONFIG_SQL,
    STATE_REGISTER_HOOKS,
    STATE_HOOKS,
    STATE_DONE
} State;

typedef enum{
    TYPE_KEY,
    TYPE_VALUE
} value_type_t;

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
} PluginData;

typedef struct stat_arr{
    char name[128];
    State state;
}stat_arr;

stat_arr stack[64];

void parse_yaml(const char *filename, PluginData *plugin_data) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        return;
    }

    yaml_parser_t parser;
    yaml_event_t event;

    if (!yaml_parser_initialize(&parser)) {
        fputs("Failed to initialize parser!\n", stderr);
        fclose(file);
        return;
    }

    yaml_parser_set_input_file(&parser, file);

    State state = STATE_START;
    value_type_t value_type = TYPE_KEY;
    int done = 0;
    char *current_key = NULL;

    while (!done) {
        if (!yaml_parser_parse(&parser, &event)) {
            fprintf(stderr, "Parser error %d\n", parser.error);
            break;
        }

        switch (event.type) {
            case YAML_STREAM_START_EVENT:
                break;
            case YAML_STREAM_END_EVENT:
                done = 1;
                break;
            case YAML_KEY_TOKEN: 
                value_type = TYPE_KEY;
                break;
            case YAML_VALUE_TOKEN: 
                value_type = TYPE_VALUE;
                break;
            case YAML_DOCUMENT_START_EVENT:
                break;
            case YAML_DOCUMENT_END_EVENT:
                state = STATE_DONE;
                break;
            case YAML_MAPPING_START_EVENT:
                if (state == STATE_START) {
                    state = STATE_MAPPING;
                }
                break;
            case YAML_MAPPING_END_EVENT:
                if (state == STATE_PLUGIN_CONFIG) {
                    state = STATE_PLUGIN;
                } else if (state == STATE_PLUGIN_CONFIG_FILE || state == STATE_PLUGIN_CONFIG_SQL) {
                    state = STATE_PLUGIN_CONFIG;
                } else if (state == STATE_REGISTER_HOOKS) {
                    state = STATE_PLUGIN;
                } else if (state == STATE_HOOKS) {
                    state = STATE_PLUGIN;
                } else if (state == STATE_PLUGIN) {
                    state = STATE_DONE;
                }
                break;
            case YAML_SEQUENCE_START_EVENT:
                if (state == STATE_PLUGIN_CONFIG && strcmp(current_key, "file") == 0) {
                    state = STATE_PLUGIN_CONFIG_FILE;
                } else if (state == STATE_PLUGIN_CONFIG && strcmp(current_key, "sql") == 0) {
                    state = STATE_PLUGIN_CONFIG_SQL;
                } else if (state == STATE_PLUGIN && strcmp(current_key, "register_hooks") == 0) {
                    state = STATE_REGISTER_HOOKS;
                } else if (state == STATE_PLUGIN && strcmp(current_key, "hooks") == 0) {
                    state = STATE_HOOKS;
                }
                break;
            case YAML_SEQUENCE_END_EVENT:
                if (state == STATE_PLUGIN_CONFIG_FILE || state == STATE_PLUGIN_CONFIG_SQL) {
                    state = STATE_PLUGIN_CONFIG;
                } else if (state == STATE_REGISTER_HOOKS) {
                    state = STATE_PLUGIN;
                } else if (state == STATE_HOOKS) {
                    state = STATE_PLUGIN;
                }
                break;
            case YAML_SCALAR_EVENT:
                if(value_type == TYPE_KEY){
                    current_key = strdup((char *)event.data.scalar.value);
                }else{

                }
                if (state == STATE_PLUGIN) {
                    current_key = strdup((char *)event.data.scalar.value);
                    printf("%s\n", current_key);
                    if (strcmp(current_key, "open") == 0 || strcmp(current_key, "name") == 0 || strcmp(current_key, "description") == 0) {
                        state = STATE_PLUGIN_CONFIG;
                    }else if (strcmp(current_key, "plugin") == 0)
                    {
                        state = STATE_PLUGIN;
                    }
                    
                } else if (state == STATE_PLUGIN_CONFIG) {
                    // printf("%s\n", current_key);
                    if (strcmp(current_key, "open") == 0) {
                        plugin_data->open = (strcmp((char *)event.data.scalar.value, "true") == 0);
                    } else if (strcmp(current_key, "name") == 0) {
                        plugin_data->name = strdup((char *)event.data.scalar.value);
                        printf("===%s  %s\n",plugin_data->name, current_key);
                    } else if (strcmp(current_key, "description") == 0) {
                        plugin_data->description = strdup((char *)event.data.scalar.value);
                    } else {
                        current_key = strdup((char *)event.data.scalar.value);
                    }
                } else if (state == STATE_PLUGIN_CONFIG_FILE) {
                    plugin_data->file[plugin_data->file_count++] = strdup((char *)event.data.scalar.value);
                } else if (state == STATE_PLUGIN_CONFIG_SQL) {
                    if (plugin_data->sql_count == 0 || plugin_data->sql[plugin_data->sql_count - 1].database != NULL) {
                        plugin_data->sql[plugin_data->sql_count].database = strdup((char *)event.data.scalar.value);
                    } else if (plugin_data->sql[plugin_data->sql_count - 1].sql_cmd == NULL) {
                        plugin_data->sql[plugin_data->sql_count].sql_cmd = strdup((char *)event.data.scalar.value);
                    } else {
                        plugin_data->sql[plugin_data->sql_count++].check_fun = strdup((char *)event.data.scalar.value);
                    }
                } else if (state == STATE_REGISTER_HOOKS) {
                    if (plugin_data->register_hooks_count == 0 || plugin_data->register_hooks[plugin_data->register_hooks_count - 1].plugin_name != NULL) {
                        plugin_data->register_hooks[plugin_data->register_hooks_count].plugin_name = strdup((char *)event.data.scalar.value);
                    } else if (plugin_data->register_hooks[plugin_data->register_hooks_count - 1].hook_name == NULL) {
                        plugin_data->register_hooks[plugin_data->register_hooks_count].hook_name = strdup((char *)event.data.scalar.value);
                    } else if (plugin_data->register_hooks[plugin_data->register_hooks_count - 1].func_name == NULL) {
                        plugin_data->register_hooks[plugin_data->register_hooks_count].func_name = strdup((char *)event.data.scalar.value);
                    } else {
                        plugin_data->register_hooks[plugin_data->register_hooks_count++].func_description = strdup((char *)event.data.scalar.value);
                    }
                } else if (state == STATE_HOOKS) {
                    if (plugin_data->hooks_count == 0 || plugin_data->hooks[plugin_data->hooks_count - 1].hook_name != NULL) {
                        plugin_data->hooks[plugin_data->hooks_count].hook_name = strdup((char *)event.data.scalar.value);
                    } else if (plugin_data->hooks[plugin_data->hooks_count - 1].hook_description == NULL) {
                        plugin_data->hooks[plugin_data->hooks_count].hook_description = strdup((char *)event.data.scalar.value);
                    } else {
                        plugin_data->hooks[plugin_data->hooks_count++].plugin_use = (strcmp((char *)event.data.scalar.value, "true") == 0);
                    }
                }
                break;
            default:
                break;
        }

        yaml_event_delete(&event);
    }

    yaml_parser_delete(&parser);
    fclose(file);
}



void parse_yaml_2(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        return;
    }

    yaml_parser_t parser;
    yaml_token_t token;

    if (!yaml_parser_initialize(&parser)) {
        fputs("Failed to initialize parser!\n", stderr);
        fclose(file);
        return;
    }

    yaml_parser_set_input_file(&parser, file);
    int stack_len = 0;
    int iskey = 0;
    State state = STATE_START;
    do {
        yaml_parser_scan(&parser, &token);
        switch (token.type) {
            case YAML_STREAM_START_TOKEN:
                // puts("yaml stream start...");
                break;
            case YAML_STREAM_END_TOKEN:
                // puts("yaml stream end...");
                break;
            case YAML_KEY_TOKEN: 
                printf("(Key token)  "); 
                iskey = 1;
                break;
            case YAML_VALUE_TOKEN: 
                printf("(Value token)  "); 
                break;
            case YAML_BLOCK_SEQUENCE_START_TOKEN: 
                puts("<b>Start Block (Sequence)</b>"); 
                break;
            case YAML_BLOCK_ENTRY_TOKEN: 
                puts("<b>Start Block (Entry)</b>"); 
                break;
            case YAML_BLOCK_END_TOKEN: puts("<b>End block</b>"); 
                break;
            case YAML_BLOCK_MAPPING_START_TOKEN: 
                puts("<b>Start Block (Mapping)</b>");
                break;
            case YAML_SCALAR_TOKEN: 
                printf("scalar %s \n", token.data.scalar.value); 
                if(iskey && state == STATE_START && strncmp(token.data.scalar.value, "plugin", sizeof("plugin")) == 0){
                    stack[stack_len].state = STATE_START;
                    strcpy(&stack[stack_len].name, "plugin");
                    iskey = 0;
                    state = STATE_PLUGIN;
                    break;
                }
                if()
                break;
            default: 
                printf("Got token of type %d\n", token.type);
        }
        if(token.type != YAML_STREAM_END_TOKEN)
            yaml_token_delete(&token);
    } while (token.type != YAML_STREAM_END_TOKEN);

    yaml_token_delete(&token);
    yaml_parser_delete(&parser);
    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    parse_yaml_2(argv[1]);

    PluginData plugin_data = {0};
    // parse_yaml(argv[1], &plugin_data);

    // // 输出解析结果
    // printf("Plugin:\n");
    // printf("  Name: %s\n", plugin_data.name);
    // printf("  Description: %s\n", plugin_data.description);
    // printf("  Open: %d\n", plugin_data.open);

    // printf("Files:\n");
    // for (int i = 0; i < plugin_data.file_count; i++) {
    //     printf("  %s\n", plugin_data.file[i]);
    // }

    // printf("SQL:\n");
    // for (int i = 0; i < plugin_data.sql_count; i++) {
    //     printf("  Database: %s, SQL Command: %s, Check Function: %s\n", plugin_data.sql[i].database, plugin_data.sql[i].sql_cmd, plugin_data.sql[i].check_fun);
    // }

    // printf("Register Hooks:\n");
    // for (int i = 0; i < plugin_data.register_hooks_count; i++) {
    //     printf("  Plugin Name: %s, Hook Name: %s, Function Name: %s, Function Description: %s, Plugin Use: %d\n", 
    //         plugin_data.register_hooks[i].plugin_name, 
    //         plugin_data.register_hooks[i].hook_name, 
    //         plugin_data.register_hooks[i].func_name, 
    //         plugin_data.register_hooks[i].func_description,
    //         plugin_data.register_hooks[i].plugin_use);
    // }

    // printf("Hooks:\n");
    // for (int i = 0; i < plugin_data.hooks_count; i++) {
    //     printf("  Hook Name: %s, Hook Description: %s, Plugin Use: %d\n", plugin_data.hooks[i].hook_name, plugin_data.hooks[i].hook_description, plugin_data.hooks[i].plugin_use);
    // }

    return 0;
}
