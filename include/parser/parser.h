#ifndef PARSER_H
#define PARSER_H

#include "ast.h"

typedef struct parser * PARSER;

PARSER parser_init();

void parser_fini(PARSER parser);

AST_NODE parser_parse(const char *regex);

#endif