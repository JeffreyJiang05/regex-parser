#ifndef MACRO_H
#define MACRO_H

#define EMPTY()

#define PRIMITIVE_CAT(a, b) a ## b
#define CAT(a, b) PRIMITIVE_CAT(a, b)

#define NARGS(...) NARGS_IMPL(__VA_ARGS__, 30, \
    29, 28, 27, 26, 25, 24, 23, 22, 21, 20, \
    19, 18, 17, 16, 15, 14, 13, 12, 11, 10, \
     9,  8,  7,  6,  5,  4,  3,  2,  1,  0)
#define NARGS_IMPL(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
    _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
    _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, \
    N, ...) N

#define EVAL1(...) __VA_ARGS__
#define EVAL2(...) EVAL1(EVAL1(EVAL1(__VA_ARGS__)))
#define EVAL3(...) EVAL2(EVAL2(EVAL2(__VA_ARGS__)))
#define EVAL4(...) EVAL3(EVAL3(EVAL3(__VA_ARGS__)))
#define EVAL(...) EVAL4(__VA_ARGS__)

#define TUPLE_AT_2(x, n, ...) n 
#define CHECK(...) TUPLE_AT_2(__VA_ARGS__, 0)

#define EQUAL_0_0 ,1
#define EQUAL_1_1 ,1
#define EQUAL_2_2 ,1
#define EQUAL_3_3 ,1
#define EQUAL_4_4 ,1
#define EQUAL_5_5 ,1
#define EQUAL_6_6 ,1
#define EQUAL_7_7 ,1
#define EQUAL_8_8 ,1
#define EQUAL_9_9 ,1
#define EQUAL_10_10 ,1

#define EQUALS(a, b) defined(EQUAL ## a ## _ ## b)

#define EQUAL_HELPER(...) TUPLE_AT_2(__VA_ARGS__,0)
#define EQUAL_(x, y) CHECK(EQUAL_##x##_##y)
#define EQUAL(x, y) EQUAL_(x, y)

#endif