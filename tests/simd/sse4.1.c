#include "../minunit.h"
#include "../../c/simd/simd.h"
/*#include <stdio.h>*/

MU_TEST(test_i128_zero) {
    i128 zero_vector = i128_zero();
    mu_check(i64x2_extract_h(zero_vector) == 0);
    mu_check(i64x2_extract_l(zero_vector) == 0);
}

MU_TEST(test_i128_make_and_extract) {
    uint64_t lo = 999;
    uint64_t hi = 111;
    i128 b = i64x2_make(hi, lo);

    mu_check(i64x2_extract_h(b) == hi);
    mu_check(i64x2_extract_l(b) == lo);
}

MU_TEST(test_i128_add) {
    uint64_t a_lo = 1;
    uint64_t a_hi = 2;
    i128 a = i64x2_make(a_hi, a_lo);
    uint64_t b_lo = 3;
    uint64_t b_hi = 4;
    i128 b = i64x2_make(b_hi, b_lo);

    i128 c = i64x2_add(a, b);

    mu_check(i64x2_extract_h(c) == a_hi + b_hi);
    mu_check(i64x2_extract_l(c) == a_lo + b_lo);
}

MU_TEST_SUITE(test_suite) {
    MU_RUN_TEST(test_i128_zero);
    MU_RUN_TEST(test_i128_add);
    MU_RUN_TEST(test_i128_make_and_extract);
}

int main(int argc, char *argv[]) {
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}
