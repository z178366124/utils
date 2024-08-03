typedef struct {
    char *database;
    char *sql_cmd;
    char *check_fun;
} SqlConfig;

typedef struct {
    char *hook_name;
    char *hook_description;
    int is_use;
} Hook;

typedef struct {
    char *plugin_name;
    char *hook_name;
    int is_use;
    char *func_name;
    int priority;
    char *func_description;
} RegisterHook;

typedef struct {
    char *name;
    char *so_dir;
    char *so_name;
    int open;
    char *description;
    struct {
        char *files[10];
        int file_count;
        SqlConfig sql_configs[10];
        int sql_count;
    } config;
    Hook hooks[10];
    int hook_count;
    RegisterHook register_hooks[10];
    int register_hook_count;
    char *init_fun;
    char *run_func;
    char *destory_fun;
    char *reset_fun;
} PluginConfig;


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <yaml.h>

#define INITIAL_SIZE 10

void parse_plugin_config(const char *filename, PluginConfig *config);

void free_plugin_config(PluginConfig *config) {
    free(config->name);
    free(config->so_dir);
    free(config->so_name);
    free(config->description);
    for (int i = 0; i < config->config.file_count; ++i) {
        free(config->config.files[i]);
    }
    // free(config->config.files);
    for (int i = 0; i < config->config.sql_count; ++i) {
        free(config->config.sql_configs[i].database);
        free(config->config.sql_configs[i].sql_cmd);
        free(config->config.sql_configs[i].check_fun);
    }
    // free(config->config.sql_configs);
    for (int i = 0; i < config->hook_count; ++i) {
        free(config->hooks[i].hook_name);
        free(config->hooks[i].hook_description);
    }
    // free(config->hooks);
    for (int i = 0; i < config->register_hook_count; ++i) {
        free(config->register_hooks[i].plugin_name);
        free(config->register_hooks[i].hook_name);
        free(config->register_hooks[i].func_name);
        free(config->register_hooks[i].func_description);
    }
    // free(config->register_hooks);
    free(config->init_fun);
    free(config->run_func);
    free(config->destory_fun);
    free(config->reset_fun);
}

typedef enum{
    TYPE_KEY,
    TYPE_VALUE
} value_type_t;

void parse_mapping(yaml_parser_t *parser, PluginConfig *config, yaml_token_t *token, int *depth);

void parse_sequence(yaml_parser_t *parser, PluginConfig *config, yaml_token_t *token, int *depth);

void parse_scalar(yaml_parser_t *parser, PluginConfig *config, yaml_token_t *token, int *depth, char **stack_name, value_type_t type);

void parse_yaml(const char *filename, PluginConfig *config) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    yaml_parser_t parser;
    yaml_parser_initialize(&parser);
    yaml_parser_set_input_file(&parser, file);

    yaml_token_t token;
    int depth = 0;
    value_type_t value_type = TYPE_KEY;

    char *stack_name[64] = {0};
    int done = 0;
    do{
        // if (!yaml_parser_parse(&parser, &event)) {
        //     fprintf(stderr, "Error parsing YAML: %s\n", parser.problem);
        //     exit(EXIT_FAILURE);
        // }
        yaml_parser_scan(&parser, &token);

        switch (token.type) {
            case YAML_STREAM_START_TOKEN:
                depth++;
                break;
            case YAML_KEY_TOKEN: 
                value_type = TYPE_KEY;
                break;
            case YAML_VALUE_TOKEN: 
                value_type = TYPE_VALUE;
                break;
            case YAML_STREAM_END_TOKEN:
                depth--;
                done = 1;
                break;
            case YAML_DOCUMENT_START_TOKEN:
                depth++;
                break;
            case YAML_DOCUMENT_END_TOKEN:
                depth--;
                break;
            case YAML_BLOCK_MAPPING_START_TOKEN:
                break;
            case YAML_BLOCK_SEQUENCE_START_TOKEN: 
                depth++;
                break;
            case YAML_BLOCK_ENTRY_TOKEN: 
                depth--;
                break;
            // case YAML_SEQUENCE_START_EVENT:
            //     parse_sequence(&parser, config, &event, &depth);
            //     break;
            // case YAML_SEQUENCE_END_EVENT:
            //     break;
            case YAML_SCALAR_TOKEN:
                parse_scalar(&parser, config, &token, &depth, stack_name, value_type);
                break;
            default:
                break;
        }
        yaml_token_delete(&token);
    }while(!done);

    yaml_parser_delete(&parser);
    fclose(file);
}

void parse_scalar(yaml_parser_t *parser, PluginConfig *config, yaml_token_t *token, int *depth, char **stack_name, value_type_t type) {
    // This function should handle parsing scalar values
    // and fill the appropriate fields in the config structure
    if(type == TYPE_KEY){
        if(stack_name[*depth]) free(stack_name[*depth]);
        stack_name[*depth] =  strdup((char *)token->data.scalar.value);
        printf("key : %s\n", stack_name[*depth]);
    }else{
        if(!stack_name[*depth]) return;
        printf("%s\n", (char *)token->data.scalar.value);
        if(strcmp(stack_name[*depth], "name") == 0){
            config->name = strdup((char *)token->data.scalar.value);
        }else if(strcmp(stack_name[*depth], "so_dir") == 0){
            config->so_dir = strdup((char *)token->data.scalar.value);
        }else if(strcmp(stack_name[*depth], "so_name") == 0){
            config->so_name = strdup((char *)token->data.scalar.value);
        }else if(strcmp(stack_name[*depth], "open") == 0){
            config->open = strcmp((char *)token->data.scalar.value, "true") ? 1: 0;
        }else if(strcmp(stack_name[*depth], "description") == 0){
            config->description = strdup((char *)token->data.scalar.value);
        }else if(strcmp(stack_name[*depth], "init_fun") == 0){
            config->init_fun = strdup((char *)token->data.scalar.value);
        }else if(strcmp(stack_name[*depth], "run_func") == 0){
            config->run_func = strdup((char *)token->data.scalar.value);
        }else if(strcmp(stack_name[*depth], "destory_fun") == 0){
            config->destory_fun = strdup((char *)token->data.scalar.value);
        }else if(strcmp(stack_name[*depth], "reset_fun") == 0){
            config->reset_fun = strdup((char *)token->data.scalar.value);
        }
    }

}

void parse_sequence(yaml_parser_t *parser, PluginConfig *config, yaml_token_t *token, int *depth) {
    // This function should handle parsing sequences
    // and fill the appropriate fields in the config structure
}

void print_plugin_config(const PluginConfig *config) {
    printf("Plugin Configuration:\n");
    printf("  Name: %s\n", config->name ? config->name : "NULL");
    printf("  SO Directory: %s\n", config->so_dir ? config->so_dir : "NULL");
    printf("  SO Name: %s\n", config->so_name ? config->so_name : "NULL");
    printf("  Open: %s\n", config->open ? "true" : "false");
    printf("  Description: %s\n", config->description ? config->description : "NULL");

    // printf("  Config Files:\n");
    // for (int i = 0; i < config->config.file_count; ++i) {
    //     printf("    File %d: %s\n", i + 1, config->config.files[i]);
    // }

    // printf("  SQL Configurations:\n");
    // for (int i = 0; i < config->config.sql_count; ++i) {
    //     printf("    SQL Config %d:\n", i + 1);
    //     printf("      Database: %s\n", config->config.sql_configs[i].database);
    //     printf("      SQL Command: %s\n", config->config.sql_configs[i].sql_cmd);
    //     printf("      Check Function: %s\n", config->config.sql_configs[i].check_fun);
    // }

    // printf("  Hooks:\n");
    // for (int i = 0; i < config->hook_count; ++i) {
    //     printf("    Hook %d:\n", i + 1);
    //     printf("      Name: %s\n", config->hooks[i].hook_name);
    //     printf("      Description: %s\n", config->hooks[i].hook_description);
    //     printf("      Is Use: %s\n", config->hooks[i].is_use ? "true" : "false");
    // }

    // printf("  Register Hooks:\n");
    // for (int i = 0; i < config->register_hook_count; ++i) {
    //     printf("    Register Hook %d:\n", i + 1);
    //     printf("      Plugin Name: %s\n", config->register_hooks[i].plugin_name);
    //     printf("      Hook Name: %s\n", config->register_hooks[i].hook_name);
    //     printf("      Is Use: %s\n", config->register_hooks[i].is_use ? "true" : "false");
    //     printf("      Function Name: %s\n", config->register_hooks[i].func_name);
    //     printf("      Priority: %d\n", config->register_hooks[i].priority);
    //     printf("      Function Description: %s\n", config->register_hooks[i].func_description);
    // }

    printf("  Init Function: %s\n", config->init_fun ? config->init_fun : "NULL");
    printf("  Run Function: %s\n", config->run_func ? config->run_func : "NULL");
    printf("  Destroy Function: %s\n", config->destory_fun ? config->destory_fun : "NULL");
    printf("  Reset Function: %s\n", config->reset_fun ? config->reset_fun : "NULL");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <yaml-file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    PluginConfig config = {0};
    parse_yaml(argv[1], &config);
    print_plugin_config(&config);

    // Print parsed data for verification
    // ...

    free_plugin_config(&config);

    return EXIT_SUCCESS;
}
