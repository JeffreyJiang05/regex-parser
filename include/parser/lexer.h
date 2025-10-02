#ifndef LEXER_H
#define LEXER_H

#include <stdlib.h>

#include "macro.h"
#include "parser/loc.h"

typedef enum token_type
{
    BASIC_SYMBOL,
    ESCAPED_SYMBOL, 
    CLASS_SYMBOL,
    NUMBER,
    LPAREN,
    RPAREN,
    LBRACKET,
    RBRACKET,
    LBRACE,
    RBRACE,
    ASTERISK,
    PLUS,
    QUESTION,
    COMMA,
    UNION,
    MINUS,
    END = -1
} TOKEN_TYPE;

typedef enum class_symbol
{
    WHITESPACE,
    DIGIT,
    WORD
} CLASS_SYMBOL_TYPE;

typedef struct token * TOKEN;

TOKEN_TYPE token_get_type(TOKEN tok);

int token_get_basic_symbol(TOKEN tok);

size_t token_get_number(TOKEN tok);

char * token_get_text(TOKEN tok);

CLASS_SYMBOL_TYPE token_get_class_symbol(TOKEN tok);

// ---------------------------------------------------------------------------------------------------- //

typedef enum lexer_status
{
    LEX_SUCCESS = 0,
    LEX_WARNING = 1,
    LEX_UNRECOGNIZED_TOKEN = -1,
    LEX_UNRECOGNIZED_SYMBOL = -2,
    LEX_UNKNOWN_ERROR = -3
} LEXER_STATUS;

typedef struct regex_lexer * LEXER;

LEXER lex_init(const char *regex);

void lex_fini(LEXER lexer);

const char *lex_get_regex(LEXER lexer);

LEXER_STATUS lex_status(LEXER lexer);

TOKEN lex_peek_token(LEXER lexer);

TOKEN_TYPE lex_peek_token_type(LEXER lexer);

LOC lex_peek_token_loc(LEXER lexer);

int lex_peek_token_is_impl(LEXER lexer, size_t count, ...);
#define lex_peek_token_is(lexer, ...) lex_peek_token_is_impl(lexer, NARGS(__VA_ARGS__), ## __VA_ARGS__)

void lex_consume_token(LEXER lexer);

#endif