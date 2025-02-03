#include <immintrin.h>
#include <stdio.h>
#include <stdint.h>

// 32x2 64-bit SIMD registers
typedef __m64 i64;

// 64x2 128-bit SIMD registers
typedef __m128d f128;
typedef __m128i i128;

inline i64 i64_zero() { return _mm_setzero_si64(); }
inline f128 f128_zero() { return _mm_setzero_pd(); }
inline i128 i128_zero() { return _mm_setzero_si128(); }

/*
 * Returns the i64 value containing the concatenation of hi and lo.
 */
static inline i64 i32x2_make(uint32_t e1, uint32_t e0) {
    return _mm_set_pi32(e1, e0);
}

/*
 * Returns the lower (e0) 32-bit value from an i64.
 */
static inline uint32_t i32x2_extract_l(i64 in) {
    return (uint32_t)_mm_cvtsi64_si32(in);
}

/*
 * Returns the higher (e1) 32-bit value from an i64.
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
static inline i128 i64x2_make(uint64_t e1, uint64_t e0) {
    return _mm_set_epi64x(e1, e0);
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


/*
 * Add each lane of the 64x2 and 32x2 vectors and store the result in a 64x2 vector.
 */
//static inline i128 vaddw_u32(i128 a, i64 b) {
    //// Extract the lower and higher 32-bit lanes from b.
    //uint32_t b_lo = i32x2_extract_l(b);
    //uint32_t b_hi = i32x2_extract_h(b);

    //// Zero-extend the 32-bit values to 64-bit.
    //uint64_t b_lo64 = (uint64_t)b_lo;
    //uint64_t b_hi64 = (uint64_t)b_hi;

    //// Pack the extended values into a 64x2 vector.
    //i128 wide_b = i64x2_make(b_hi64, b_lo64);

    //// Add the 64-bit lanes of a and wide_b.
    //return i64x2_add(a, wide_b);
//}
