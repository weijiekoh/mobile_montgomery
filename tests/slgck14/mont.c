#include "../minunit.h"
#include <stdio.h>

#include "../../c/constants.h"
#include "../../c/bigints/bigint_8x32/bigint.h"
#include "../../c/bigints/bigint_8x32/hex.h"
#include "../../c/slgck14/mont.h"
#include "../data/test_mont_data.h"

MU_TEST(test_mont_mul) {
    // For the BN254 scalar field.
    uint64_t n0 = BN254_SCALAR_N0_8x32;
    char* p_hex = BN254_SCALAR_HEX;

    char** hex_strs = get_mont_test_data();

    BigInt p, ar, br, abr, expected;

    // Convert p_hex to a BigInt
    int result;
    result = bigint_from_hex(p_hex, &p);
    mu_check(result == 0);

    i64 vp40 = i32x2_make(p.v[4], p.v[0]);
    i64 vp62 = i32x2_make(p.v[6], p.v[2]);
    i64 vp51 = i32x2_make(p.v[5], p.v[1]);
    i64 vp73 = i32x2_make(p.v[7], p.v[3]);
    i64 transposed_p[4] = {vp40, vp62, vp51, vp73};

    size_t NUM_TESTS = 1024;

    for (int i = 0; i < NUM_TESTS; i++) {
        char* ar_hex = hex_strs[i * 3];
        char* br_hex = hex_strs[i * 3 + 1];
        char* c_hex = hex_strs[i * 3 + 2];

        result = bigint_from_hex(ar_hex, &ar);
        mu_check(result == 0);
        result = bigint_from_hex(br_hex, &br);
        mu_check(result == 0);
        result = bigint_from_hex(c_hex, &expected);
        mu_check(result == 0);

        i64 vai[NUM_LIMBS];
        for (int i = 0; i < NUM_LIMBS; i++) {
            vai[i] = i32x2_make(ar.v[i], ar.v[i]);
        }

        i64 transposed_b[4];
        transposed_b[0] = i32x2_make(br.v[4], br.v[0]);
        transposed_b[1] = i32x2_make(br.v[6], br.v[2]);
        transposed_b[2] = i32x2_make(br.v[5], br.v[1]);
        transposed_b[3] = i32x2_make(br.v[7], br.v[3]);

        // Perform mont mul
        abr = mont_mul(&vai, &transposed_b, &p, &transposed_p, n0);

        char *abr_hex = bigint_to_hex(&abr);

        /*printf("\nabr_hex:      %s\n", abr_hex);*/
        /*printf("expected_hex: %s\n", c_hex);*/

        mu_check(strcmp(abr_hex, c_hex) == 0);
        mu_check(bigint_eq(&abr, &expected));
    }
}

MU_TEST_SUITE(test_suite) {
    MU_RUN_TEST(test_mont_mul);
}

int main(int argc, char *argv[]) {
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}
