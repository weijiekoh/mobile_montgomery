#include <stdio.h>
#include <assert.h>
#include "../time.h"
#include "../black_box.h"
#include "../../c/constants.h"
#include "../../c/bigints/bigint_8x32/bigint.h"
#include "../../c/bigints/bigint_8x32/hex.h"
#include "../../c/slgck14/mont.h"
#include "../data/benchmark_mont_data.h"

// This benchmarking approach needs to change. It's a little hacky, and ideally
// we should use something like the Rust Criterion library.

DO_OPT // Allow optimisations for this function
__attribute__((noinline))
void optimised_mont_mul_no_reduce(
    BigInt *ar,
    BigInt *br,
    BigInt *p,
    uint64_t n0,
    uint64_t *t
) {
    mont_mul_no_reduce(ar, br, p, n0, t);
}

// Unoptimised function to run the Montgomery multiplication without reduction `cost` times
NO_OPT
uint64_t reference_func(
    BigInt *a,
    BigInt *b,
    BigInt *p,
    uint64_t n0,
    int cost
) {
    BigInt x = *a;
    BigInt y = *b;
    uint64_t t[NUM_LIMBS + 1] = {0};

    for (int i = 0; i < cost; i ++) {
        optimised_mont_mul_no_reduce(&x, &y, p, n0, t);
    }
    return black_box(t[0]);
}

int main(int argc, char *argv[]) {
    const BenchmarkData* data = get_benchmark_data();
    int length = get_benchmark_data_length();

    uint64_t n0 = BN254_SCALAR_N0_8x32;
    char* p_hex = BN254_SCALAR_HEX;

    BigInt a, b, c, p;

    int result;

    result = bigint_from_hex(p_hex, &p);
    assert(result == 0);

    i64 vp40 = i32x2_make(p.v[4], p.v[0]);
    i64 vp62 = i32x2_make(p.v[6], p.v[2]);
    i64 vp51 = i32x2_make(p.v[5], p.v[1]);
    i64 vp73 = i32x2_make(p.v[7], p.v[3]);
    i64 transposed_p[4] = {vp40, vp62, vp51, vp73};

    int num_runs = 5;

    for (int i = length - 1; i < length; i++) {
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
            reference_func(&a, &b, &p, n0, cost);
            double end = get_now_ms();
            double elapsed = end - start;
            avg += elapsed;
        }
        avg /= num_runs;

        printf("%d Mont muls with SLGCK's CIOS method (SIMD) took: %f ms (avg over %d runs)\n", cost, avg, num_runs);
    }
}
