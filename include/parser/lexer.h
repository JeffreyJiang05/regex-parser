#ifndef LEXER_H
#define LEXER_H

#include <stdlib.h>

#include "macro.h"

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
    EOF = -1
} TOKEN_TYPE;

typedef enum meta_symbol
{
    WHITESPACE,
    DIGIT,
    WORD
} META_SYMBOL;

typedef struct token * TOKEN;

int token_get_basic_symbol(TOKEN tok);

size_t token_get_number(TOKEN tok);

META_SYMBOL token_get_class_symbol(TOKEN tok);

// ---------------------------------------------------------------------------------------------------- //

typedef struct regex_lexer * LEXER;

LEXER lex_init(const char *regex);

void lex_fini(LEXER lexer);

TOKEN lex_peek_token(LEXER lexer);

int lex_peek_token_is_impl(LEXER lexer, int count, ...);
#define lex_peek_token_is(lexer, ...) lex_peek_token_is_impl(lexer, NARGS(__VA_ARGS__), ## __VA_ARGS__)

void lex_consume_token(LEXER lexer);

#endif