#include <stdio.h>

#include "debug.h"

#include "parser/error.h"
#include "parser/lexer.h"

#include "parser/ast.h"

int main()
{
    void *ast = ast_new(ASTConcat,
        ast_new(ASTConcat, 
            ast_new(ASTSymbol, 'a'),
            ast_new(ASTSymbol, 'b')
        ),
        ast_new(ASTSymbol, 'c')
    );

    ast_print(ast, 0);
    ast_delete(ast);

    return 0;
}