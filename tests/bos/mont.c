#include "../minunit.h"
#include <stdio.h>

#include "../../c/bigints/bigint_8x32/bigint.h"
#include "../../c/bigints/bigint_8x32/hex.h"
#include "../../c/bos/mont.h"
/*#include "./test_mont_data.h"*/

MU_TEST(test_mont_mul) {
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


    uint64_t mu = 1;

    abr = mont_mul(&ar, &br, &p, mu);

    char *abr_hex = bigint_to_hex(&abr);

    printf("abr_hex: %s\n", abr_hex);
    printf("expected_hex: %s\n", expected_hex);
    mu_check(strcmp(abr_hex, expected_hex) == 0);
}

MU_TEST_SUITE(test_suite) {
    MU_RUN_TEST(test_mont_mul);
}

int main(int argc, char *argv[]) {
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}
