#include <arm_neon.h>

// Define a 32x2 SIMD vector (64 bits total)
typedef uint32x2_t i64;

// Define a 64x2 SIMD vector (128 bits total)
typedef uint64x2_t i128;

// Initialize an i64 (2×32-bit vector) to 0's
static inline i64 i64_zero(void) {
    return vdup_n_u32(0);
}

/*
 * Create an i64 vector from two 32-bit elements.
 * e1 is placed in lane 1 (the high half) and e0 in lane 0 (the low half).
 */
static inline i64 i32x2_make(uint32_t e1, uint32_t e0) {
    // Note: The order in the initializer is {lane0, lane1}
    return (i64){ e0, e1 };
}

/*
 * Extract the low (lane 0) element from an i64 vector.
 */
static inline uint32_t i32x2_extract_l(i64 in) {
    return vget_lane_u32(in, 0);
}

/*
 * Extract the high (lane 1) element from an i64 vector.
 */
static inline uint32_t i32x2_extract_h(i64 in) {
    return vget_lane_u32(in, 1);
}

/*
 * Print the binary representation of an i64 (as two 32-bit hexadecimal values)
 * in the format: "(lo: lo, hi: hi)".
 */
void print_i64(i64 in) {
    uint32_t hi = i32x2_extract_h(in);
    uint32_t lo = i32x2_extract_l(in);
    printf("(lo: %08x; hi: %08x)", lo, hi);
}

/*
 * Create an i128 vector from two 64-bit elements.
 * The parameter 'e1' is placed in lane 1 (the high 64 bits)
 * and 'e0' is placed in lane 0 (the low 64 bits).
 */
static inline i128 i64x2_make(uint64_t e1, uint64_t e0) {
    // The initializer order is { lane0, lane1 }.
    return (i128){ e0, e1 };
}

/*
 * Extract the low (lane 0) element from an i128 vector.
 */
static inline uint64_t i64x2_extract_l(i128 in) {
    return vgetq_lane_u64(in, 0);
}

/*
 * Extract the high (lane 1) element from an i128 vector.
 */
static inline uint64_t i64x2_extract_h(i128 in) {
    return vgetq_lane_u64(in, 1);
}

// Initialize an i128 (2×64-bit vector) to 0's
static inline i128 i128_zero(void) {
    return vdupq_n_u64(0);
}

// madd: a + b*c, using a 64-bit accumulator (i128)
// The NEON intrinsic vmlal_u32 does a multiply long of two 32-bit vectors,
// adding the result to a 64-bit vector.
static inline i128 madd(i128 a, i64 b, i64 c) {
    return vmlal_u32(a, b, c);
}

// u64x2_shr: shift right each 64-bit element by s bits
static inline i128 u64x2_shr(i128 a, int s) {
    return vshrq_n_u64(a, s);
}

// i64x2_add: add two 128-bit vectors (each holding two 64-bit numbers)
static inline i128 i64x2_add(i128 a, i128 b) {
    return vaddq_u64(a, b);
}

// i128x2_and: bitwise and of two 128-bit vectors
static inline i128 i128_and(i128 a, i128 b) {
    return vandq_u64(a, b);
}
