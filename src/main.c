#include <stdio.h>

#include "debug.h"

#include "parser/error.h"
#include "parser/lexer.h"

int main()
{
    errlogs_install();

    LEXER lexer = lex_init("ABC+|**{}23[1-2]{12}-[\\t-abc12-a-Z\\w-\\d][][{}]{12,32,44}");

    while (lex_status(lexer) >= 0)
    {
        TOKEN tok = lex_peek_token(lexer);
        LOC loc = lex_peek_token_loc(lexer);
        if (token_get_type(tok) == END) break;

        errlogs_report_warning(lex_get_regex(lexer), loc, token_get_text(tok));

        lex_consume_token(lexer);
    }
    printf("\n");

    lex_fini(lexer);

    return 0;
}