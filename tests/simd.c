#include "./minunit.h"
#include "../c/simd/simd.h"
#include "../c/transpose.h"

// i64s
MU_TEST(test_i64_zero) {
    i64 zero_vector = i64_zero();
    mu_check(i32x2_extract_h(zero_vector) == (uint32_t)0);
    mu_check(i32x2_extract_l(zero_vector) == (uint32_t)0);
}

MU_TEST(test_i64_make_and_extract) {
    uint32_t lo = 111;
    uint32_t hi = 999;
    i64 b = i32x2_make(hi, lo);

    mu_check(i32x2_extract_h(b) == hi);
    mu_check(i32x2_extract_l(b) == lo);
}

// i128s
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

MU_TEST(test_mul_and_transpose) {
    uint64_t prods[8] = {0};
    uint64_t rhi[8] = {0};
    uint64_t rlo[8] = {0};

    // First, do the operations without SIMD to get expected results.
    uint32_t a = 0x12345678;
    uint32_t b[8] = {
        0x11111111, 0x22222222, 0x33333333, 0x44444444,
        0x55555555, 0x66666666, 0x77777777, 0x88888888
    };
    uint32_t transposed_b[8] = { b[0], b[4], b[2], b[6], b[1], b[5], b[3], b[7] };

    for (int i = 0; i < 8; i++) {
        prods[i] = (uint64_t)a * transposed_b[i];
    }

    // Reference implementation
    transpose_into_hi_and_lo(prods, rhi, rlo);

    // Perform multiplications
    i64 aa = i32x2_make(a, a);
    i64 b04 = i32x2_make(b[4], b[0]);
    i64 b26 = i32x2_make(b[6], b[2]);
    i64 b15 = i32x2_make(b[5], b[1]);
    i64 b37 = i32x2_make(b[7], b[3]);
    
    i128 ab04 = i64x2_mul(aa, b04);
    i128 ab26 = i64x2_mul(aa, b26);
    i128 ab15 = i64x2_mul(aa, b15);
    i128 ab37 = i64x2_mul(aa, b37);

    // Transpose the result into higher and lower bits
    i128x2 transposed_ab04ab26 = transpose(ab04, ab26);
    i128 transposed_ab04ab26_lo = transposed_ab04ab26.val[0];
    i128 transposed_ab04ab26_hi = transposed_ab04ab26.val[1];

    // Check the lower bits
    uint64_t transposed_ab04ab26_lo_lo = i64x2_extract_l(transposed_ab04ab26_lo);
    uint64_t transposed_ab04ab26_lo_hi = i64x2_extract_h(transposed_ab04ab26_lo);

    uint32_t ab0_lo = transposed_ab04ab26_lo_lo & 0xFFFFFFFF;
    uint32_t ab2_lo = transposed_ab04ab26_lo_lo >> 32;
    uint32_t ab4_lo = transposed_ab04ab26_lo_hi & 0xFFFFFFFF;
    uint32_t ab6_lo = transposed_ab04ab26_lo_hi >> 32;
    mu_check((uint64_t)ab0_lo == rlo[0]);
    mu_check((uint64_t)ab2_lo == rlo[2]);
    mu_check((uint64_t)ab4_lo == rlo[4]);
    mu_check((uint64_t)ab6_lo == rlo[6]);

    // Check the higher bits
    uint64_t transposed_ab04ab26_hi_lo = i64x2_extract_l(transposed_ab04ab26_hi);
    uint64_t transposed_ab04ab26_hi_hi = i64x2_extract_h(transposed_ab04ab26_hi);

    uint32_t ab0_hi = transposed_ab04ab26_hi_lo & 0xFFFFFFFF;
    uint32_t ab2_hi = transposed_ab04ab26_hi_lo >> 32;
    uint32_t ab4_hi = transposed_ab04ab26_hi_hi & 0xFFFFFFFF;
    uint32_t ab6_hi = transposed_ab04ab26_hi_hi >> 32;
    mu_check((uint64_t)ab0_hi == rhi[0]);
    mu_check((uint64_t)ab2_hi == rhi[2]);
    mu_check((uint64_t)ab4_hi == rhi[4]);
    mu_check((uint64_t)ab6_hi == rhi[6]);

    // Transpose the result into higher and lower bits
    i128x2 transposed_ab15ab37 = transpose(ab15, ab37);
    i128 transposed_ab15ab37_lo = transposed_ab15ab37.val[0];
    i128 transposed_ab15ab37_hi = transposed_ab15ab37.val[1];

    // Check the lower bits
    uint64_t transposed_ab15ab37_lo_lo = i64x2_extract_l(transposed_ab15ab37_lo);
    uint64_t transposed_ab15ab37_lo_hi = i64x2_extract_h(transposed_ab15ab37_lo);

    uint32_t ab1_lo = transposed_ab15ab37_lo_lo & 0xFFFFFFFF;
    uint32_t ab3_lo = transposed_ab15ab37_lo_lo >> 32;
    uint32_t ab5_lo = transposed_ab15ab37_lo_hi & 0xFFFFFFFF;
    uint32_t ab7_lo = transposed_ab15ab37_lo_hi >> 32;

    mu_check((uint64_t)ab1_lo == rlo[1]);
    mu_check((uint64_t)ab3_lo == rlo[3]);
    mu_check((uint64_t)ab5_lo == rlo[5]);
    mu_check((uint64_t)ab7_lo == rlo[7]);

    // Check the higher bits
    uint64_t transposed_ab15ab37_hi_lo = i64x2_extract_l(transposed_ab15ab37_hi);
    uint64_t transposed_ab15ab37_hi_hi = i64x2_extract_h(transposed_ab15ab37_hi);

    uint32_t ab1_hi = transposed_ab15ab37_hi_lo & 0xFFFFFFFF;
    uint32_t ab3_hi = transposed_ab15ab37_hi_lo >> 32;
    uint32_t ab5_hi = transposed_ab15ab37_hi_hi & 0xFFFFFFFF;
    uint32_t ab7_hi = transposed_ab15ab37_hi_hi >> 32;

    mu_check((uint64_t)ab1_hi == rhi[1]);
    mu_check((uint64_t)ab3_hi == rhi[3]);
    mu_check((uint64_t)ab5_hi == rhi[5]);
    mu_check((uint64_t)ab7_hi == rhi[7]);
    // TODO: do an ASCII art diagram
}

MU_TEST(test_transpose) {
    i128x2 orig;
    orig.val[0] = i64x2_make(0x6666666622222222, 0x4444444400000000);
    orig.val[1] = i64x2_make(0x7777777733333333, 0x5555555511111111);

    i128x2 transposed = transpose(orig.val[0], orig.val[1]);

    mu_check(i128_eq(transposed.val[0], i64x2_make(0x3333333322222222, 0x1111111100000000)));

    mu_check(i128_eq(transposed.val[1], i64x2_make(0x7777777766666666, 0x5555555544444444)));
}


MU_TEST(test_i64x2_widening_add) {
    i64 a = i32x2_make(0x66666666, 0x88888888);
    i64 b = i32x2_make(0xffffffff, 0xffffffff);
    i128 r = i64x2_widening_add(a, b);
    i128 expected = i64x2_make(0x188888887, 0x166666665);

    mu_check(i128_eq(r, expected));
}

MU_TEST_SUITE(test_suite) {
    MU_RUN_TEST(test_i64_zero);
    MU_RUN_TEST(test_i64_make_and_extract);
    MU_RUN_TEST(test_i128_zero);
    MU_RUN_TEST(test_i128_add);
    MU_RUN_TEST(test_i128_make_and_extract);
    MU_RUN_TEST(test_transpose);
    MU_RUN_TEST(test_mul_and_transpose);
    MU_RUN_TEST(test_i64x2_widening_add);
}

int main(int argc, char *argv[]) {
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}

