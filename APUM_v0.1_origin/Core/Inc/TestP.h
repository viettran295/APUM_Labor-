#ifndef TEST_P_H
#define TEST_P_H

#define EXPAND(X) X
#define SUFFIX(...) EXPAND(SUFFIX_H(__VA_ARGS__, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1))
#define SUFFIX_H(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, XXX, ...) XXX

#define TEST_P(...) HELP_1(SUFFIX(__VA_ARGS__), __VA_ARGS__)
#define HELP_1(...) EXPAND(HELP_2(__VA_ARGS__))
#define HELP_2(S, ...) EXPAND(TEST_P_##S(__VA_ARGS__))

#define TEST_P_16(A, B, ...) A && EXPAND(TEST_P_15(A->B, __VA_ARGS__))
#define TEST_P_15(A, B, ...) A && EXPAND(TEST_P_14(A->B, __VA_ARGS__))
#define TEST_P_14(A, B, ...) A && EXPAND(TEST_P_13(A->B, __VA_ARGS__))
#define TEST_P_13(A, B, ...) A && EXPAND(TEST_P_12(A->B, __VA_ARGS__))
#define TEST_P_12(A, B, ...) A && EXPAND(TEST_P_11(A->B, __VA_ARGS__))
#define TEST_P_11(A, B, ...) A && EXPAND(TEST_P_10(A->B, __VA_ARGS__))
#define TEST_P_10(A, B, ...) A && EXPAND(TEST_P_9(A->B, __VA_ARGS__))
#define TEST_P_9(A, B, ...) A && EXPAND(TEST_P_8(A->B, __VA_ARGS__))
#define TEST_P_8(A, B, ...) A && EXPAND(TEST_P_7(A->B, __VA_ARGS__))
#define TEST_P_7(A, B, ...) A && EXPAND(TEST_P_6(A->B, __VA_ARGS__))
#define TEST_P_6(A, B, ...) A && EXPAND(TEST_P_5(A->B, __VA_ARGS__))
#define TEST_P_5(A, B, ...) A && EXPAND(TEST_P_4(A->B, __VA_ARGS__))
#define TEST_P_4(A, B, ...) A && EXPAND(TEST_P_3(A->B, __VA_ARGS__))
#define TEST_P_3(A, B, ...) A && EXPAND(TEST_P_2(A->B, __VA_ARGS__))
#define TEST_P_2(A, B, ...) A && TEST_P_1(A->B)
#define TEST_P_1(A) A

#endif
