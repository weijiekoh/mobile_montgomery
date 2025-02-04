#include "../minunit.h"
#include <stdio.h>

#include "../../c/bigints/bigint_8x32/bigint.h"
#include "../../c/bigints/bigint_8x32/hex.h"
#include "../../c/acar/mont.h"
#include "../data/test_mont_data.h"

MU_TEST(test_mont_mul) {
    // For the BN254 scalar field.
    uint64_t n0 = 4026531839;
    char* p_hex = "30644e72e131a029b85045b68181585d2833e84879b9709143e1f593f0000001";

    char** hex_strs = get_mont_test_data();

    BigInt p, ar, br, abr, expected;

    // Convert p_hex to a BigInt
    int result;
    result = bigint_from_hex(p_hex, &p);
    mu_check(result == 0);

    size_t NUM_TESTS = 1024;
        
    // Sample values that work:
    /*char* ar_hex = "0b626d61fa9249f1cdb1ed842fb0ce3683f172e5127d698fdcb3c98cba5a3dcb";*/
    /*char* br_hex = "060746d8f3aa110102f1a1ab3d42df987110b2c030400f4c16da68ed2578bf10";*/
    /*char* c_hex = "11632819f9df31ebfcb1f55ee017a35b6c55b71ed489094efef76714eb7e1236";*/

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

        mu_check(bigint_eq(&abr, &expected));

        char *abr_hex = bigint_to_hex(&abr);
        mu_check(strcmp(abr_hex, c_hex) == 0);
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
