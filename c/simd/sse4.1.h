#include <immintrin.h>
#include <stdio.h>
#include <stdint.h>

typedef __m128d f128;
typedef __m128i i128;

inline f128 f128_zero() { return _mm_setzero_pd(); }
inline i128 i128_zero() { return _mm_setzero_si128(); }

/*
 * Print the binary representation of an i128 (i0i1) as two 64-bit hexadecimal
 * values: "(hi: hi, lo: lo)".
 */
void print_i128(i128 in) {
    unsigned long long v[2];
    _mm_storeu_si128((i128*)v, in);
    printf("(lo: %016llx; hi: %016llx)", v[0], v[1]);
}

/*
 * Returns the i128 value containing the concatenation of hi and lo.
 */
static inline i128 i64x2_make(uint64_t hi, uint64_t lo) {
    // __m128i _mm_set_epi64x (__int64 hi, __int64 lo):
    //  dst[63:0] := lo
    //  dst[127:64] := hi
    return _mm_set_epi64x(hi, lo);
}

/*
 * Returns the lower 64-bit value from an i128.
 */
static inline uint64_t i64x2_extract_l(i128 in) {
    return _mm_cvtsi128_si64(in);
}

/*
 * Returns the higher 64-bit value from an i128.
 */
static inline uint64_t i64x2_extract_h(i128 in) {
    // Requires SSE4.1. A workaround is needed for SSE2.
    return _mm_extract_epi64(in, 1);
}

/*
 * Performs two-lane 64-bit SIMD addition of two i128s.
 */
static inline i128 i64x2_add(i128 a, i128 b) {
    return _mm_add_epi64(a, b);
}

static inline i128 u64x2_mul(i128 a, i128 b) {
    return _mm_mul_epu32(a, b);
}

static inline i128 u64x2_shr(i128 x, int bits) {
    return _mm_srli_epi64(x, bits);
}

static inline i128 i64x2_and(i128 a, i128 b) {
    return _mm_and_si128(a, b);
}

i128 i128_and(i128 a, i128 b) {
    return _mm_and_si128(a, b);
}
