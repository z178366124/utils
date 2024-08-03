#include "plugin_parse.h"

void parse_yaml(const char *filename) {
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

    do {
        yaml_parser_scan(&parser, &token);
        switch (token.type) {
            case YAML_STREAM_START_TOKEN:
                // puts("yaml stream start...");
                break;
            case YAML_STREAM_END_TOKEN:
                puts("yaml stream end...");
                break;
            case YAML_KEY_TOKEN: 
                printf("(Key token)  "); 
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