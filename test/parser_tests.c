#include <criterion/criterion.h>

#include "parser/parser.h"

TestSuite(parser_tests, .timeout = 5);

Test(parser_tests, single_symbol_0)
{
    int ret;
    void *expected = ast_new(ASTSymbol, 'a');
    void *output = parser_parse("a");
    
    ret = ast_equals(expected, output);
    cr_assert(ret, "Output AST did not match expected!");
    ast_delete(expected);
    ast_delete(output);
}

Test(parser_tests, single_symbol_1)
{
    int ret;
    void *expected = ast_new(ASTSymbol, 'Z');
    void *output = parser_parse("Z");
    
    ret = ast_equals(expected, output);
    cr_assert(ret, "Output AST did not match expected!");
    ast_delete(expected);
    ast_delete(output);
}

Test(parser_tests, single_escaped_symbol_0)
{
    int ret;
    void *expected = ast_new(ASTSymbol, '\x9');
    void *output = parser_parse("\\t");
    
    ret = ast_equals(expected, output);
    cr_assert(ret, "Output AST did not match expected!");
    ast_delete(expected);
    ast_delete(output);
}

Test(parser_tests, single_escaped_symbol_1)
{
    int ret;
    void *expected = ast_new(ASTSymbol, '{');
    void *output = parser_parse("\\{");
    
    ret = ast_equals(expected, output);
    cr_assert(ret, "Output AST did not match expected!");
    ast_delete(expected);
    ast_delete(output);
}

Test(parser_tests, single_escaped_symbol_2)
{
    int ret;
    void *expected = ast_new(ASTSymbol, '?');
    void *output = parser_parse("\\?");
    
    ret = ast_equals(expected, output);
    cr_assert(ret, "Output AST did not match expected!");
    ast_delete(expected);
    ast_delete(output);
}

Test(parser_tests, single_escaped_symbol_3)
{
    int ret;
    void *expected = ast_new(ASTSymbol, '|');
    void *output = parser_parse("\\|");
    
    ret = ast_equals(expected, output);
    cr_assert(ret, "Output AST did not match expected!");
    ast_delete(expected);
    ast_delete(output);
}

Test(parser_tests, character_class_0)
{
    int ret;
    void *expected = ast_new(ASTCharClass, 3,
        ast_new(ASTSymbol, 'a'),
        ast_new(ASTSymbol, 'b'),
        ast_new(ASTSymbol, 'c')
    );
    void *output = parser_parse("[abc]");
    
    ret = ast_equals(expected, output);
    cr_assert(ret, "Output AST did not match expected!");
    ast_delete(expected);
    ast_delete(output);
}

Test(parser_tests, character_class_1)
{
    int ret;
    void *expected = ast_new(ASTCharClass, 2,
        ast_new(ASTCharRange,
            ast_new(ASTSymbol, 'a'),
            ast_new(ASTSymbol, 'z')
        ),
        ast_new(ASTCharRange, 
            ast_new(ASTSymbol, 'A'),
            ast_new(ASTSymbol, 'Z')
        )
    );
    void *output = parser_parse("[a-zA-Z]");
    
    ret = ast_equals(expected, output);
    cr_assert(ret, "Output AST did not match expected!");
    ast_delete(expected);
    ast_delete(output);
}

Test(parser_tests, character_class_2)
{
    int ret;
    void *expected = ast_new(ASTCharClass, 5,
        ast_new(ASTCharRange,
            ast_new(ASTSymbol, 'a'),
            ast_new(ASTSymbol, 'z')
        ),
        ast_new(ASTSymbol, '1'),
        ast_new(ASTSymbol, '2'),
        ast_new(ASTSymbol, '3'),
        ast_new(ASTCharRange, 
            ast_new(ASTSymbol, 'A'),
            ast_new(ASTSymbol, 'Z')
        )
    );
    void *output = parser_parse("[a-z123A-Z]");
    
    ret = ast_equals(expected, output);
    cr_assert(ret, "Output AST did not match expected!");
    ast_delete(expected);
    ast_delete(output);
}

Test(parser_tests, union_simple_0)
{
    int ret;
    void *expected = ast_new(ASTUnion,
        ast_new(ASTSymbol, 'a'),
        ast_new(ASTSymbol, 'b')        
    );
    void *output = parser_parse("a|b");
    
    ret = ast_equals(expected, output);
    cr_assert(ret, "Output AST did not match expected!");
    ast_delete(expected);
    ast_delete(output);
}

// tests precedence
Test(parser_tests, union_simple_1)
{
    int ret;
    void *expected = ast_new(ASTUnion,
        ast_new(ASTUnion,
            ast_new(ASTUnion,
                ast_new(ASTSymbol, 'a'),
                ast_new(ASTSymbol, 'b')
            ),
            ast_new(ASTSymbol, 'c')
        ),
        ast_new(ASTSymbol, 'd')       
    );
    void *output = parser_parse("a|b|c|d");
    
    ret = ast_equals(expected, output);
    cr_assert(ret, "Output AST did not match expected!");
    ast_delete(expected);
    ast_delete(output);
}

Test(parser_tests, concat_simple_0)
{
    int ret;
    void *expected = ast_new(ASTConcat,
        ast_new(ASTSymbol, 'a'),
        ast_new(ASTSymbol, 'b')       
    );
    void *output = parser_parse("ab");
    
    ret = ast_equals(expected, output);
    cr_assert(ret, "Output AST did not match expected!");
    ast_delete(expected);
    ast_delete(output);
}

Test(parser_tests, concat_simple_1)
{
    int ret;
    void *expected = ast_new(ASTConcat,
        ast_new(ASTConcat,
            ast_new(ASTConcat,
                ast_new(ASTSymbol, 'a'),
                ast_new(ASTSymbol, 'b')
            ),
            ast_new(ASTSymbol, 'c')
        ),
        ast_new(ASTSymbol, 'd')       
    );
    void *output = parser_parse("abcd");
    
    ret = ast_equals(expected, output);
    cr_assert(ret, "Output AST did not match expected!");
    ast_delete(expected);
    ast_delete(output);
}

Test(parser_tests, quantifier_simple_0)
{
    int ret;
    void *expected = ast_new(ASTRange, 
        ast_new(ASTSymbol, 'a'), 0, -1
    );
    void *output = parser_parse("a*");
    
    ret = ast_equals(expected, output);
    cr_assert(ret, "Output AST did not match expected!");
    ast_delete(expected);
    ast_delete(output);
}

Test(parser_tests, quantifier_simple_1)
{
    int ret;
    void *expected = ast_new(ASTRange, 
        ast_new(ASTSymbol, 'a'), 0, 1
    );
    void *output = parser_parse("a?");
    
    ret = ast_equals(expected, output);
    cr_assert(ret, "Output AST did not match expected!");
    ast_delete(expected);
    ast_delete(output);
}

Test(parser_tests, quantifier_simple_2)
{
    int ret;
    void *expected = ast_new(ASTRange, 
        ast_new(ASTSymbol, 'a'), 1, -1
    );
    void *output = parser_parse("a+");
    
    ret = ast_equals(expected, output);
    cr_assert(ret, "Output AST did not match expected!");
    ast_delete(expected);
    ast_delete(output);
}

Test(parser_tests, quantifier_simple_3)
{
    int ret;
    void *expected = ast_new(ASTRange, 
        ast_new(ASTSymbol, 'a'), 3, 3
    );
    void *output = parser_parse("a{3}");
    
    ret = ast_equals(expected, output);
    cr_assert(ret, "Output AST did not match expected!");
    ast_delete(expected);
    ast_delete(output);
}

Test(parser_tests, quantifier_simple_4)
{
    int ret;
    void *expected = ast_new(ASTRange, 
        ast_new(ASTSymbol, 'a'), 0, 4
    );
    void *output = parser_parse("a{,4}");
    
    ret = ast_equals(expected, output);
    cr_assert(ret, "Output AST did not match expected!");
    ast_delete(expected);
    ast_delete(output);
}

Test(parser_tests, quantifier_simple_5)
{
    int ret;
    void *expected = ast_new(ASTRange, 
        ast_new(ASTSymbol, 'a'), 12, -1
    );
    void *output = parser_parse("a{12,}");
    
    ret = ast_equals(expected, output);
    cr_assert(ret, "Output AST did not match expected!");
    ast_delete(expected);
    ast_delete(output);
}

Test(parser_tests, quantifier_simple_6)
{
    int ret;
    void *expected = ast_new(ASTRange, 
        ast_new(ASTSymbol, 'a'), 3, 7
    );
    void *output = parser_parse("a{3,7}");
    
    ret = ast_equals(expected, output);
    cr_assert(ret, "Output AST did not match expected!");
    ast_delete(expected);
    ast_delete(output);
}

Test(parser_tests, group_simple_0)
{
    int ret;
    void *expected = ast_new(ASTGroup, 
        ast_new(ASTSymbol, 'a')
    );
    void *output = parser_parse("(a)");
    
    ret = ast_equals(expected, output);
    cr_assert(ret, "Output AST did not match expected!");
    ast_delete(expected);
    ast_delete(output);
}

Test(parser_tests, group_complex_0)
{
    int ret;
    void *expected = ast_new(ASTGroup, 
        ast_new(ASTConcat,
            ast_new(ASTConcat, 
                ast_new(ASTSymbol, 'a'),
                ast_new(ASTSymbol, 'b')
            ),
            ast_new(ASTSymbol, 'c')
        )
    );
    void *output = parser_parse("(abc)");
    
    ret = ast_equals(expected, output);
    cr_assert(ret, "Output AST did not match expected!");
    ast_delete(expected);
    ast_delete(output);
}

Test(parser_tests, group_complex_1)
{
    int ret;
    void *expected = ast_new(ASTGroup, 
        ast_new(ASTUnion,
            ast_new(ASTUnion, 
                ast_new(ASTSymbol, 'a'),
                ast_new(ASTSymbol, 'b')
            ),
            ast_new(ASTSymbol, 'c')
        )
    );
    void *output = parser_parse("(a|b|c)");
    
    ret = ast_equals(expected, output);
    cr_assert(ret, "Output AST did not match expected!");
    ast_delete(expected);
    ast_delete(output);
}

Test(parser_tests, group_complex_2)
{
    int ret;
    void *expected = ast_new(ASTGroup, 
        ast_new(ASTUnion,
            ast_new(ASTConcat, 
                ast_new(ASTSymbol, 'a'),
                ast_new(ASTSymbol, 'b')
            ),
            ast_new(ASTConcat,
                ast_new(ASTSymbol, 'c'),
                ast_new(ASTSymbol, 'd')
            )
        )
    );
    void *output = parser_parse("(ab|cd)");
    
    ret = ast_equals(expected, output);
    cr_assert(ret, "Output AST did not match expected!");
    ast_delete(expected);
    ast_delete(output);
}

Test(parser_tests, group_complex_3)
{
    int ret;
    void *expected = ast_new(ASTUnion, 
        ast_new(ASTGroup, 
            ast_new(ASTConcat, 
                ast_new(ASTSymbol, 'a'),
                ast_new(ASTSymbol, 'b')
            )
        ),
        ast_new(ASTGroup,
            ast_new(ASTConcat,
                ast_new(ASTSymbol, 'c'),
                ast_new(ASTSymbol, 'd')
            )
        )
    );
    void *output = parser_parse("(ab)|(cd)");
    
    ret = ast_equals(expected, output);
    cr_assert(ret, "Output AST did not match expected!");
    ast_delete(expected);
    ast_delete(output);
}

Test(parser_tests, group_complex_4)
{
    int ret;
    void *expected = ast_new(ASTConcat, 
        ast_new(ASTGroup, 
            ast_new(ASTConcat, 
                ast_new(ASTSymbol, 'a'),
                ast_new(ASTSymbol, 'b')
            )
        ),
        ast_new(ASTGroup,
            ast_new(ASTConcat,
                ast_new(ASTSymbol, 'c'),
                ast_new(ASTSymbol, 'd')
            )
        )
    );
    void *output = parser_parse("(ab)(cd)");
    
    ret = ast_equals(expected, output);
    cr_assert(ret, "Output AST did not match expected!");
    ast_delete(expected);
    ast_delete(output);
}

Test(parser_tests, group_complex_5)
{
    int ret;
    void *expected = ast_new(ASTGroup, 
        ast_new(ASTGroup,
            ast_new(ASTGroup,
                ast_new(ASTSymbol, 'a')
            )
        )
    );
    void *output = parser_parse("(((a)))");
    
    ret = ast_equals(expected, output);
    cr_assert(ret, "Output AST did not match expected!");
    ast_delete(expected);
    ast_delete(output);
}

Test(parser_tests, group_complex_6)
{
    int ret;
    void *expected = ast_new(ASTGroup, 
        ast_new(ASTConcat, 
            ast_new(ASTGroup,
                ast_new(ASTConcat,
                    ast_new(ASTGroup, 
                        ast_new(ASTSymbol, 'a')
                    ),
                    ast_new(ASTSymbol, 'b')
                )
            ),
            ast_new(ASTSymbol, 'c')
        )
    );
    void *output = parser_parse("(((a)b)c)");
    
    ret = ast_equals(expected, output);
    cr_assert(ret, "Output AST did not match expected!");
    ast_delete(expected);
    ast_delete(output);
}

Test(parser_tests, group_complex_7)
{
    int ret;
    void *expected = ast_new(ASTGroup, 
        ast_new(ASTConcat, 
            ast_new(ASTConcat,
                ast_new(ASTRange, 
                    ast_new(ASTSymbol, 'a'), 0, -1 
                ),
                ast_new(ASTRange,
                    ast_new(ASTSymbol, 'b'), 0, 1
                )
            ),
            ast_new(ASTRange,
                ast_new(ASTSymbol, 'c'), 1, -1
            )
        )
    );
    void *output = parser_parse("(a*b?c+)");
    
    ret = ast_equals(expected, output);
    cr_assert(ret, "Output AST did not match expected!");
    ast_delete(expected);
    ast_delete(output);
}

Test(parser_tests, group_complex_8)
{
    int ret;
    void *expected = ast_new(ASTGroup, 
        ast_new(ASTConcat, 
            ast_new(ASTConcat,
                ast_new(ASTConcat, 
                    ast_new(ASTRange, 
                        ast_new(ASTSymbol, 'a'), 3, -1
                    ),
                    ast_new(ASTRange, 
                        ast_new(ASTSymbol, 'b'), 0, 7
                    )
                ),
                ast_new(ASTRange,
                    ast_new(ASTSymbol, 'c'), 4, 5
                )
            ),
            ast_new(ASTRange,
                ast_new(ASTSymbol, 'd'), 9, 9
            )
        )
    );
    void *output = parser_parse("(a{3,}b{,7}c{4,5}d{9})");
    
    ret = ast_equals(expected, output);
    cr_assert(ret, "Output AST did not match expected!");
    ast_delete(expected);
    ast_delete(output);
}

Test(parser_tests, concat_union_0)
{
    int ret;
    void *expected = ast_new(ASTUnion, 
        ast_new(ASTUnion,
            ast_new(ASTUnion, 
                ast_new(ASTConcat, 
                    ast_new(ASTSymbol, 'a'),
                    ast_new(ASTSymbol, 'b')
                ),
                ast_new(ASTConcat,
                    ast_new(ASTSymbol, 'c'),
                    ast_new(ASTSymbol, 'd')
                )
            ),
            ast_new(ASTConcat,
                ast_new(ASTSymbol, 'e'),
                ast_new(ASTSymbol, 'f')
            )
        ),
        ast_new(ASTConcat,
            ast_new(ASTSymbol, 'g'),
            ast_new(ASTSymbol, 'h')
        )
    );
    void *output = parser_parse("ab|cd|ef|gh");
    
    ret = ast_equals(expected, output);
    cr_assert(ret, "Output AST did not match expected!");
    ast_delete(expected);
    ast_delete(output);
}

Test(parser_tests, complex_regex_0)
{
    int ret;
    void *expected = ast_new(ASTConcat, 
        ast_new(ASTConcat,
            ast_new(ASTRange, 
                ast_new(ASTGroup,
                    ast_new(ASTConcat,
                        ast_new(ASTSymbol, 'a'),
                        ast_new(ASTSymbol, 'b')
                    )
                ), 0, 1
            ),
            ast_new(ASTSymbol, 'c')
        ),
        ast_new(ASTSymbol, 'd')
    );
    void *output = parser_parse("(ab)?cd");
    
    ret = ast_equals(expected, output);
    cr_assert(ret, "Output AST did not match expected!");
    ast_delete(expected);
    ast_delete(output);
}

Test(parser_tests, decimal_regex)
{
    int ret;
    void *expected = ast_new(ASTConcat, 
        ast_new(ASTConcat, 
            ast_new(ASTGroup, 
                ast_new(ASTUnion, 
                    ast_new(ASTSymbol, '0'),
                    ast_new(ASTConcat, 
                        ast_new(ASTCharClass, 1,
                            ast_new(ASTCharRange, 
                                ast_new(ASTSymbol, '1'),
                                ast_new(ASTSymbol, '9')
                            )
                        ),
                        ast_new(ASTRange, 
                            ast_new(ASTClassSymbol, DIGIT), 0, -1
                        )
                    )
                )
            ), 
            ast_new(ASTSymbol, '.')
        ),
        ast_new(ASTGroup, 
            ast_new(ASTUnion, 
                ast_new(ASTSymbol, '0'),
                ast_new(ASTConcat, 
                    ast_new(ASTRange, 
                        ast_new(ASTClassSymbol, DIGIT), 0, -1
                    ),
                    ast_new(ASTCharClass, 1, 
                        ast_new(ASTCharRange, 
                            ast_new(ASTSymbol, '1'),
                            ast_new(ASTSymbol, '9')
                        )
                    )
                )
            )
        )
    );
    void *output = parser_parse("(0|[1-9]\\d*).(0|\\d*[1-9])");
    
    ret = ast_equals(expected, output);
    cr_assert(ret, "Output AST did not match expected!");
    ast_delete(expected);
    ast_delete(output);
}

Test(parser_tests, phone_number_regex)
{
    int ret;
    void *expected = ast_new(ASTConcat,
        ast_new(ASTConcat,
            ast_new(ASTConcat,
                ast_new(ASTConcat, 
                    ast_new(ASTConcat, 
                        ast_new(ASTRange,
                            ast_new(ASTGroup,
                                ast_new(ASTConcat, 
                                    ast_new(ASTSymbol, '+'),
                                    ast_new(ASTRange, 
                                        ast_new(ASTClassSymbol, DIGIT), 1, 3
                                    )
                                )
                            ), 0, 1
                        ),
                        ast_new(ASTRange, 
                            ast_new(ASTClassSymbol, DIGIT), 3, 3
                        )
                    ),
                    ast_new(ASTSymbol, '-')
                ),
                ast_new(ASTRange,
                    ast_new(ASTClassSymbol, DIGIT), 3, 3
                )
            ),
            ast_new(ASTSymbol, '-')
        ),
        ast_new(ASTRange, 
            ast_new(ASTClassSymbol, DIGIT), 4, 4
        )
    );
    void *output = parser_parse("(\\+\\d{1,3})?\\d{3}-\\d{3}-\\d{4}");
    
    ret = ast_equals(expected, output);
    cr_assert(ret, "Output AST did not match expected!");
    ast_delete(expected);
    ast_delete(output);
}

Test(parser_tests, identifier_regex)
{
    int ret;
    void *expected = ast_new(ASTConcat,
        ast_new(ASTCharClass, 3,
            ast_new(ASTCharRange,
                ast_new(ASTSymbol, 'a'),
                ast_new(ASTSymbol, 'z')
            ),
            ast_new(ASTCharRange,
                ast_new(ASTSymbol, 'A'),
                ast_new(ASTSymbol, 'Z')
            ),
            ast_new(ASTSymbol, '_')
        ),
        ast_new(ASTRange, 
            ast_new(ASTCharClass, 4,
                ast_new(ASTCharRange, 
                    ast_new(ASTSymbol, '0'),
                    ast_new(ASTSymbol, '9')
                ),
                ast_new(ASTCharRange,
                    ast_new(ASTSymbol, 'a'),
                    ast_new(ASTSymbol, 'z')
                ),
                ast_new(ASTCharRange,
                    ast_new(ASTSymbol, 'A'),
                    ast_new(ASTSymbol, 'Z')
                ),
                ast_new(ASTSymbol, '_')
            )
        , 0, -1)
    );
    void *output = parser_parse("[a-zA-Z_][0-9a-zA-Z_]*");
    
    ret = ast_equals(expected, output);
    cr_assert(ret, "Output AST did not match expected!");
    ast_delete(expected);
    ast_delete(output);
}

Test(parser_tests, parse_err_empty_regex)
{
    void *output = parser_parse("");
    cr_assert(output == NULL, "Expected output AST to be null. Got %p instead.", output);
}

Test(parser_tests, parse_err_class_symbol_in_class)
{
    void *output = parser_parse("[a-z\\d]");
    cr_assert(output == NULL, "Expected output AST to be null. Got %p instead.", output);
}

Test(parser_tests, parse_err_invalid_char_range_0)
{
    void *output = parser_parse("[a-zA-]");
    cr_assert(output == NULL, "Expected output AST to be null. Got %p instead.", output);
}

Test(parser_tests, parse_err_invalid_char_range_1)
{
    void *output = parser_parse("[a-zA-)]");
    cr_assert(output == NULL, "Expected output AST to be null. Got %p instead.", output);
}

Test(parser_tests, parse_err_empty_char_range)
{
    void *output = parser_parse("[]");
    cr_assert(output == NULL, "Expected output AST to be null. Got %p instead.", output);
}

Test(parser_tests, parse_err_empty_group)
{
    void *output = parser_parse("()");
    cr_assert(output == NULL, "Expected output AST to be null. Got %p instead.", output);
}

Test(parser_tests, parse_err_empty_quantifier)
{
    void *output = parser_parse("a{}");
    cr_assert(output == NULL, "Expected output AST to be null. Got %p instead.", output);
}

Test(parser_tests, parse_err_unexpected_token_0)
{
    void *output = parser_parse("a??");
    cr_assert(output == NULL, "Expected output AST to be null. Got %p instead.", output);
}

Test(parser_tests, parse_err_unexpected_token_1)
{
    void *output = parser_parse("b{}}");
    cr_assert(output == NULL, "Expected output AST to be null. Got %p instead.", output);
}

Test(parser_tests, parse_err_unexpected_token_2)
{
    void *output = parser_parse("b{3,,}");
    cr_assert(output == NULL, "Expected output AST to be null. Got %p instead.", output);
}

Test(parser_tests, parse_err_unexpected_token_3)
{
    void *output = parser_parse("[a--z]");
    cr_assert(output == NULL, "Expected output AST to be null. Got %p instead.", output);
}

Test(parser_tests, parse_err_unexpected_token_4)
{
    void *output = parser_parse("a{a,a}");
    cr_assert(output == NULL, "Expected output AST to be null. Got %p instead.", output);
}

Test(parser_tests, parse_err_unexpected_token_5)
{
    void *output = parser_parse("a||b");
    cr_assert(output == NULL, "Expected output AST to be null. Got %p instead.", output);
}

Test(parser_tests, parse_err_unexpected_token_6)
{
    void *output = parser_parse("[a-z]]");
    cr_assert(output == NULL, "Expected output AST to be null. Got %p instead.", output);
}

Test(parser_tests, parse_err_unexpected_token_7)
{
    void *output = parser_parse("{}");
    cr_assert(output == NULL, "Expected output AST to be null. Got %p instead.", output);
}


// TODO: ADD SEMANTIC TESTS