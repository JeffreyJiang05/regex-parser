#include <stdio.h>

#include "debug.h"

#include "parser/error.h"
#include "parser/lexer.h"

#include "parser/ast.h"
#include "parser/parser.h"

int main()
{
    // errlogs_install();
    AST_NODE node = parser_parse("[a-z]]");
    ast_print(node, 0);
    ast_delete(node);
    return 0;
}