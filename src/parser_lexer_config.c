#include "parser/config.h"

#include <stdlib.h>

struct lexer_config
{
    char fields[LEXER_CONF_LENGTH];
};

LEXER_CONFIG lexconf_init()
{
    LEXER_CONFIG config = calloc(1, sizeof(struct lexer_config));
    return config;
}

void lexconf_fini(LEXER_CONFIG conf)
{
    free(conf);
}

int lexconf_get(LEXER_CONFIG conf, LEXER_CONFIG_FIELD field)
{
    if (field < 0 || field >= LEXER_CONF_LENGTH) return -1;
    return conf->fields[field];
}

int lexconf_set(LEXER_CONFIG conf, LEXER_CONFIG_FIELD field, int new_value)
{
    if (field < 0 || field >= LEXER_CONF_LENGTH) return -1;
    char old_value = conf->fields[field];
    conf->fields[field] = new_value;
    return old_value;
}
