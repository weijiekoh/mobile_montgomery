#include "../minunit.h"
#include <stdio.h>

#include "../../c/bigints/bigint_8x32/bigint.h"
#include "../../c/bigints/bigint_8x32/hex.h"
#include "../../c/acar/mont.h"
/*#include "./test_mont_data.h"*/

MU_TEST(test_mont_mul) {
    printf("Hello\n");
}

MU_TEST_SUITE(test_suite) {
    MU_RUN_TEST(test_mont_mul);
}

int main(int argc, char *argv[]) {
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}
