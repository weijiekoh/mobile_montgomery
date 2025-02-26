#include <immintrin.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// All SIMD vector types defined here store their values in little endian format.
// e.g. a 64-bit i64 stores 0x77771111 has 0x7777 as the higher 32 bits, and
// 0x1111 as the lower 32 bits.

// 32x2 64-bit SIMD vector
typedef __m64 i64;

// 64x2 128-bit SIMD vector
typedef __m128i i128;

typedef __m128i i32x4;

// A 32x4x2 SIMD vector
typedef struct {
    i128 val[2];
} i128x2;

// Returns zero values
static inline i64 i64_zero() { return _mm_setzero_si64(); }
static inline i128 i128_zero() { return _mm_setzero_si128(); }

/*
 * Returns the i64 value containing the concatenation of hi and lo.
 */
static inline i64 i32x2_make(uint32_t hi, uint32_t lo) {
    return _mm_set_pi32(hi, lo);
}

static inline i64 i32x2_splat(uint32_t x) {
    return i32x2_make(x, x);
}

/*
 * Returns the lower 32-bit value from an i64.
 */
static inline uint32_t i32x2_extract_l(i64 in) {
    return (uint32_t)_mm_cvtsi64_si32(in);
}

/*
 * Returns the higher 32-bit value from an i64.
 */
static inline uint32_t i32x2_extract_h(i64 in) {
    __m64 tmp = _mm_srli_si64(in, 32);
    return (uint32_t)_mm_cvtsi64_si32(tmp);
}

/*
 * Print the binary representation of an i64 (i0i1) as two 64-bit hexadecimal
 * values: "(hi: hi, lo: lo)".
 */
void print_i64(i64 in) {
    uint32_t hi = i32x2_extract_h(in);
    uint32_t lo = i32x2_extract_l(in);
    printf("(lo: %08x; hi: %08x)", lo, hi);
}

/*
 * Performs two-lane 32-bit SIMD addition of two i64s.
 */
static inline i64 i32x2_add(i64 a, i64 b) {
    return _mm_add_pi32(a, b);
}

/*
 * Returns the bitwise AND of a and b.
 */
static inline i64 i32x2_and(i64 a, i64 b) {
    return _mm_and_si64(a, b);
}

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
 * According to the definition of _mm_set_epi64x(__int64 e1, __int64 e0),
 * e1 is stored at dst[127:64], and e0 is stored at dst[63:0].
 */
static inline i128 i64x2_make(uint64_t hi, uint64_t lo) {
    return _mm_set_epi64x(hi, lo);
}

static inline i128 i64x2_mul(i64 a, i64 b) {
    uint64_t a_lo = i32x2_extract_l(a);
    uint64_t a_hi = i32x2_extract_h(a);
    uint64_t b_lo = i32x2_extract_l(b);
    uint64_t b_hi = i32x2_extract_h(b);
    uint64_t prod_lo = a_lo * b_lo;
    uint64_t prod_hi = a_hi * b_hi;
    return i64x2_make(prod_hi, prod_lo);
}

static inline i128 i64x2_widening_add(i64 a, i64 b) {
    // Extract the two 32-bit lanes from each input
    uint32_t a_lo = i32x2_extract_l(a);
    uint32_t a_hi = i32x2_extract_h(a);
    uint32_t b_lo = i32x2_extract_l(b);
    uint32_t b_hi = i32x2_extract_h(b);

    // Widen each 32-bit lane to 64 bits and add corresponding lanes
    uint64_t sum_lo = (uint64_t)a_lo + (uint64_t)b_lo;
    uint64_t sum_hi = (uint64_t)a_hi + (uint64_t)b_hi;

    // Pack the 64-bit results into a 128-bit SIMD vector:
    // The first parameter is for the high lane and the second for the low lane.
    return i64x2_make(sum_lo, sum_hi);
}

/*
 * Returns the lower (e0) 64-bit value from an i128.
 */
static inline uint64_t i64x2_extract_l(i128 in) {
    return _mm_cvtsi128_si64(in);
}

/*
 * Returns the higher (e1) 64-bit value from an i128.
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

static inline i128 u64x2_shr(i128 x, int bits) {
    return _mm_srli_epi64(x, bits);
}

static inline i128 i128_and(i128 a, i128 b) {
    return _mm_and_si128(a, b);
}

/*
 * Emulates the NEON instruction vmal_s32, which computes:
 * Va[i] + Vb[i] * Vc[i] for i in [0, 1]
 */
i128 madd(i128 a, i64 b, i64 c) {
    uint32_t b_lo = i32x2_extract_l(b);
    uint32_t b_hi = i32x2_extract_h(b);
    uint32_t c_lo = i32x2_extract_l(c);
    uint32_t c_hi = i32x2_extract_h(c);

    uint64_t prod_lo = (uint64_t)b_lo * (uint64_t)c_lo;
    uint64_t prod_hi = (uint64_t)b_hi * (uint64_t)c_hi;

    uint64_t a_lo = i64x2_extract_l(a);
    uint64_t a_hi = i64x2_extract_h(a);

    uint64_t res_lo = a_lo + prod_lo;
    uint64_t res_hi = a_hi + prod_hi;

    return i64x2_make(res_hi, res_lo);
}

// Floating-point SIMD
/*
 * Print the binary representation of an f128 (f0f1) as such:
 * "(f0: <sign> <exp minus 1023 in decimal> <mantissa in hex>, f1: <sign> <exp
 * minus 1023 in decimal> <mantissa in hex>)"
 */
void print_f128(i128 in) {
    unsigned long long f0, f1;
    uint8_t sign0, sign1;
    uint32_t exp0, exp1;
    uint64_t mantissa0, mantissa1;

    f0 = i64x2_extract_l(in);
    f1 = i64x2_extract_h(in);

    sign0 = f0 >> 63;
    sign1 = f1 >> 63;

    uint64_t mantissa_mask = 0xfffffffffffff;
    exp0 = ((f0 >> 52) & 0x7ff) - 1023;
    exp1 = ((f1 >> 52) & 0x7ff) - 1023;

    mantissa0 = f0 & mantissa_mask;
    mantissa1 = f1 & mantissa_mask;

    char sign0_char, sign1_char;

    sign0_char = sign0 == 0 ? '+' : '-';
    sign1_char = sign1 == 0 ? '+' : '-';

    printf(
        "(%c %d %013lx, %c %d %013lx)",
        sign0_char, exp0, mantissa0, sign1_char, exp1, mantissa1
    );
}

static inline i128x2 transpose(i128 a, i128 b) {
    uint64_t a0 = i64x2_extract_l(a);
    uint64_t a1 = i64x2_extract_h(a);

    uint32_t a00 = (uint32_t)a0;
    uint32_t a01 = (uint32_t)(a0 >> 32);
    uint32_t a10 = (uint32_t)a1;
    uint32_t a11 = (uint32_t)(a1 >> 32);

    uint64_t b0 = i64x2_extract_l(b);
    uint64_t b1 = i64x2_extract_h(b);

    uint32_t b00 = (uint32_t)b0;
    uint32_t b01 = (uint32_t)(b0 >> 32);
    uint32_t b10 = (uint32_t)b1;
    uint32_t b11 = (uint32_t)(b1 >> 32);

    uint64_t c0 = (uint64_t)i32x2_make(b10, a10);
    uint64_t c1 = (uint64_t)i32x2_make(b00, a00);
    uint64_t d0 = (uint64_t)i32x2_make(b11, a11);
    uint64_t d1 = (uint64_t)i32x2_make(b01, a01);

    i128 c = i64x2_make(c0, c1);
    i128 d = i64x2_make(d0, d1);

    i128x2 res;
    res.val[0] = c;
    res.val[1] = d;
    return res;
}

static inline bool i128_eq(i128 a, i128 b) {
    return (__int128) a == (__int128) b;
}

static inline i128 trn1(i32x4 a, i32x4 b) {
    __m128i t1 = _mm_unpacklo_epi32(a, b); // t1 = [a0, b0, a1, b1]
    __m128i t2 = _mm_unpackhi_epi32(a, b); // t2 = [a2, b2, a3, b3]
    // Combine the lower 64 bits from t1 and t2 to get [a0, b0, a2, b2]
    return _mm_unpacklo_epi64(t1, t2);
}

static inline i128 trn2(i32x4 a, i32x4 b) {
    __m128i t1 = _mm_unpacklo_epi32(a, b); // t1 = [a0, b0, a1, b1]
    __m128i t2 = _mm_unpackhi_epi32(a, b); // t2 = [a2, b2, a3, b3]
    // Combine the higher 64 bits from t1 and t2 to get [a1, b1, a3, b3]
    return _mm_unpackhi_epi64(t1, t2);
}

// Concatenates a and b and extracts a vector starting at the (i*4)-th byte.
// For example, if i==1, result = [a1, a2, a3, b0].
static inline i128 extq(i32x4 a, i32x4 b, int i) {
    uint32_t lane0 = _mm_extract_epi32(a, 1);  // a[1]
    uint32_t lane1 = _mm_extract_epi32(b, 0);  // b[0]
    uint32_t lane2 = _mm_extract_epi32(a, 3);  // a[3]
    uint32_t lane3 = _mm_extract_epi32(a, 0);  // a[0]
    return _mm_setr_epi32(lane0, lane1, lane2, lane3);
}

static inline i32x4 i32x4_zero() {
    return _mm_setzero_si128();
}

static inline uint32_t i32x4_extract_0(i32x4 a) {
    uint64_t hi = i64x2_extract_h(a);
    return hi & 0xffffffff;
}

static inline uint32_t i32x4_extract_2(i32x4 a) {
    uint64_t lo = i64x2_extract_l(a);
    return lo & 0xffffffff;
}
