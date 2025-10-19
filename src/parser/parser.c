#include "parser/parser.h"

#include <string.h>
#include <stdio.h>

#include "parser/error.h"

// predictive parser
// the parser functions are not speculative 

typedef struct context
{
    char invalid_ast;
} *CONTEXT;

static void lex_consume_until_impl(LEXER lexer, size_t n, ...)
{
    static char lex_consume_until_lookup[END + 1] = { 0 };

    va_list args;
    va_start(args, n);
    for (size_t i = 0; i < n; ++i)
    {
        TOKEN_TYPE tok_type = va_arg(args, TOKEN_TYPE);
        lex_consume_until_lookup[tok_type] = 1; 
    }
    va_end(args);
    lex_consume_until_lookup[END] = 1;

    while (!lex_consume_until_lookup[lex_peek_token_type(lexer)])
        lex_consume_token(lexer);

    memset(lex_consume_until_lookup, 0, sizeof(lex_consume_until_lookup));
}

#define lex_consume_until(lexer, ...) lex_consume_until_impl(lexer, NARGS(__VA_ARGS__), __VA_ARGS__)

static void *parse_symbol(LEXER lexer, CONTEXT ctx)
{
    LOC start_loc = lex_peek_token_loc(lexer);
    TOKEN tok = lex_peek_token(lexer);

    switch (token_get_type(tok))
    {
        case BASIC_SYMBOL:
        {
            lex_consume_token(lexer);
            return ast_new(ASTSymbol, token_get_basic_symbol(tok));
        }
        case ESCAPED_SYMBOL:
        {
            lex_consume_token(lexer);
            return ast_new(ASTSymbol, token_get_basic_symbol(tok));
        }
        case CLASS_SYMBOL:
        {
            lex_consume_token(lexer);
            return ast_new(ASTClassSymbol, token_get_class_symbol(tok));
        }
        default: // ERROR
        {
            ctx->invalid_ast = 1;
            errlogs_report_error(
                lex_get_regex(lexer),
                start_loc,
                "Unexpected token. Expected a basic symbol, escaped symbol, or class symbol here!"
            );
            lex_consume_until(lexer, ASTERISK, PLUS, QUESTION, LBRACE, LPAREN, RPAREN, UNION, 
                LBRACKET, BASIC_SYMBOL, ESCAPED_SYMBOL, CLASS_SYMBOL);
            return ast_new(ASTError);
        }
    }
}

static void *parse_simple_symbol(LEXER lexer, CONTEXT ctx)
{
    LOC start_loc = lex_peek_token_loc(lexer);
    TOKEN tok = lex_peek_token(lexer);

    switch (token_get_type(tok))
    {
        case BASIC_SYMBOL: 
        {
            lex_consume_token(lexer);
            return ast_new(ASTSymbol, token_get_basic_symbol(tok));
        }
        case ESCAPED_SYMBOL:
        {
            lex_consume_token(lexer);
            return ast_new(ASTSymbol, token_get_basic_symbol(tok));
        }
        case CLASS_SYMBOL:
        {
            ctx->invalid_ast = 1;
            errlogs_report_error(
                lex_get_regex(lexer),
                start_loc,
                "Class symbols are not allowed within this context!"
            );
            lex_consume_until(lexer, RBRACKET, MINUS, BASIC_SYMBOL, ESCAPED_SYMBOL);
            return ast_new(ASTError);
        }
        default:
        {
            ctx->invalid_ast = 1;
            errlogs_report_error(
                lex_get_regex(lexer),
                start_loc,
                "Unexpected token. Expected a symbol or escaped symbol here!"
            );
            lex_consume_until(lexer, RBRACKET, MINUS, BASIC_SYMBOL, ESCAPED_SYMBOL);
            return ast_new(ASTError);
        }
    }
}

static void *parse_bracket_symbol_prime(LEXER lexer, CONTEXT ctx, AST_NODE lhs)
{
    LOC start_loc = lex_peek_token_loc(lexer);
    TOKEN tok = lex_peek_token(lexer);

    switch (token_get_type(tok))
    {
        case BASIC_SYMBOL:
        case ESCAPED_SYMBOL:
        case RBRACKET:
        {
            return lhs;            
        }
        case CLASS_SYMBOL:
        {
            ctx->invalid_ast = 1;
            errlogs_report_error(
                lex_get_regex(lexer),
                start_loc,
                "Class symbol can not appear within another character class!"
            );
            lex_consume_until(lexer, RBRACKET, BASIC_SYMBOL, ESCAPED_SYMBOL);
            return ast_new(ASTError);
        }
        case LBRACKET:
        {
            ctx->invalid_ast = 1;
            errlogs_report_error(
                lex_get_regex(lexer),
                start_loc,
                "A character class can not be introduced within another character class!"
            );
            lex_consume_until(lexer, RBRACKET, BASIC_SYMBOL, ESCAPED_SYMBOL);
            return ast_new(ASTError);
        }
        case MINUS:
        {
            lex_consume_token(lexer); // consume minus token
            AST_NODE rhs = parse_simple_symbol(lexer, ctx); // parse the simple symbol
            return ast_new(ASTCharRange, lhs, rhs);
        }
        default:
        {
            ctx->invalid_ast = 1;
            errlogs_report_error(
                lex_get_regex(lexer),
                start_loc,
                "Unexpected token within character class! Did you mean use to its escape character?"
            );
            lex_consume_until(lexer, RBRACKET, BASIC_SYMBOL, ESCAPED_SYMBOL);
            return ast_new(ASTError);
        }
    }
}

static void *parse_bracket_symbol(LEXER lexer, CONTEXT ctx)
{
    LOC start_loc = lex_peek_token_loc(lexer);
    TOKEN tok = lex_peek_token(lexer);

    switch (token_get_type(tok))
    {
        case BASIC_SYMBOL:
        case ESCAPED_SYMBOL:
        {
            AST_NODE lhs = parse_simple_symbol(lexer, ctx);
            return parse_bracket_symbol_prime(lexer, ctx, lhs);
        }
        case CLASS_SYMBOL:
        {
            ctx->invalid_ast = 1;
            errlogs_report_error(
                lex_get_regex(lexer),
                start_loc,
                "Class symbol should not appear within a character class!"
            );
            lex_consume_until(lexer, RBRACKET, BASIC_SYMBOL, ESCAPED_SYMBOL);
            return ast_new(ASTError);
        }
        case RBRACKET:
        {
            ctx->invalid_ast = 1;
            errlogs_report_error(
                lex_get_regex(lexer),
                start_loc,
                "Character class can not appear within another character class!"
            );
            lex_consume_until(lexer, RBRACKET, BASIC_SYMBOL, ESCAPED_SYMBOL);
            return ast_new(ASTError);
        }
        default:
        {
            ctx->invalid_ast = 1;
            errlogs_report_error(
                lex_get_regex(lexer),
                start_loc,
                "Unexpected token within character class! Did you mean to use its escape character?"
            );
            lex_consume_until(lexer, RBRACKET, BASIC_SYMBOL, ESCAPED_SYMBOL);
            return ast_new(ASTError);
        }
    }
}

// creates the bracket list
static void *parse_bracket_list(LEXER lexer, CONTEXT ctx)
{
    LOC start_loc = lex_peek_token_loc(lexer);
    AST_NODE bracket_list = ast_new(ASTCharClass, 0);
    AST_NODE node;

    int is_valid = 0;
    while (lex_peek_token_type(lexer) != RBRACKET)
    {
        is_valid = 1;
        node = parse_bracket_symbol(lexer, ctx);
        ast_add_child(bracket_list, node);
    }

    if (!is_valid)
    {
        ctx->invalid_ast = 1;
        errlogs_report_error(
            lex_get_regex(lexer),
            (LOC) { start_loc.begin - 1, start_loc.end },
            "Character class can not be empty!"
        );
        ast_delete(bracket_list);
        return ast_new(ASTError);
    }
    return bracket_list;
}

static void *parse_bracket_term(LEXER lexer, CONTEXT ctx)
{
    LOC start_loc = lex_peek_token_loc(lexer);
    TOKEN tok = lex_peek_token(lexer);

    switch (token_get_type(tok))
    {
        case BASIC_SYMBOL:
        case ESCAPED_SYMBOL:
        case CLASS_SYMBOL:
        {
            return parse_symbol(lexer, ctx);
        }
        case LBRACKET:
        {
            lex_consume_token(lexer);
            AST_NODE list = parse_bracket_list(lexer, ctx);
            
            LOC end_loc = lex_peek_token_loc(lexer);
            if (lex_peek_token_type(lexer) != RBRACKET)
            {
                errlogs_report_warning(
                    lex_get_regex(lexer),
                    (LOC) { start_loc.begin, end_loc.begin - 1 },
                    "Expected terminating ] to end character class!"
                );
            }
            else lex_consume_token(lexer);
            return list;
        }
        default:
        {
            ctx->invalid_ast = 1;
            errlogs_report_error(
                lex_get_regex(lexer),
                start_loc,
                "Unexpected token! Did you mean to use its escaped character?"
            );
            lex_consume_until(lexer, ASTERISK, PLUS, QUESTION, LBRACE, LPAREN, RPAREN, UNION,
                LBRACKET, BASIC_SYMBOL, ESCAPED_SYMBOL, CLASS_SYMBOL);
            return ast_new(ASTError);
        }
    }
}

// DECLARE FOR parse_group_term
static void *parse_alt_term(LEXER lexer, CONTEXT ctx);

static void *parse_group_term(LEXER lexer, CONTEXT ctx)
{
    LOC start_loc = lex_peek_token_loc(lexer);
    TOKEN tok = lex_peek_token(lexer);

    switch (token_get_type(tok))
    {
        case BASIC_SYMBOL:
        case ESCAPED_SYMBOL:
        case CLASS_SYMBOL:
        case LBRACKET:
        {
            return parse_bracket_term(lexer, ctx);
        }
        case LPAREN:
        {
            lex_consume_token(lexer);
            AST_NODE term = parse_alt_term(lexer, ctx);
            
            LOC end_loc = lex_peek_token_loc(lexer);
            if (lex_peek_token_type(lexer) != RPAREN)
            {
                errlogs_report_warning(
                    lex_get_regex(lexer),
                    (LOC) { start_loc.begin, end_loc.begin - 1 },
                    "Expected terminating ) to end character class!"
                );
            }
            else lex_consume_token(lexer);
            return ast_new(ASTGroup, term);
        }
        default:
        {
            ctx->invalid_ast = 1;
            errlogs_report_error(
                lex_get_regex(lexer),
                start_loc,
                "Unexpected token! Did you mean to use its escaped character?"
            );
            lex_consume_until(lexer, ASTERISK, PLUS, QUESTION, LBRACE, LPAREN, RPAREN, UNION,
                LBRACKET, BASIC_SYMBOL, ESCAPED_SYMBOL, CLASS_SYMBOL);
            return ast_new(ASTError);
        }
    }
}

static void *parse_dup_range_prime(LEXER lexer, CONTEXT ctx, LOC quantifier_start, AST_NODE term, int min)
{
    LOC start_loc = lex_peek_token_loc(lexer);
    TOKEN tok = lex_peek_token(lexer);
    
    switch (token_get_type(tok))
    {
        case NUMBER:
        {
            int max = (int) token_get_number(tok);
            lex_consume_token(lexer);

            if (lex_peek_token_type(lexer) != RBRACE)
            {
                LOC end_loc = lex_peek_token_loc(lexer);
                errlogs_report_warning(
                    lex_get_regex(lexer),
                    (LOC){ quantifier_start.begin, end_loc.begin - 1 },
                    "Expected terminating } to end quantifier."
                );
            }
            else lex_consume_token(lexer);
            return ast_new(ASTRange, term, min, max);
        }
        case RBRACE:
        {
            int max = -1;
            lex_consume_token(lexer);
            return ast_new(ASTRange, term, min, max);   
        }
        case COMMA:
        {
            ctx->invalid_ast = 1;
            errlogs_report_error(
                lex_get_regex(lexer),
                start_loc,
                "Extraneous comma in quantifier! Please remove."
            );
            lex_consume_until(lexer, LPAREN, RPAREN, UNION, LBRACKET, BASIC_SYMBOL, ESCAPED_SYMBOL, CLASS_SYMBOL);
            return term;
        }
        default:
        {
            ctx->invalid_ast = 1;
            errlogs_report_error(
                lex_get_regex(lexer),
                start_loc,
                "Only nonnegative integer values can appear within a quantifier!"
            );
            lex_consume_until(lexer, LPAREN, RPAREN, UNION, LBRACKET, BASIC_SYMBOL, ESCAPED_SYMBOL, CLASS_SYMBOL);
            return term;
        }
    }
}

static void *parse_dup_range(LEXER lexer, CONTEXT ctx, AST_NODE term)
{
    LOC start_loc = lex_peek_token_loc(lexer);
    TOKEN tok = lex_peek_token(lexer);

    switch (token_get_type(tok))
    {
        case NUMBER:
        {
            int min = (int) token_get_number(tok);
            lex_consume_token(lexer);
            LOC end_loc = lex_peek_token_loc(lexer);
            switch (lex_peek_token_type(lexer))
            {
                case COMMA:
                {  
                    lex_consume_token(lexer);
                    return parse_dup_range_prime(lexer, ctx, start_loc, term, min);
                }
                case RBRACE:
                {
                    lex_consume_token(lexer);
                    return ast_new(ASTRange, term, min, min);
                }
                default: //* maybe promote this to a warning with insertion of RBRACE
                {
                    ctx->invalid_ast = 1;
                    errlogs_report_error(
                        lex_get_regex(lexer),
                        end_loc,
                        "Only nonnegative integer values can appear within a quantifier!"
                    );
                    lex_consume_until(lexer, LPAREN, RPAREN, UNION, LBRACKET, BASIC_SYMBOL, ESCAPED_SYMBOL, CLASS_SYMBOL);
                    return term; // return parsed term as is
                }
            }
        }
        case COMMA:
        {
            int min = 0;
            lex_consume_token(lexer); // consume the comma
            LOC end_loc = lex_peek_token_loc(lexer);
            tok = lex_peek_token(lexer);
            switch (token_get_type(tok))
            {
                case NUMBER:
                {
                    int max = token_get_number(tok);
                    lex_consume_token(lexer);

                    if (lex_peek_token_type(lexer) != RBRACE)
                    {
                        end_loc = lex_peek_token_loc(lexer);
                        errlogs_report_warning(
                            lex_get_regex(lexer),
                            (LOC){ start_loc.begin, end_loc.begin - 1 },
                            "Expected terminating } to end quantifier."
                        );
                    } 
                    else lex_consume_token(lexer);
                    return ast_new(ASTRange, term, min, max);
                }
                case RBRACE:
                {
                    errlogs_report_error(
                        lex_get_regex(lexer),
                        (LOC){ start_loc.begin, end_loc.end },
                        "Empty quantifier is not allowed!"
                    );
                    lex_consume_until(lexer, LPAREN, RPAREN, UNION, LBRACKET, BASIC_SYMBOL, ESCAPED_SYMBOL, CLASS_SYMBOL);
                    return term;
                }
                default:
                {
                    ctx->invalid_ast = 1;
                    errlogs_report_error(
                        lex_get_regex(lexer),
                        end_loc,
                        "Only nonnegative integer values can appear within a quantifier!"
                    );
                    lex_consume_until(lexer, LPAREN, RPAREN, UNION, LBRACKET, BASIC_SYMBOL, ESCAPED_SYMBOL, CLASS_SYMBOL);
                    return term;
                }
            }
        }
        default:
        {
            ctx->invalid_ast = 1;
            errlogs_report_error(
                lex_get_regex(lexer),
                start_loc,
                "Only nonnegative integer values can appear within a quantifier!"
            );
            lex_consume_until(lexer, LPAREN, RPAREN, UNION, LBRACKET, BASIC_SYMBOL, ESCAPED_SYMBOL, CLASS_SYMBOL);
            return term;
        }
    }
}

static void *parse_dup_term_prime(LEXER lexer, CONTEXT ctx, AST_NODE term)
{
    LOC start_loc = lex_peek_token_loc(lexer);
    TOKEN tok = lex_peek_token(lexer);

    switch (token_get_type(tok))
    {
        case BASIC_SYMBOL:
        case ESCAPED_SYMBOL:
        case CLASS_SYMBOL:
        case LBRACKET:
        case LPAREN:
        case RPAREN:
        case UNION:
        case END:
        {
            return term;
        }
        case ASTERISK:
        {
            lex_consume_token(lexer);
            return ast_new(ASTRange, term, 0, -1);
        }
        case PLUS:
        {
            lex_consume_token(lexer);
            return ast_new(ASTRange, term, 1, -1);
        }
        case QUESTION:
        {
            lex_consume_token(lexer);
            return ast_new(ASTRange, term, 0, 1);
        }
        case LBRACE:
        {
            lex_consume_token(lexer);
            return parse_dup_range(lexer, ctx, term);
        }
        default:
        {
            ctx->invalid_ast = 1;
            errlogs_report_error(
                lex_get_regex(lexer),
                start_loc,
                "Unexpected token!"
            );
            lex_consume_until(lexer, LPAREN, RPAREN, UNION, LBRACKET, BASIC_SYMBOL, ESCAPED_SYMBOL, CLASS_SYMBOL);
            return ast_new(ASTError);  
        }
    }
}

static void *parse_dup_term(LEXER lexer, CONTEXT ctx)
{
    LOC start_loc = lex_peek_token_loc(lexer);
    TOKEN tok = lex_peek_token(lexer);

    switch (token_get_type(tok))
    {
        case BASIC_SYMBOL:
        case ESCAPED_SYMBOL:
        case CLASS_SYMBOL:
        case LBRACKET:
        case LPAREN:
        {
            AST_NODE term = parse_group_term(lexer, ctx);
            return parse_dup_term_prime(lexer, ctx, term);
        }
        default:
        {
            ctx->invalid_ast = 1;
            errlogs_report_error(
                lex_get_regex(lexer),
                start_loc,
                "Unexpected token! Did you mean to use its escaped character?"
            );
            lex_consume_until(lexer, LPAREN, RPAREN, UNION, LBRACKET, BASIC_SYMBOL, ESCAPED_SYMBOL, CLASS_SYMBOL);
            return ast_new(ASTError);
        }
    }
}

static void *parse_concat_term_prime(LEXER lexer, CONTEXT ctx, AST_NODE lhs)
{
    LOC start_loc = lex_peek_token_loc(lexer);
    TOKEN tok = lex_peek_token(lexer);

    switch (token_get_type(tok))
    {
        case BASIC_SYMBOL:
        case ESCAPED_SYMBOL:
        case CLASS_SYMBOL:
        case LBRACKET:
        case LPAREN:
        {
            AST_NODE rhs = parse_dup_term(lexer, ctx);
            lhs = ast_new(ASTConcat, lhs, rhs);
            AST_NODE result = parse_concat_term_prime(lexer, ctx, lhs);
            return result;
        }
        case RPAREN:
        case UNION:
        case END:
        {
            return lhs;
        }
        default:
        {
            ctx->invalid_ast = 1;
            errlogs_report_error(
                lex_get_regex(lexer),
                start_loc,
                "Unexpected token! Did you mean to use its escaped character?"
            );
            lex_consume_until(lexer, RPAREN, UNION);
            return lhs;
        }
    }
}

static void *parse_concat_term(LEXER lexer, CONTEXT ctx)
{
    LOC start_loc = lex_peek_token_loc(lexer);
    TOKEN tok = lex_peek_token(lexer);

    switch (token_get_type(tok))
    {
        case BASIC_SYMBOL:
        case ESCAPED_SYMBOL:
        case CLASS_SYMBOL:
        case LBRACKET:
        case LPAREN:
        {
            AST_NODE lhs = parse_dup_term(lexer, ctx);
            return parse_concat_term_prime(lexer, ctx, lhs);
        }
        default:
        {
            ctx->invalid_ast = 1;
            errlogs_report_error(
                lex_get_regex(lexer),
                start_loc,
                "Unexpected token! Did you mean to use its escaped character?"
            );
            lex_consume_until(lexer, RPAREN, UNION);
            return ast_new(ASTError);
        }
    }
}

static void *parse_alt_term_prime(LEXER lexer, CONTEXT ctx, AST_NODE lhs)
{
    LOC start_loc = lex_peek_token_loc(lexer);
    TOKEN tok = lex_peek_token(lexer);

    switch (token_get_type(tok))
    {
        case RPAREN:
        case END:
        {
            return lhs;
        }
        case UNION:
        {
            lex_consume_token(lexer);
            AST_NODE rhs = parse_concat_term(lexer, ctx);
            lhs = ast_new(ASTUnion, lhs, rhs);
            AST_NODE result = parse_alt_term_prime(lexer, ctx, lhs);
            return result;
        }
        default:
        {
            ctx->invalid_ast = 1;
            errlogs_report_error(
                lex_get_regex(lexer),
                start_loc,
                "Unexpected token! Did you mean to use its escaped character?"
            );
            lex_consume_until(lexer, RPAREN);
            return lhs; // just return the left hand side which was already parsed
        }
    }
}

static void *parse_alt_term(LEXER lexer, CONTEXT ctx)
{
    LOC start_loc = lex_peek_token_loc(lexer);
    TOKEN tok = lex_peek_token(lexer);

    switch (token_get_type(tok))
    {
        case BASIC_SYMBOL:
        case ESCAPED_SYMBOL:
        case CLASS_SYMBOL:
        case LBRACKET:
        case LPAREN:
        {
            AST_NODE lhs = parse_concat_term(lexer, ctx);
            return parse_alt_term_prime(lexer, ctx, lhs);
        }
        default:
        {
            ctx->invalid_ast = 1;
            errlogs_report_error(
                lex_get_regex(lexer),
                start_loc,
                "Unexpected token! Did you mean to use its escaped character?"
            );
            lex_consume_until(lexer, RPAREN);
            return ast_new(ASTError);
        }
    }
}

static void *parse_alt_terms(LEXER lexer, CONTEXT ctx)
{
    LOC start_loc = lex_peek_token_loc(lexer);
    TOKEN tok = lex_peek_token(lexer);

    switch (token_get_type(tok))
    {
        case BASIC_SYMBOL:
        case ESCAPED_SYMBOL:
        case CLASS_SYMBOL:
        case LBRACKET:
        case LPAREN:
        {
            return parse_alt_term(lexer, ctx);
        }
        default:
        {
            ctx->invalid_ast = 1;
            errlogs_report_error(
                lex_get_regex(lexer),
                start_loc,
                "Unexpected token! Did you mean to use its escaped character?"
            );
            lex_consume_until(lexer, RPAREN);
            return ast_new(ASTError);  
        }      
    }
}

static AST_NODE parse_regex(LEXER lexer, CONTEXT ctx)
{
    if (lex_peek_token_type(lexer) == END)
    {
        ctx->invalid_ast = 1;
        errlogs_report_error(
            "",
            (LOC) { 0, 0 },
            "Empty regex pattern are not accepted!"
        );
        return ast_new(ASTError);
    }
    return parse_alt_terms(lexer, ctx);
}

// ---------------------------------------------------------------------------------------- //

AST_NODE parser_parse(const char *regex)
{
    LEXER lex = lex_init(regex);
    struct context ctx = { .invalid_ast = 0 };
    AST_NODE node = parse_regex(lex, &ctx);
    if (ctx.invalid_ast)
    {
        ast_delete(node);
        node = NULL;
    }
    lex_fini(lex);
    return node;
}