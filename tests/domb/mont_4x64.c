#include "../minunit.h"
#include <stdio.h>

#include "../../c/constants.h"
#include "../../c/bigints/bigint_4x64/bigint.h"
#include "../../c/bigints/bigint_4x64/hex.h"
#include "../../c/domb/mont_4x64.h"
#include "../data/test_mont_data.h"

MU_TEST(test_mont_mul) {
    // For the BN254 scalar field.
    uint64_t n0 = BN254_SCALAR_N0_4x64;
    char* p_hex = BN254_SCALAR_HEX;

    char** hex_strs = get_mont_test_data();

    BigInt p, ar, br, expected;

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

        uint64_t t[NUM_LIMBS] = {0};

        // Perform mont mul
        mont_mul_no_reduce(&ar, &br, &p, n0, t);

        printf("\n");
        for (int i = 0; i < NUM_LIMBS; i++) {
            printf("%016lx\n", t[i]);
        }

        /*char *abr_hex = bigint_to_hex(&abr);*/

        /*mu_check(strcmp(abr_hex, c_hex) == 0);*/
        /*mu_check(bigint_eq(&abr, &expected));*/
        break;
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
