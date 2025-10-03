#include "parser/lexer.h"

#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdio.h>

#include "parser/loc.h"
#include "parser/error.h"
#include "parser/config.h"
#include "utility/map.h"

#define TOKEN_STR_SIZE 32

struct token
{
    TOKEN_TYPE type;
    union
    {
        int number;
        CLASS_SYMBOL_TYPE class_symbol;
        int basic_symbol;
    };
    char *str;
};

// SINGLETON INSTANCES FOR TOKENS
static struct token LParenToken   = { LPAREN,   .str = "LParen"   };
static struct token RParenToken   = { RPAREN,   .str = "RParen"   };
static struct token LBracketToken = { LBRACKET, .str = "LBracket" };
static struct token RBracketToken = { RBRACKET, .str = "RBracket" };
static struct token LBraceToken   = { LBRACE,   .str = "LBrace"   };
static struct token RBraceToken   = { RBRACE,   .str = "RBrace"   };
static struct token AsteriskToken = { ASTERISK, .str = "Asterisk" };
static struct token PlusToken     = { PLUS,     .str = "Plus"     };
static struct token QuestionToken = { QUESTION, .str = "Question" };
static struct token CommaToken    = { COMMA,    .str = "Comma"    };
static struct token UnionToken    = { UNION,    .str = "Union"    };
static struct token MinusToken    = { MINUS,    .str = "Minus"    };
static struct token EOFToken      = { END,      .str = "END"      };

// SINGLETON INSTANCES FOR ESCAPED SYMBOLS
static struct token EscapedBackslashToken = { ESCAPED_SYMBOL, { .basic_symbol = '\\' }, .str = "EscSymbol[\\]"      };
static struct token EscapedTabToken       = { ESCAPED_SYMBOL, { .basic_symbol = '\t' }, .str = "EscSymbol[TAB]"     };
static struct token EscapedNewLineToken   = { ESCAPED_SYMBOL, { .basic_symbol = '\n' }, .str = "EscSymbol[NEWLINE]" };
static struct token EscapedLBracketToken  = { ESCAPED_SYMBOL, { .basic_symbol = '['  }, .str = "EscSymbol['[']"     };
static struct token EscapedRBracketToken  = { ESCAPED_SYMBOL, { .basic_symbol = ']'  }, .str = "EscSymbol[']']"     };
static struct token EscapedLParenToken    = { ESCAPED_SYMBOL, { .basic_symbol = '('  }, .str = "EscSymbol[(]"       };
static struct token EscapedRParenToken    = { ESCAPED_SYMBOL, { .basic_symbol = ')'  }, .str = "EscSymbol[)]"       };
static struct token EscapedLBraceToken    = { ESCAPED_SYMBOL, { .basic_symbol = '{'  }, .str = "EscSymbol[{]"       };
static struct token EscapedRBraceToken    = { ESCAPED_SYMBOL, { .basic_symbol = '}'  }, .str = "EscSymbol[}]"       };
static struct token EscapedAsteriskToken  = { ESCAPED_SYMBOL, { .basic_symbol = '*'  }, .str = "EscSymbol[*]"       };
static struct token EscapedPlusToken      = { ESCAPED_SYMBOL, { .basic_symbol = '+'  }, .str = "EscSymbol[+]"       };
static struct token EscapedQuestionToken  = { ESCAPED_SYMBOL, { .basic_symbol = '?'  }, .str = "EscSymbol[?]"       };
static struct token EscapedUnionToken     = { ESCAPED_SYMBOL, { .basic_symbol = '|'  }, .str = "EscSymbol[|]"       };
static struct token EscapedMinusToken     = { ESCAPED_SYMBOL, { .basic_symbol = '-'  }, .str = "EscSymbol[-]"       };

// SINGLETON INSTANCES FOR CLASS SYMBOLS
static struct token WhitespaceClassToken = { .type = CLASS_SYMBOL, { .class_symbol = WHITESPACE }, .str = "WhitespaceClass" };
static struct token DigitClassToken      = { .type = CLASS_SYMBOL, { .class_symbol = DIGIT },      .str = "DigitClass"      };
static struct token WordClassToken       = { .type = CLASS_SYMBOL, { .class_symbol = WORD },       .str = "WordClass"       };

TOKEN_TYPE token_get_type(TOKEN tok)
{
    return tok->type;
}

int token_get_basic_symbol(TOKEN tok)
{
    return tok->basic_symbol;
}

size_t token_get_number(TOKEN tok)
{
    return tok->number;
}

CLASS_SYMBOL_TYPE token_get_class_symbol(TOKEN tok)
{
    return tok->class_symbol;
}

char * token_get_text(TOKEN tok)
{
    return tok->str;
}

// ---------------------------------------------------------------------------------------------------- //

struct regex_lexer
{
    MAP token_symbol_map; 
    MAP token_number_map;
    char *regex;
    size_t curr;
    LEXER_STATUS status;
    char class_context;
    char range_context; 
    TOKEN peek_token;
    LOC peek_loc;
    LEXER_CONFIG conf;
};

LEXER lex_init(const char *regex)
{
    size_t regex_len = strlen(regex);

    LEXER lexer = malloc(sizeof(struct regex_lexer));
    lexer->token_symbol_map = map_init();
    lexer->token_number_map = map_init();
    lexer->regex = calloc(regex_len + 1, sizeof(char));
    strcpy(lexer->regex, regex);
    lexer->curr = 0;
    lexer->status = LEX_SUCCESS;
    lexer->class_context = lexer->range_context = 0;
    lexer->peek_token = NULL;
    lexer->peek_loc.begin = lexer->peek_loc.end = 0;
    lexer->conf = lexconf_init();

    lex_consume_token(lexer);
    return lexer;
}

void lex_fini(LEXER lexer)
{
    // free all symbol tokens
    void *token;
    MAP_ITERATOR iter = map_iterator_init(lexer->token_symbol_map);
    while (map_iterator_has_next(iter))
    {
        map_iterator_next(iter, NULL, &token);
        if (token)
        {
            if (((TOKEN) token)->type == BASIC_SYMBOL)
                free(((TOKEN) token)->str);
            free(token);
        }
    }
    map_iterator_fini(iter);
    map_fini(lexer->token_symbol_map);
    // free all number
    iter = map_iterator_init(lexer->token_number_map);
    while (map_iterator_has_next(iter))
    {
        map_iterator_next(iter, NULL, &token);
        free(((TOKEN) token)->str);
        free(token);
    }
    map_iterator_fini(iter);
    map_fini(lexer->token_number_map);
    lexconf_fini(lexer->conf);
    free(lexer->regex);
    free(lexer);
}

LEXER_CONFIG lex_config(LEXER lexer)
{
    return lexer->conf;
}

void lex_config_set(LEXER lexer, LEXER_CONFIG config)
{
    if (!lexer || !config) return;
    lexconf_fini(lexer->conf);
    lexer->conf = config;
}

const char *lex_get_regex(LEXER lexer)
{
    return lexer->regex;
}

LEXER_STATUS lex_status(LEXER lexer)
{
    return lexer->status;
}

TOKEN lex_peek_token(LEXER lexer)
{
    return lexer->peek_token;
}

TOKEN_TYPE lex_peek_token_type(LEXER lexer)
{
    return lexer->peek_token->type;
}

LOC lex_peek_token_loc(LEXER lexer)
{
    return lexer->peek_loc;
}

int lex_peek_token_is_impl(LEXER lexer, size_t count, ...)
{
    TOKEN_TYPE tok_type = lexer->peek_token->type;
    va_list va;
    va_start(va, count);
    for (size_t i = 0; i < count; ++i)
    {
        TOKEN_TYPE expected_type = va_arg(va, TOKEN_TYPE);
        if (tok_type == expected_type)
        {
            va_end(va);
            return 1;
        }
    }
    va_end(va);
    return 0;
}

#define UPDATE_LEXER_CURR(lexer, current)  \
do {                                       \
    lexer->curr = current;                 \
} while (0)

#define UPDATE_LEXER_TOKEN(lexer, token, current, tok_begin, tok_end) \
do {                                                \
    lexer->peek_token = token;                      \
    lexer->peek_loc = (LOC){ tok_begin, tok_end };  \
    lexer->curr = current;                          \
} while (0) 

#define UPDATE_LEXER_STATE(lexer, class_ctx, range_ctx)             \
do {                                                                \
    if ((class_ctx) != -1) lexer->class_context = class_ctx;        \
    if ((range_ctx) != -1) lexer->range_context = range_ctx;        \
} while (0)

static TOKEN retrieve_token_from_map(LEXER lexer, char symbol)
{
    TOKEN tok = map_get(lexer->token_symbol_map, symbol);
    if (tok) return tok;
    tok = malloc(sizeof(struct token));
    tok->type = BASIC_SYMBOL;
    tok->basic_symbol = symbol;
    tok->str = calloc(10, sizeof(char));
    sprintf(tok->str, "Symbol[%c]", symbol);
    map_add(lexer->token_symbol_map, symbol, tok);
    return tok;
}

static TOKEN retrieve_number_from_map(LEXER lexer, int num)
{
    TOKEN tok = map_get(lexer->token_number_map, num);
    if (tok) return tok;
    tok = malloc(sizeof(struct token));
    tok->type = NUMBER;
    tok->number = num;
    tok->str = calloc(20, sizeof(char));
    sprintf(tok->str, "Number[%d]", num);
    map_add(lexer->token_number_map, num, tok);
    return tok;
}

static void gen_ctx_next_tok(LEXER lexer)
{
    size_t curr = lexer->curr;
    size_t start = curr;
    char c = lexer->regex[curr++];
    // the start of the token

    if (c == 0) // NULL TERMINATOR, END OF STRING
    {
        UPDATE_LEXER_TOKEN(lexer, &EOFToken, curr, start, curr);
        UPDATE_LEXER_STATE(lexer, 0, 0);
    }
    else if (c == '\\') // ESCAPE CHARACTER
    {
        // read next character
        c = lexer->regex[curr++];
        switch (c)
        {
        // HANDLE ESCAPE CHARACTERS
        case '\\':
            UPDATE_LEXER_TOKEN(lexer, &EscapedBackslashToken, curr, start, curr); break;
        case 't':
            UPDATE_LEXER_TOKEN(lexer, &EscapedTabToken, curr, start, curr); break;
        case 'n':
            UPDATE_LEXER_TOKEN(lexer, &EscapedNewLineToken, curr, start, curr); break;
        case '[':
            UPDATE_LEXER_TOKEN(lexer, &EscapedLBracketToken, curr, start, curr); break;
        case ']':
            UPDATE_LEXER_TOKEN(lexer, &EscapedRBracketToken, curr, start, start); break;
        case '(':
            UPDATE_LEXER_TOKEN(lexer, &EscapedLParenToken, curr, start, curr); break;
        case ')':
            UPDATE_LEXER_TOKEN(lexer, &EscapedRParenToken, curr, start, curr); break;
        case '{':
            UPDATE_LEXER_TOKEN(lexer, &EscapedLBraceToken, curr, start, curr); break;
        case '}':
            UPDATE_LEXER_TOKEN(lexer, &EscapedRBraceToken, curr, start, curr); break;
        case '*':
            UPDATE_LEXER_TOKEN(lexer, &EscapedAsteriskToken, curr, start, curr); break;
        case '+':
            UPDATE_LEXER_TOKEN(lexer, &EscapedPlusToken, curr, start, curr); break;
        case '?':
            UPDATE_LEXER_TOKEN(lexer, &EscapedQuestionToken, curr, start, curr); break;
        case '|':
            UPDATE_LEXER_TOKEN(lexer, &EscapedUnionToken, curr, start, curr); break;
        // HANDLE CLASSES NOW
        case 's':
            UPDATE_LEXER_TOKEN(lexer, &WhitespaceClassToken, curr, start, curr); break;
        case 'd':
            UPDATE_LEXER_TOKEN(lexer, &DigitClassToken, curr, start, curr); break;
        case 'w':
            UPDATE_LEXER_TOKEN(lexer, &WordClassToken, curr, start, curr); break;
        case 0:
            if (lexconf_get(lexer->conf, IGN_UNKNOWN_ESC_SEQ))
            {
                lexer->status = LEX_WARNING;
                errlogs_report_warning(lexer->regex, (LOC){ start, curr - 1 }, "No character following escape character! Ignoring.");
                UPDATE_LEXER_CURR(lexer, curr);
            }
            else
            {
                lexer->status = LEX_UNRECOGNIZED_TOKEN;
                errlogs_report_error(lexer->regex, (LOC){ start, curr - 1 }, "No character following escape character!" );
            }
            break;
        default:
            if (lexconf_get(lexer->conf, IGN_UNKNOWN_ESC_SEQ))
            {
                lexer->status = LEX_WARNING;
                errlogs_report_warning(lexer->regex, (LOC){ start, curr }, "Unrecognized escaped sequence. Ignoring.");
                UPDATE_LEXER_CURR(lexer, curr);
                // we attempt to find the next token after the invalid escape sequence!
                gen_ctx_next_tok(lexer);
            }
            else
            {
                lexer->status = LEX_UNRECOGNIZED_TOKEN;
                errlogs_report_error(lexer->regex, (LOC){ start, curr }, "Unrecognized escaped sequence!" );
            }
        }
    }
    else if (isprint(c))
    {
        switch (c)
        {
        case '[':
            UPDATE_LEXER_TOKEN(lexer, &LBracketToken, curr, start, curr);
            UPDATE_LEXER_STATE(lexer, 1, 0);
            break;
        case ']':
            // #if PARSE_CONFIG(TREAT_UNEXPECTED_TOKENS_AS_ESCAPED)
            if (lexconf_get(lexer->conf, TREAT_UNEXPECTED_TOK_AS_ESC))
            {
                lexer->status = LEX_WARNING;
                errlogs_report_warning(lexer->regex, (LOC){ start, curr }, "Unexpected token. Treating as if escaped.");
                UPDATE_LEXER_TOKEN(lexer, &EscapedRBracketToken, curr, start, curr);
            }
            else
            {
                // propagate it so it becomes an syntactic error not a lexical error
                UPDATE_LEXER_TOKEN(lexer, &RBracketToken, curr, start, curr);
            }
            break;
        case '(':
            UPDATE_LEXER_TOKEN(lexer, &LParenToken, curr, start, curr);
            break;
        case ')':
            UPDATE_LEXER_TOKEN(lexer, &RParenToken, curr, start, curr);
            break;
        case '{':
            UPDATE_LEXER_TOKEN(lexer, &LBraceToken, curr, start, curr);
            UPDATE_LEXER_STATE(lexer, 0, 1);
            break;
        case '}':
            if (lexconf_get(lexer->conf, TREAT_UNEXPECTED_TOK_AS_ESC))
            {
                lexer->status = LEX_WARNING;
                errlogs_report_warning(lexer->regex, (LOC){ start, curr }, "Unexpected token. Treating as if escaped.");
                UPDATE_LEXER_TOKEN(lexer, &EscapedRBraceToken, curr, start, curr);
            }
            else
            {
                // propagate it so it becomes an syntactic error not a lexical error
                UPDATE_LEXER_TOKEN(lexer, &RBraceToken, curr, start, curr);
            }
            break;
        case '*':
            UPDATE_LEXER_TOKEN(lexer, &AsteriskToken, curr, start, curr);
            break;
        case '+':
            UPDATE_LEXER_TOKEN(lexer, &PlusToken, curr, start, curr);
            break;
        case '?':
            UPDATE_LEXER_TOKEN(lexer, &QuestionToken, curr, start, curr);
            break;
        case '|':
            UPDATE_LEXER_TOKEN(lexer, &UnionToken, curr, start, curr);
            break;
        default:
            // a regular symbol we will construct
            TOKEN tok = retrieve_token_from_map(lexer, c);
            UPDATE_LEXER_TOKEN(lexer, tok, curr, start, curr);
            break;
        }
    }
    else
    {
        // #if PARSE_CONFIG(IGNORE_NONPRINTABLE_REGEX_CHAR)
        if (lexconf_get(lexer->conf, IGN_NONPRINT_REGEX_SYM))
        {
            lexer->status = LEX_WARNING;
            errlogs_report_warning(lexer->regex, (LOC){ start, curr }, "Nonprintable character in regular expression. Ignoring.");
            UPDATE_LEXER_CURR(lexer, curr + 1);
            gen_ctx_next_tok(lexer);
        }
        else
        {
            lexer->status = LEX_UNRECOGNIZED_SYMBOL;
            errlogs_report_error(lexer->regex, (LOC){ start, curr }, "Nonprintable character in regular expression.");
        }
    }
}

static void class_ctx_next_tok(LEXER lexer)
{
    size_t curr = lexer->curr;
    size_t start = curr;
    char c = lexer->regex[curr++];

    if (c == 0)
    {
        UPDATE_LEXER_TOKEN(lexer, &EOFToken, curr, start, curr);
        UPDATE_LEXER_STATE(lexer, 0, 0);
    }
    else if (c == '-')
    {
        UPDATE_LEXER_TOKEN(lexer, &MinusToken, curr, start, curr);
    }
    else if (c == ']')
    {
        UPDATE_LEXER_TOKEN(lexer, &RBracketToken, curr, start, curr);
        UPDATE_LEXER_STATE(lexer, 0, 0);
    }
    else if (c == '\\') // if it is an escape symbol 
    {
        c = lexer->regex[curr++];
        switch (c)
        {
        // HANDLE ESCAPE CHARACTERS
        case '\\':
            UPDATE_LEXER_TOKEN(lexer, &EscapedBackslashToken, curr, start, curr); break;
        case 't':
            UPDATE_LEXER_TOKEN(lexer, &EscapedTabToken, curr, start, curr); break;
        case 'n':
            UPDATE_LEXER_TOKEN(lexer, &EscapedNewLineToken, curr, start, curr); break;
        case '[':
            UPDATE_LEXER_TOKEN(lexer, &EscapedLBracketToken, curr, start, curr); break;
        case ']':
            UPDATE_LEXER_TOKEN(lexer, &EscapedRBracketToken, curr, start, start); break;
        case '(':
            UPDATE_LEXER_TOKEN(lexer, &EscapedLParenToken, curr, start, curr); break;
        case ')':
            UPDATE_LEXER_TOKEN(lexer, &EscapedRParenToken, curr, start, curr); break;
        case '{':
            UPDATE_LEXER_TOKEN(lexer, &EscapedLBraceToken, curr, start, curr); break;
        case '}':
            UPDATE_LEXER_TOKEN(lexer, &EscapedRBraceToken, curr, start, curr); break;
        case '*':
            UPDATE_LEXER_TOKEN(lexer, &EscapedAsteriskToken, curr, start, curr); break;
        case '+':
            UPDATE_LEXER_TOKEN(lexer, &EscapedPlusToken, curr, start, curr); break;
        case '?':
            UPDATE_LEXER_TOKEN(lexer, &EscapedQuestionToken, curr, start, curr); break;
        case '|':
            UPDATE_LEXER_TOKEN(lexer, &EscapedUnionToken, curr, start, curr); break;
        case '-':
            UPDATE_LEXER_TOKEN(lexer, &EscapedMinusToken, curr, start, curr); break;
        // HANDLE CLASSES NOW
        case 's':
            UPDATE_LEXER_TOKEN(lexer, &WhitespaceClassToken, curr, start, curr); break;
        case 'd':
            UPDATE_LEXER_TOKEN(lexer, &DigitClassToken, curr, start, curr); break;
        case 'w':
            UPDATE_LEXER_TOKEN(lexer, &WordClassToken, curr, start, curr); break;
        // HANDLE TRAILING BLACKSLASH
        case 0:
            if (lexconf_get(lexer->conf, IGN_UNKNOWN_ESC_SEQ))
            {
                lexer->status = LEX_WARNING;
                errlogs_report_warning(lexer->regex, (LOC){ start, curr - 1 }, "No character following escape character! Ignoring.");
                UPDATE_LEXER_CURR(lexer, curr);
            }
            else
            {
                lexer->status = LEX_UNRECOGNIZED_TOKEN;
                errlogs_report_error(lexer->regex, (LOC){ start, curr - 1 }, "No character following escape character!" );
            }
            break;
        default:
            if (lexconf_get(lexer->conf, IGN_UNKNOWN_ESC_SEQ))
            {
                lexer->status = LEX_WARNING;
                errlogs_report_warning(lexer->regex, (LOC){ start, curr }, "Unrecognized escaped sequence. Ignoring.");
                UPDATE_LEXER_CURR(lexer, curr);
                // we attempt to find the next token after the invalid escape sequence!
                class_ctx_next_tok(lexer);
            }
            else
            {
                lexer->status = LEX_UNRECOGNIZED_TOKEN;
                errlogs_report_error(lexer->regex, (LOC){ start, curr }, "Unrecognized escaped sequence!" );
            }
        }
    }
    else if (isprint(c))
    {
        switch (c)
        {
        case '[':
            UPDATE_LEXER_TOKEN(lexer, &LBracketToken, curr, start, curr);
            break;
        case '(':
            UPDATE_LEXER_TOKEN(lexer, &LParenToken, curr, start, curr);
            break;
        case ')':
            if (lexconf_get(lexer->conf, TREAT_UNEXPECTED_TOK_AS_ESC))
            {
                lexer->status = LEX_WARNING;
                errlogs_report_warning(lexer->regex, (LOC){ start, curr }, "Unexpected token in character class. Treating as if escaped.");
                UPDATE_LEXER_TOKEN(lexer, &EscapedRParenToken, curr, start, curr);
            }
            else
            {
                // propagate it so it becomes an syntactic error not a lexical error
                UPDATE_LEXER_TOKEN(lexer, &RParenToken, curr, start, curr);
            }
            break;
        case '{':
            if (lexconf_get(lexer->conf, TREAT_UNEXPECTED_TOK_AS_ESC))
            {
                lexer->status = LEX_WARNING;
                errlogs_report_warning(lexer->regex, (LOC){ start, curr }, "Unexpected token in character class. Treating as if escaped.");
                UPDATE_LEXER_TOKEN(lexer, &EscapedLBraceToken, curr, start, curr);
            }
            else
            {
                // propagate it so it becomes an syntactic error not a lexical error
                UPDATE_LEXER_TOKEN(lexer, &LBraceToken, curr, start, curr);
            }
            break;
        case '}':
            if (lexconf_get(lexer->conf, TREAT_UNEXPECTED_TOK_AS_ESC))
            {
                lexer->status = LEX_WARNING;
                errlogs_report_warning(lexer->regex, (LOC){ start, curr }, "Unexpected token in character class. Treating as if escaped.");
                UPDATE_LEXER_TOKEN(lexer, &EscapedRBraceToken, curr, start, curr);
            }
            else
            {
                UPDATE_LEXER_TOKEN(lexer, &RBraceToken, curr, start, curr);
            }
            break;
        case '*':
            UPDATE_LEXER_TOKEN(lexer, &AsteriskToken, curr, start, curr);
            break;
        case '+':
            UPDATE_LEXER_TOKEN(lexer, &PlusToken, curr, start, curr);
            break;
        case '?':
            UPDATE_LEXER_TOKEN(lexer, &QuestionToken, curr, start, curr);
            break;
        case '|':
            UPDATE_LEXER_TOKEN(lexer, &UnionToken, curr, start, curr);
            break;
        default:
            // a regular symbol we will construct
            TOKEN tok = retrieve_token_from_map(lexer, c);
            UPDATE_LEXER_TOKEN(lexer, tok, curr, start, curr);
            break;
        }
    }
    else
    {
        if (lexconf_get(lexer->conf, IGN_NONPRINT_REGEX_SYM))
        {
            lexer->status = LEX_WARNING;
            errlogs_report_warning(lexer->regex, (LOC){ start, curr }, "Nonprintable character in regular expression. Ignoring.");
            UPDATE_LEXER_CURR(lexer, curr + 1);
            class_ctx_next_tok(lexer);
        }
        else
        {
            lexer->status = LEX_UNRECOGNIZED_SYMBOL;
            errlogs_report_error(lexer->regex, (LOC){ start, curr }, "Nonprintable character in regular expression.");
        }
    }
}

static void range_ctx_next_tok(LEXER lexer)
{
    size_t curr = lexer->curr;
    size_t start = curr;
    char c = lexer->regex[curr++];

    if (c == 0)
    {
        UPDATE_LEXER_TOKEN(lexer, &EOFToken, curr, start, curr);
        UPDATE_LEXER_STATE(lexer, 0, 0);
    }
    else if (c == '}')
    {
        UPDATE_LEXER_TOKEN(lexer, &RBraceToken, curr, start, curr);
        UPDATE_LEXER_STATE(lexer, 0, 0);
    }
    else if (c == ',')
    {
        UPDATE_LEXER_TOKEN(lexer, &CommaToken, curr, start, curr);
    }
    else if (isdigit(c))
    {
        // build the integer
        int value = 0;
        do
        {
            value = value * 10 + c - '0';
            c = lexer->regex[curr++];
        } while (isdigit(c));
        TOKEN tok = retrieve_number_from_map(lexer, value);
        UPDATE_LEXER_TOKEN(lexer, tok, curr - 1, start, curr - 1);
    }
    else
    {
        // we exit from this context and reattempt
        UPDATE_LEXER_STATE(lexer, 0, 0);
        lex_consume_token(lexer);
    }
}

void lex_consume_token(LEXER lexer)
{
    // if we have run into an error, we stop attempting to tokenize
    if (lexer->status < 0) return;
    // we are done, we can not consume anymore
    if (lexer->peek_token == &EOFToken) return;

    if (lexer->class_context)
    {
        class_ctx_next_tok(lexer);
    }
    else if (lexer->range_context)
    {
        range_ctx_next_tok(lexer);
    }
    else
    {
        gen_ctx_next_tok(lexer);
    }
    
}
