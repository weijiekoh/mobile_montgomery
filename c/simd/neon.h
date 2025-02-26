#include <arm_neon.h>
#include <stdbool.h>

// Define a 32x2 SIMD vector (64 bits total)
typedef uint32x2_t i64;

// Define a 64x2 SIMD vector (128 bits total)
typedef uint64x2_t i128;

typedef uint32x4_t i32x4;

// Define a 32x4x2 SIMD vector (128 bits total)
typedef struct {
    i128 val[2];
} i128x2;

static inline uint64x2_t cast_u32x4_to_u64x2(uint32x4_t v) {
    union {
        uint32x4_t v32;
        uint64x2_t v64;
    } u;
    u.v32 = v;
    return u.v64;
}

static inline uint32x4_t cast_u64x2_to_u32x4(uint64x2_t v) {
    union {
        uint64x2_t v64;
        uint32x4_t v32;
    } u;
    u.v64 = v;
    return u.v32;
}

// Initialize an i64 (2×32-bit vector) to 0's
static inline i64 i64_zero(void) {
    return vdup_n_u32(0);
}

/*
 * Create an i64 vector from two 32-bit elements.
 */
static inline i64 i32x2_make(uint32_t hi, uint32_t lo) {
    return (i64){ hi, lo };
}

static inline i64 i32x2_splat(uint32_t x) {
    return vdup_n_u32(x);
}

/*
 * Extract the low element from an i64 vector.
 */
static inline uint32_t i32x2_extract_l(i64 in) {
    return vget_lane_u32(in, 1);
}

/*
 * Extract the high element from an i64 vector.
 */
static inline uint32_t i32x2_extract_h(i64 in) {
    return vget_lane_u32(in, 0);
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
static inline i128 i64x2_make(uint64_t hi, uint64_t lo) {
    // The initializer order is { lane0, lane1 }.
    return (i128){ hi, lo };
}

static inline i128 i64x2_splat(uint64_t x) {
    return vdupq_n_u64(x);
}

static inline i128x2 i32x8_make(
    uint32_t e7, uint32_t e6, uint32_t e5, uint32_t e4,
    uint32_t e3, uint32_t e2, uint32_t e1, uint32_t e0
) {
    i128x2 result;
    // Pack so that the lowest lanes form an ascending sequence from e0 upward.
    uint64_t e0e1 = (uint64_t) i32x2_make(e1, e0);
    uint64_t e2e3 = (uint64_t) i32x2_make(e3, e2);
    uint64_t e4e5 = (uint64_t) i32x2_make(e5, e4);
    uint64_t e6e7 = (uint64_t) i32x2_make(e7, e6);

    result.val[0] = i64x2_make(e0e1, e2e3);
    result.val[1] = i64x2_make(e4e5, e6e7);
    return result;
}

/*
 * Extract the low (lane 0) element from an i128 vector.
 */
static inline uint64_t i64x2_extract_l(i128 in) {
    return vgetq_lane_u64(in, 1);
}

/*
 * Extract the high (lane 1) element from an i128 vector.
 */
static inline uint64_t i64x2_extract_h(i128 in) {
    return vgetq_lane_u64(in, 0);
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

void print_i128(i128 in) {
    //// Extract each lane
    //uint64_t val0 = vgetq_lane_u64(in, 0);
    //uint64_t val1 = vgetq_lane_u64(in, 1);

    //// Print the values as hex (casting to unsigned long long for portability)
    //printf("0x%016llx %016llx\n", (unsigned long long)val0, (unsigned long long)val1);
    uint64_t hi = i64x2_extract_h(in);
    uint64_t lo = i64x2_extract_l(in);
    printf("(lo: %016lx; hi: %016lx)", lo, hi);
}

// "Unsigned Multiply long (vector). This instruction multiplies corresponding
// vector elements in the lower or upper half of the two source SIMD&FP
// registers, places the result in a vector, and writes the vector to the
// destination SIMD&FP register. The destination vector elements are twice as
// long as the elements that are multiplied. All the values in this instruction
// are unsigned integer values."
// https://developer.arm.com/architectures/instruction-sets/intrinsics/vmull_u32
static inline i128 i64x2_mul(i64 a, i64 b) {
    return vmull_u32(a, b);
}

static inline i128 i64x2_widening_add(i64 a, i64 b) {
    return vaddl_u32(a, b);
}

static inline i128x2 transpose(i128 a, i128 b) {
    uint32x4x2_t t = vtrnq_u32((uint32x4_t) a, (uint32x4_t) b);

    i128x2 res;
    res.val[0] = cast_u32x4_to_u64x2(t.val[0]);
    res.val[1] = cast_u32x4_to_u64x2(t.val[1]);
    return res;
}

// Extract the lowest 32-bit element from an i128 vector
// high -> 0 1 2 3 <- low

static inline bool i128_eq(i128 a, i128 b) {
    uint64_t a0 = i64x2_extract_l(a);
    uint64_t a1 = i64x2_extract_h(a);
    uint64_t b0 = i64x2_extract_l(b);
    uint64_t b1 = i64x2_extract_h(b);
    return a0 == b0 && a1 == b1;
}

static inline i128 trn1(uint32x4_t a, uint32x4_t b) {
    return (i128) vtrn1q_u32(a, b);
}

static inline i128 trn2(uint32x4_t a, uint32x4_t b) {
    return (i128) vtrn2q_u32(a, b);
}

static inline i128 extq(i32x4 a, i32x4 b, int i) {
    return (i128) vextq_u32(a, b, i);
}

static inline i32x4 i32x4_zero() {
    return vdupq_n_u32(0);
}

static inline uint32_t i32x4_extract(i32x4 a, int i) { 
    return vgetq_lane_u32(a, i);
}

static inline uint32_t i32x4_extract_0(i32x4 a) {
    return vgetq_lane_u32(a, 0);
}

static inline uint32_t i32x4_extract_2(i32x4 a) {
    return vgetq_lane_u32(a, 2);
}
