#ifndef LEXER_H
#define LEXER_H

#include <stdlib.h>

#include "macro.h"
#include "parser/loc.h"
#include "parser/config.h"

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

/**
 * retrieve type of the token
 * 
 * @param tok the token
 * @return the type of the token
 */
TOKEN_TYPE token_get_type(TOKEN tok);

/**
 * retrieves the basic symbol stored with the token
 * 
 * @param tok the token
 * @return the basic symbol stored with the token
 * @warning this function should ONLY be used on tokens of type BASIC_SYMBOL or ESCAPED_SYMBOL.
 */
int token_get_basic_symbol(TOKEN tok);

/**
 * retrieves the number stored with the token
 * 
 * @param tok the token
 * @return the number stored with the token
 * @warning this function should ONLY be used on tokens of type NUMBER
 */
size_t token_get_number(TOKEN tok);

/**
 * returns the lexeme or the text that was matched with the token
 * 
 * @param tok the token
 * @return the text that was matched with the token
 */
char * token_get_text(TOKEN tok);

/**
 * retrieves the class symbol type stored with the token 
 * 
 * @param tok the token
 * @return the class symbol stored in the token 
 * @warning this function should ONLY be used on tokens of type CLASS_SYMBOL
 */
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

/**
 * handle for the lexer instance
 * 
 * the lexer holds ownership to the tokens encountered. 
 *  this means that a TOKEN returned by any of the following function SHOULD NOT be freed
 *  when the lexer instance is destroyed, then all tokens will be freed appropriately
 * 
 * The lexer also maintains that invariant that given two tokens (lexeme0, value0) and
 * (lexeme1, value1), then if lexeme0 == lexeme1 and value0 == value1, then the 
 * TOKEN handle for each are equal. put simply, if two tokens are functionally the same,
 * they will reference the same TOKEN.
 * 
 * however, the even if two tokens are the same, they may refer to different locations. 
 */
typedef struct regex_lexer * LEXER;

/**
 * initializes the lexer with an input regex
 * 
 * @param regex the regular expression to start with
 * @return the newly created lexer
 * @note any lexer created with this function must be destroyed via `lex_fini`
 */
LEXER lex_init(const char *regex);

/**
 * destroys the lexer and all associated memory and tokens
 * 
 * @param lexer the lexer to destroy
 * @note the param to this function must be a lexer created via `lex_init`. 
 * calling this function on a lexer that is already destroyed is UB.
 */
void lex_fini(LEXER lexer);

/**
 * returns a handle to the configuration associated with this lexer
 * 
 * @param lexer the lexer
 * @return a handle to the configuration associated with the lexer
 * @note modifying the configuration here modifies the configuration on the lexer.
 * @warning - do NOT free the handle. ownership over the configuration is managed
 * by the lexer itself. 
 * @warning - later calls to `lex_config_set` or `lex_fini` invalidates the
 * handler making it UB to access or modify the handler. thus, configs should
 * not be stored for long.
 */
LEXER_CONFIG lex_config(LEXER lexer);

/**
 * replaces the configuration handler of the lexer with a new configuration
 * 
 * @param lexer the lexer to configure 
 * @param config a new handler to the configuration
 * @note this function transfers the ownership of `config` to the lexer. it should not
 * be destroyed via `lexconf_fini` 
 * @warning - this will invalidate any living handlers to the previous configuration,
 * making it UB to modify or access them. 
 */
void lex_config_set(LEXER lexer, LEXER_CONFIG config);

/**
 * returns the regular expression the lexer is parsing
 * 
 * @param lexer the lexer
 * @return the regular expression the lexer is parsing
 */
const char *lex_get_regex(LEXER lexer);

/**
 * returns the status of the lexer
 * 
 * @param lexer the lexer
 * @return the status of the lexer
 * @note a value less than zero signifies an error
 */
LEXER_STATUS lex_status(LEXER lexer);

/**
 * returns the current token that is matched
 * 
 * @param lexer the lexer
 * @return returns a handle to the parser the token has matched 
 */
TOKEN lex_peek_token(LEXER lexer);

/**
 * returns the type of the peeked token
 * 
 * @param lexer the lexer
 * @return returns the type of the token that the parser has matched
 */
TOKEN_TYPE lex_peek_token_type(LEXER lexer);

/**
 * returns the location in the original regex that corresponds with
 * the matched token
 * 
 * @param lexer the lexer
 * @return the location that corresponds with the peeked token
 */
LOC lex_peek_token_loc(LEXER lexer);

/**
 * utility function to check if the type of the peek token is any of the following
 * in the list
 * 
 * @param lexer the lexer
 * @param count the number of types to check for
 * @param types the types to check the peek token by
 * @return true if the type of the peek token matched any of the listed. false otherwise
 */
int lex_peek_token_is_impl(LEXER lexer, size_t count, ... /* TOKEN_TYPE types */);

/**
 * utility function to check if the type of the peek token is any of the following
 * in the list
 * 
 * @param lexer the lexer
 * @param __VA_ARGS__ the types to check the peek token by
 */
#define lex_peek_token_is(lexer, ...) lex_peek_token_is_impl(lexer, NARGS(__VA_ARGS__), ## __VA_ARGS__)

/**
 * the parser consumes the current token and attempt to parse another. this may update
 * the status of the parser.
 * 
 * @param lexer the lexer
 */
void lex_consume_token(LEXER lexer);

#endif