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

        // Perform mont mul
        abr = mont_mul(&ar, &br, &p, n0);

        char *abr_hex = bigint_to_hex(&abr);

        /*printf("\nabr_hex:      %s\n", abr_hex);*/
        /*printf("expected_hex: %s\n", c_hex);*/

        mu_check(strcmp(abr_hex, c_hex) == 0);
        mu_check(bigint_eq(&abr, &expected));
        /*break;*/
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
