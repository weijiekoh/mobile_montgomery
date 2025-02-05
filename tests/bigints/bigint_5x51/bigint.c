#include <string.h>

#include "../prns.h"
#include "../../minunit.h"
#include "../../../c/bigints/bigint_5x51/bigint.h"
#include "../../../c/bigints/bigint_5x51/hex.h"

MU_TEST(test_bigint_gt) {
    const char *a_hex_str = "30644e72e131a029b85045b68181585d97816a916871ca8d3c208c16d87cfd47";
    const char *b_hex_str = "20644e72e131a029b85045b68181585d97816a916871ca8d3c208c16d87cfd47";
    BigInt a, b;
    int r = bigint_from_hex(a_hex_str, &a);
    mu_check(r == 0);
    r = bigint_from_hex(b_hex_str, &b);
    mu_check(r == 0);

    bool result = bigint_gt(&a, &b);
    mu_check(result == true);
    result = bigint_gt(&b, &a);
    mu_check(result == false);
}

MU_TEST(test_bigint_sub) {
    const char *a_hex_str = "30644e72e131a029b85045b68181585d97816a916871ca8d3c208c16d87cfd47";
    const char *b_hex_str = "20644e72e131a029b85045b68181585d97816a916871ca8d3c208c16d87cfd47";
    const char *c_hex_str = "1000000000000000000000000000000000000000000000000000000000000000";
    BigInt a, b, c;
    int r = bigint_from_hex(a_hex_str, &a);
    mu_check(r == 0);
    r = bigint_from_hex(b_hex_str, &b);
    mu_check(r == 0);
    r = bigint_from_hex(c_hex_str, &c);
    mu_check(r == 0);

    BigInt result = bigint_sub(&a, &b);

    mu_check(bigint_eq(&result, &c));
}

MU_TEST(test_bigint_from_hex) {
    BigInt number;
    BigInt expected_number = {
        .v = {
            0x8c16d87cfd47ULL,
            0x22d0e3951a784ULL,
            0x60561765e05aaULL,
            0x14dc2822db40ULL,
            0x30644e72e131aULL
        }
    };
    const char *hex_str = "30644e72e131a029b85045b68181585d97816a916871ca8d3c208c16d87cfd47";
    int result = bigint_from_hex(hex_str, &number);

    mu_check(result == 0);
    mu_check(bigint_eq(&number, &expected_number));
}

MU_TEST(test_bigint_to_and_from_hex) {
    // Generate random BigInts, convert them to hex strings, and vice versa
    int num_tests = 256;
    BigInt v = bigint_new();
    BigInt w = bigint_new();
    for (int i = 0; i < num_tests; i++) {
        for (int j = 0; j < NUM_LIMBS; j++) {
            uint64_t rand = prns_at(i * NUM_LIMBS + j);
            rand = rand & LIMB_MASK;
            v.v[j] = rand;
        }
        char* hex_str = bigint_to_hex(&v);
        mu_check(hex_str != NULL);
        bigint_from_hex(hex_str, &w);
        mu_check(bigint_eq(&v, &w));
    }
}

MU_TEST_SUITE(test_suite) {
    MU_RUN_TEST(test_bigint_gt);
    MU_RUN_TEST(test_bigint_sub);
    MU_RUN_TEST(test_bigint_from_hex);
    MU_RUN_TEST(test_bigint_to_and_from_hex);
}

int main(int argc, char *argv[]) {
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}
