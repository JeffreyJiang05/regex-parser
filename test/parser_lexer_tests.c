#include <criterion/criterion.h>

#include "parser/lexer.h"

// X MACROS FOR DEFINING TESTS
//      X(REGEX_INPUT, EXPECTED_OUTPUT)

#define BUILD_TEST()                                                                                    \
do                                                                                                      \
{                                                                                                       \
    const char *input = REGEX_INPUT;                                                                    \
    const char *expected[] = { EXPECTED_OUTPUT };                                                       \
    size_t expected_sz = sizeof(expected) / sizeof(const char *);                                       \
    const char **output = calloc(expected_sz, sizeof(const char *));                                    \
    size_t output_sz = 0;                                                                               \
    LEXER lexer = lex_init(input);                                                                      \
    while (lex_status(lexer) >= 0)                                                                      \
    {                                                                                                   \
        TOKEN tok = lex_peek_token(lexer);                                                              \
        if (output_sz < expected_sz)                                                                    \
        {                                                                                               \
            output[output_sz] = token_get_text(tok);                                                    \
        }                                                                                               \
        output_sz++;                                                                                    \
        lex_consume_token(lexer);                                                                       \
        if (token_get_type(tok) == END) break;                                                          \
    }                                                                                                   \
    cr_assert(output_sz == expected_sz, "Expected size %lu. Got %lu", expected_sz, output_sz);          \
    for (size_t i = 0; i < expected_sz; ++i)                                                            \
    {                                                                                                   \
        cr_assert_str_eq(output[i], expected[i], "Expected for token #%lu to be \"%s\". Got \"%s\"",    \
            i, expected[i], output[i]);                                                                 \
    }                                                                                                   \
    free(output);                                                                                       \
} while (0)

Test(lexer_tests, simple_regex_0, .timeout = 5)
{
#define REGEX_INPUT "abc"
#define EXPECTED_OUTPUT "Symbol[a]", "Symbol[b]", "Symbol[c]", "END"
BUILD_TEST();
#undef REGEX_INPUT
#undef EXPECTED_OUTPUT
}

Test(lexer_tests, simple_regex_1, .timeout = 5)
{
#define REGEX_INPUT ""
#define EXPECTED_OUTPUT "END"
BUILD_TEST();
#undef REGEX_INPUT
#undef EXPECTED_OUTPUT
}

Test(lexer_tests, simple_regex_with_escapes_0, .timeout = 5)
{
#define REGEX_INPUT "\\t\\n\\\\"
#define EXPECTED_OUTPUT "EscSymbol[TAB]", "EscSymbol[NEWLINE]", "EscSymbol[\\]", "END"
BUILD_TEST();
#undef REGEX_INPUT
#undef EXPECTED_OUTPUT
}

Test(lexer_tests, simple_regex_2, .timeout = 5)
{
#define REGEX_INPUT "ZA\\tB,"
#define EXPECTED_OUTPUT "Symbol[Z]", "Symbol[A]", "EscSymbol[TAB]", "Symbol[B]", "Symbol[,]", "END"
BUILD_TEST();
#undef REGEX_INPUT
#undef EXPECTED_OUTPUT
}

Test(lexer_tests, simple_regex_3, .timeout = 5)
{
#define REGEX_INPUT "A\\tB,45"
#define EXPECTED_OUTPUT "Symbol[A]", "EscSymbol[TAB]", "Symbol[B]", "Symbol[,]", "Symbol[4]", "Symbol[5]", "END"
BUILD_TEST();
#undef REGEX_INPUT
#undef EXPECTED_OUTPUT
}

Test(lexer_tests, simple_regex_with_escapes_1, .timeout = 5)
{
#define REGEX_INPUT "\\\\\\t\\n\\[\\]\\(\\)\\{\\}"
#define EXPECTED_OUTPUT "EscSymbol[\\]", "EscSymbol[TAB]", "EscSymbol[NEWLINE]", "EscSymbol['[']", "EscSymbol[']']", \
                        "EscSymbol[(]", "EscSymbol[)]", "EscSymbol[{]", "EscSymbol[}]", "END"
BUILD_TEST();
#undef REGEX_INPUT
#undef EXPECTED_OUTPUT
}

Test(lexer_tests, simple_regex_with_escapes_2, .timeout = 5)
{
#define REGEX_INPUT "\\*\\+\\?\\|"
#define EXPECTED_OUTPUT "EscSymbol[*]", "EscSymbol[+]", "EscSymbol[?]", "EscSymbol[|]", "END"
BUILD_TEST();
#undef REGEX_INPUT
#undef EXPECTED_OUTPUT
}

Test(lexer_tests, regex_with_minus_token_0, .timeout = 5)
{
#define REGEX_INPUT "A-Za-z"
#define EXPECTED_OUTPUT "Symbol[A]", "Symbol[-]", "Symbol[Z]", "Symbol[a]", "Symbol[-]", "Symbol[z]", "END"
BUILD_TEST();
#undef REGEX_INPUT
#undef EXPECTED_OUTPUT
}

Test(lexer_tests, regex_with_group_0, .timeout = 5)
{
#define REGEX_INPUT "A-Z((a))"
#define EXPECTED_OUTPUT "Symbol[A]", "Symbol[-]", "Symbol[Z]", "LParen", "LParen", "Symbol[a]", "RParen", "RParen", "END"
BUILD_TEST();
#undef REGEX_INPUT
#undef EXPECTED_OUTPUT   
}

Test(lexer_tests, regex_with_group_1, .timeout = 5)
{
#define REGEX_INPUT "A-Z)("
#define EXPECTED_OUTPUT "Symbol[A]", "Symbol[-]", "Symbol[Z]", "RParen", "LParen", "END"
BUILD_TEST();
#undef REGEX_INPUT
#undef EXPECTED_OUTPUT   
}

Test(lexer_tests, regex_with_group_2, .timeout = 5)
{
#define REGEX_INPUT "())())(("
#define EXPECTED_OUTPUT "LParen", "RParen", "RParen", "LParen", "RParen", "RParen", "LParen", "LParen", "END"
BUILD_TEST();
#undef REGEX_INPUT
#undef EXPECTED_OUTPUT  
}

Test(lexer_tests, regex_with_character_class_0, .timeout = 5)
{
#define REGEX_INPUT "[abc]"
#define EXPECTED_OUTPUT "LBracket", "Symbol[a]", "Symbol[b]", "Symbol[c]", "RBracket", "END"
BUILD_TEST();
#undef REGEX_INPUT
#undef EXPECTED_OUTPUT  
}

Test(lexer_tests, regex_with_character_class_1, .timeout = 5)
{
#define REGEX_INPUT "[abc[d]]"
#define EXPECTED_OUTPUT "LBracket", "Symbol[a]", "Symbol[b]", "Symbol[c]", "LBracket", "Symbol[d]", "RBracket", \
                        "RBracket", "END"
BUILD_TEST();
#undef REGEX_INPUT
#undef EXPECTED_OUTPUT  
}

Test(lexer_tests, regex_with_character_class_2, .timeout = 5)
{
#define REGEX_INPUT "][[[]]"
#define EXPECTED_OUTPUT "RBracket", "LBracket", "LBracket", "LBracket", "RBracket", "RBracket" , "END"
BUILD_TEST();
#undef REGEX_INPUT
#undef EXPECTED_OUTPUT     
}

Test(lexer_tests, regex_with_character_range_0, .timeout = 5)
{
#define REGEX_INPUT "a{1,4}"
#define EXPECTED_OUTPUT "Symbol[a]", "LBrace", "Number[1]", "Comma", "Number[4]", "RBrace" , "END"
BUILD_TEST();
#undef REGEX_INPUT
#undef EXPECTED_OUTPUT  
}

Test(lexer_tests, regex_with_character_range_1, .timeout = 5)
{
#define REGEX_INPUT "{}"
#define EXPECTED_OUTPUT "LBrace", "RBrace" , "END"
BUILD_TEST();
#undef REGEX_INPUT
#undef EXPECTED_OUTPUT    
}

Test(lexer_tests, regex_with_character_range_2, .timeout = 5)
{
#define REGEX_INPUT "{12,544,2}12"
#define EXPECTED_OUTPUT "LBrace", "Number[12]", "Comma", "Number[544]", "Comma", "Number[2]", "RBrace", "Symbol[1]", \
                        "Symbol[2]", "END"
BUILD_TEST();
#undef REGEX_INPUT
#undef EXPECTED_OUTPUT    
}

Test(lexer_tests, regex_with_character_range_3, .timeout = 5)
{
#define REGEX_INPUT "{12a,4}"
#define EXPECTED_OUTPUT "LBrace", "Number[12]", "Symbol[a]", "Symbol[,]", "Symbol[4]", "RBrace", "END"
BUILD_TEST();
#undef REGEX_INPUT
#undef EXPECTED_OUTPUT    
}

Test(lexer_tests, regex_with_character_range_4, .timeout = 5)
{
#define REGEX_INPUT "c{124,}"
#define EXPECTED_OUTPUT "Symbol[c]", "LBrace", "Number[124]", "Comma", "RBrace", "END"
BUILD_TEST();
#undef REGEX_INPUT
#undef EXPECTED_OUTPUT    
}

Test(lexer_tests, regex_with_character_range_5, .timeout = 5)
{
#define REGEX_INPUT "a{,4}b{3,}"
#define EXPECTED_OUTPUT "Symbol[a]", "LBrace", "Comma", "Number[4]", "RBrace", "Symbol[b]", "LBrace", "Number[3]", "Comma", "RBrace", "END"
BUILD_TEST();
#undef REGEX_INPUT
#undef EXPECTED_OUTPUT    
}

Test(lexer_tests, general_regex_0, .timeout = 5)
{
#define REGEX_INPUT "(a|b)*abb"
#define EXPECTED_OUTPUT "LParen", "Symbol[a]", "Union", "Symbol[b]", "RParen", "Asterisk", "Symbol[a]", "Symbol[b]", "Symbol[b]", "END"
BUILD_TEST();
#undef REGEX_INPUT
#undef EXPECTED_OUTPUT  
}

Test(lexer_tests, general_regex_1, .timeout = 5)
{
#define REGEX_INPUT "[a-z_A-Z][a-zA-Z0-9_]*"
#define EXPECTED_OUTPUT "LBracket", "Symbol[a]", "Minus", "Symbol[z]", "Symbol[_]", "Symbol[A]", "Minus", "Symbol[Z]", "RBracket", "LBracket", \
                        "Symbol[a]", "Minus", "Symbol[z]", "Symbol[A]", "Minus", "Symbol[Z]", "Symbol[0]", "Minus", "Symbol[9]", "Symbol[_]", \
                        "RBracket", "Asterisk", "END"
BUILD_TEST();
#undef REGEX_INPUT
#undef EXPECTED_OUTPUT  
}

Test(lexer_tests, general_regex_2, .timeout = 5)
{
#define REGEX_INPUT "\\d*.\\d*"
#define EXPECTED_OUTPUT "DigitClass", "Asterisk", "Symbol[.]", "DigitClass", "Asterisk", "END"
BUILD_TEST();
#undef REGEX_INPUT
#undef EXPECTED_OUTPUT  
}

Test(lexer_tests, general_regex_3, .timeout = 5)
{
#define REGEX_INPUT "1(0|1){0,}|0"
#define EXPECTED_OUTPUT "Symbol[1]", "LParen", "Symbol[0]", "Union", "Symbol[1]", "RParen", "LBrace", "Number[0]", "Comma", "RBrace", \
                        "Union", "Symbol[0]", "END"
BUILD_TEST();
#undef REGEX_INPUT
#undef EXPECTED_OUTPUT  
}

Test(lexer_tests, general_regex_4, .timeout = 5)
{
#define REGEX_INPUT "(aa)*|b(bb)*";
#define EXPECTED_OUTPUT "LParen", "Symbol[a]", "Symbol[a]", "RParen", "Asterisk", "Union", "Symbol[b]", "LParen", "Symbol[b]", "Symbol[b]", \
                        "RParen", "Asterisk", "END"
BUILD_TEST();
#undef REGEX_INPUT
#undef EXPECTED_OUTPUT  
}