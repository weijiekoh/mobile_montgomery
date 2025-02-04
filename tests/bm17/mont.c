#include "../minunit.h"
#include <stdio.h>

#include "../../c/bigints/bigint_8x32/bigint.h"
#include "../../c/bigints/bigint_8x32/hex.h"
#include "../../c/bm17/mont.h"
#include "../data/test_mont_data.h"

MU_TEST(test_mont_mul_bn254_scalar) {
    // mu = p^-1 mod 2^LIMB_BITS
    uint64_t mu = 268435457;

    char* p_hex = "30644e72e131a029b85045b68181585d2833e84879b9709143e1f593f0000001";
    // Sample values that work:
    char* ar_hex = "14a9c2762b8ab0f20cb1096618a19a05d483d5405f405ef524524a41d90fff2f";
    char* br_hex = "0aefa8fa0094edcbcd47dd061763108702bbdc704174a53b54507c8c28c69c77";
    char* expected_hex = "03191bdfb1ecefea0760e45312c3d552e95683d9459749c3b007050dc777e8ac";

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

    abr = mont_mul(&ar, &br, &p, mu);

    char *abr_hex = bigint_to_hex(&abr);

    /*printf("%s\n", ar_hex);*/
    /*printf("%s\n", br_hex);*/
    /*printf("%s\n", abr_hex);*/
    /*printf("expected_hex: %s\n", expected_hex);*/
    mu_check(strcmp(abr_hex, expected_hex) == 0);
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
