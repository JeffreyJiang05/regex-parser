#include <stdio.h>

#include "debug.h"

#include "parser/error.h"
#include "parser/lexer.h"

#include "parser/ast.h"
#include "parser/parser.h"

int main()
{
    errlogs_install();
    parser_parse("a{4,a}");
    

    return 0;
}