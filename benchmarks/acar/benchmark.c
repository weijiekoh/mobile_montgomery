#include <stdio.h>
#include <assert.h>
#include "../time.h"
#include "../../c/bigints/bigint_8x32/bigint.h"
#include "../../c/bigints/bigint_8x32/hex.h"
#include "../../c/acar/mont.h"
#include "../data/benchmark_mont_data.h"

BigInt reference_func(
    BigInt *a,
    BigInt *b,
    BigInt *p,
    uint64_t n0,
    int cost
) {
    BigInt x = *a;
    BigInt y = *b;
    BigInt z;

    for (int i = 0; i < cost; i ++) {
        z = mont_mul(&x, &y, p, n0);
        x = y;
        y = z;
    }
    return y;
}

int main(int argc, char *argv[]) {
    const BenchmarkData* data = get_benchmark_data();
    int length = get_benchmark_data_length();

    char* p_hex = "30644e72e131a029b85045b68181585d2833e84879b9709143e1f593f0000001";
    BigInt a, b, c, p, expected;

    int result;

    result = bigint_from_hex(p_hex, &p);
    assert(result == 0);

    int num_runs = 5;
    uint64_t n0 = 4026531839;

    for (int i = 0; i < length; i++) {
        int cost = data[i].cost;
        int result = bigint_from_hex(data[i].a_hex, &a);
        assert(result == 0);
        result = bigint_from_hex(data[i].b_hex, &b);
        assert(result == 0);
        result = bigint_from_hex(data[i].result_hex, &c);
        assert(result == 0);

        double avg = 0;
        for (int i = 0; i < num_runs; i++) {
            double start = get_now_ms();
            expected = reference_func(&a, &b, &p, n0, cost);
            double end = get_now_ms();
            double elapsed = end - start;

            /*printf("elapsed: %f; start: %f; end: %f\n", elapsed, start, end);*/
            avg += elapsed;
        }
        avg /= num_runs;

        printf("%d Mont muls with Acar's CIOS method (non-SIMD, 32-bit limbs) took: %f ms (avg over %d runs)\n", cost, avg, num_runs);

        assert(bigint_eq(&expected, &c));
    }
}
