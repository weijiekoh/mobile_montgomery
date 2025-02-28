#include "../minunit.h"
#include <stdio.h>

#include "../../c/constants.h"
#include "../../c/bigints/bigint_8x32/bigint.h"
#include "../../c/bigints/bigint_8x32/hex.h"
#include "../../c/bm17/mont.h"
#include "../data/test_mont_data.h"

MU_TEST(test_mont_mul_bn254_scalar) {
    // mu = p^-1 mod 2^LIMB_BITS
    char* p_hex = BN254_SCALAR_HEX;
    uint64_t mu = BN254_SCALAR_BM17_MU_4x64;

    char** hex_strs = get_mont_test_data();

    BigInt p, ar, br, abr, expected;

    size_t NUM_TESTS = 1024;

    int result;

    for (int i = 0; i < NUM_TESTS; i++) {
        char* ar_hex = hex_strs[i * 3];
        char* br_hex = hex_strs[i * 3 + 1];
        char* c_hex = hex_strs[i * 3 + 2];

        result = bigint_from_hex(p_hex, &p);
        mu_check(result == 0);
        result = bigint_from_hex(ar_hex, &ar);
        mu_check(result == 0);
        result = bigint_from_hex(br_hex, &br);
        mu_check(result == 0);
        result = bigint_from_hex(c_hex, &expected);
        mu_check(result == 0);

        abr = mont_mul(&ar, &br, &p, mu);

        char *abr_hex = bigint_to_hex(&abr);

        mu_check(bigint_eq(&abr, &expected));
        mu_check(strcmp(abr_hex, c_hex) == 0);
    }
}

MU_TEST(test_mont_mul_bls12_377_scalar) {
    char* p_hex = "12ab655e9a2ca55660b44d1e5c37b00159aa76fed00000010a11800000000001";
    char* ar_hex = "0b626d61fa9249f1cdb1ed842fb0ce3683f172e5127d698fdcb3c98cba5a3dcb";
    char* br_hex = "060746d8f3aa110102f1a1ab3d42df987110b2c030400f4c16da68ed2578bf10";
    char* expected_hex = "11632819f9df31ebfcb1f55ee017a35b6c55b71ed489094efef76714eb7e1236";
    BigInt p, ar, br, abr, expected;

    // Convert p_hex to a BigInt
    int result;
    result = bigint_from_hex(p_hex, &p);
    mu_check(result == 0);
    result = bigint_from_hex(ar_hex, &ar);
    mu_check(result == 0);
    result = bigint_from_hex(br_hex, &br);
    mu_check(result == 0);
    result = bigint_from_hex(expected_hex, &expected);
    mu_check(result == 0);

    // mu = p^-1 mod 2^LIMB_BITS
    uint64_t mu = 1;

    abr = mont_mul(&ar, &br, &p, mu);

    char *abr_hex = bigint_to_hex(&abr);

    mu_check(bigint_eq(&abr, &expected));

    mu_check(strcmp(abr_hex, expected_hex) == 0);
}

MU_TEST_SUITE(test_suite) {
    MU_RUN_TEST(test_mont_mul_bn254_scalar);
    MU_RUN_TEST(test_mont_mul_bls12_377_scalar);
}

int main(int argc, char *argv[]) {
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}
