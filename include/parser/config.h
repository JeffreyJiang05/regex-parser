#ifndef PARSER_CONFIG_H
#define PARSER_CONFIG_H

/**
 * COMPILE-TIME CONFIGURATION FOR PARSER
 */

#include "macro.h"

#define PARSE_CONFIG(SETTING) PRIMITIVE_CAT(PARSE_CONFIG_, SETTING)

#define PARSE_CONFIG_IGNORE_UNRECOGNIZED_ESCAPED_SEQUENCE 0
#define PARSE_CONFIG_IGNORE_NONPRINTABLE_REGEX_CHAR 0
#define PARSE_CONFIG_TREAT_UNEXPECTED_TOKENS_AS_ESCAPED 0

typedef enum lexer_config_field
{
    IGNORE_UNKNOWN_ESCAPED_SEQUENCE = 0,
    IGN_UNKNOWN_ESC_SEQ = 0, 
    IGNORE_NONPRINTABLE_REGEX_SYMBOL = 1,
    IGN_NONPRINT_REGEX_SYM = 1, 
    TREAT_UNEXPECTED_TOKENS_AS_ESCAPED = 2,
    TREAT_UNEXPECTED_TOK_AS_ESC = 2,

    LEXER_CONF_LENGTH
} LEXER_CONFIG_FIELD;

/**
 * handler to the lexer configuration
 */
typedef struct lexer_config * LEXER_CONFIG;

/**
 * creates a new lexer configuration
 * 
 * @return the newly created lexer configuration
 * @note the configuration should be destroyed by `lexconf_fini` if
 * the ownership is not transferred to some lexer
 */
LEXER_CONFIG lexconf_init();

/**
 * destroys the given lexer configuration
 * 
 * @param conf the configuration to destroy
 */
void lexconf_fini(LEXER_CONFIG conf);

/**
 * retrieves a value from the lexer configuration
 * 
 * @param conf the configuration
 * @param field the field in the config to retrieve
 * @return the value stored in the config corresponding to the field
 * given that its valid, otherwise -1.
 */
int lexconf_get(LEXER_CONFIG conf, LEXER_CONFIG_FIELD field);

/**
 * sets a value from the lexer configuration
 * 
 * @param conf the configuration
 * @param field the field in the config to modify
 * @param new_value the new value to store in the field
 * @return the old configuration value if the field is valid, otherwise -1.
 */
int lexconf_set(LEXER_CONFIG conf, LEXER_CONFIG_FIELD field, int new_value);

#endif