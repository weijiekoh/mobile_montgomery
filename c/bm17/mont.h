#include "../simd/simd.h"

/// Algorithm 4 of "Montgomery Arithmetic from a Software Perspective" by Bos and Montgomery
/// Uses SIMD opcodes.
/// Also see:
/// https://github.com/coreboot/vboot/blob/060efa0cf64d4b7ccbe3e88140c9da5f747355ee/firmware/2lib/2modpow_sse2.c#L113
/*
 * We need to leverage the following NEON instructions:
 *   - madd: vmlal_u32(i128 a, i64 b, i64 c) -> a + b * c
 *   - u64x2_shr: vshrq_n_u64
 *   - i64x2_add: vaddq_u64 (TODO: check if it should be signed or unsigned addition)
 *   - i128x2_and: vand_u64
 *
 * u32: ar, br, p, mu, q, mu_b0
 * 32x2: aq, bp[i]
 * 64x2: t01, p01, de[i], t01de
 *
 * - Initialise the above variables to 0
 * - mu_b0 = mu * br[0]
 * - mask = i64x2_make(0xFFFFFFFF, 0xFFFFFFFF)
 * - bp[] = i32x2_make(br[i], p[i]) for i in 0..NUM_LIMBS
 * - For j = 0 to NUM_LIMBS:
 *    - q = mu_b0 * ar[j] + mu * (d0 - e0)
 *    - aq = i32x2_make(ar[j], q)
 *    - t01 = u64x2_shr(madd(de[0], aq, bp[0]), 32)
 *    - For i = 1 to NUM_LIMBS:
 *      - t01de = u64x2_add(t01, de[i])
 *      - p01 = madd(t01de, aq, bp[i])
 *      - t01 = u64x2_shr(p01, 32)
 *      - de[i] = i128_and(p01, mask)
 *  - C = D - E
 *  - if C < 0, return C + M; else return C
 */
BigInt mont_mul(
    BigInt *ar,
    BigInt *br,
    BigInt *p,
    uint64_t mu
) { 
    uint32_t q;
    uint32_t mu_32 = (uint32_t) mu;
    uint32_t mu_b0 = mu_32 * (uint32_t) br->v[0];
    i64 aq;
    i64 bp[NUM_LIMBS];
    i128 de[NUM_LIMBS];
    i128 t01, p01, t01de;
    i128 mask = i64x2_make(LIMB_MASK, LIMB_MASK);

    for (int i = 0; i < NUM_LIMBS; i ++) {
        de[i] = i128_zero();
        bp[i] = i32x2_make(br->v[i], p->v[i]);
    }

    for (int j = 0; j < NUM_LIMBS; j ++) {
        uint64_t d0 = i64x2_extract_h(de[0]);
        uint64_t e0 = i64x2_extract_l(de[0]);
        q = mu_b0 * (uint32_t) ar->v[j] + mu_32 * (uint32_t) (d0 - e0);
        aq = i32x2_make(ar->v[j], q);
        t01 = u64x2_shr(madd(de[0], aq, bp[0]), 32);

        for (int i = 1; i < NUM_LIMBS; i ++) {
            t01de = i64x2_add(t01, de[i]);
            p01 = madd(t01de, aq, bp[i]);
            t01 = u64x2_shr(p01, 32);
            de[i - 1] = i128_and(p01, mask);
        }
        de[NUM_LIMBS - 1] = t01;
    }

    BigInt d = bigint_new();
    BigInt e = bigint_new();
    for (int i = 0; i < NUM_LIMBS; i ++) {
        d.v[i] = i64x2_extract_h(de[i]);
        e.v[i] = i64x2_extract_l(de[i]);
    }

    BigInt result = bigint_new();
    if (bigint_gt(&e, &d)) {
        BigInt e_minus_d = bigint_sub(&e, &d);
        result = bigint_sub(p, &e_minus_d);
    } else {
        result = bigint_sub(&d, &e);
    }

    return result;
}

/*
 * Ignore the following; it is here just for reference, and can be deleted at a
 * later commit.

BigInt mont_mul_old(
    BigInt *ar,
    BigInt *br,
    BigInt *p,
    uint64_t mu
) { 
    // Old variable declarations
    uint64_t d0, e0, d0_minus_e0, q;
    i128 aq, t01, p01;
    i128 de[8] = {
        i128_zero(), i128_zero(), i128_zero(), i128_zero(),
        i128_zero(), i128_zero(), i128_zero(), i128_zero(),
    };
    i128 mask = i64x2_make(LIMB_MASK, LIMB_MASK);
    uint64_t mu_b0 = mu * br->v[0];
    i128 bp[NUM_LIMBS];
    for (int i = 0; i < NUM_LIMBS; i ++) {
        bp[i] = i64x2_make(br->v[i], p->v[i]);
    }

    // New variable declarations
    uint32_t d0_32, e0_32, d0_32_minus_e0_32, q_32;
    i64 aq_64, t01_64;
    i64 de_64[8] = {
        i64_zero(), i64_zero(), i64_zero(), i64_zero(),
        i64_zero(), i64_zero(), i64_zero(), i64_zero(),
    };
    uint32_t ar_32[NUM_LIMBS] = {0};
    uint32_t br_32[NUM_LIMBS] = {0};
    for (int i = 0; i < NUM_LIMBS; i ++) {
        ar_32[i] = (uint32_t) ar->v[i];
        br_32[i] = (uint32_t) br->v[i];
    }
    uint32_t mu_32 = (uint32_t) mu;
    uint32_t mu_b0_32 = mu_32 * br_32[0];
    i64 bp_32[NUM_LIMBS];
    for (int i = 0; i < NUM_LIMBS; i ++) {
        bp_32[i] = i32x2_make(br_32[i], (uint32_t) p->v[i]);
    }

    for (int j = 0; j < NUM_LIMBS; j ++) {
        // Compute q
        d0 = i64x2_extract_h(de[0]);
        e0 = i64x2_extract_l(de[0]);
        d0_minus_e0 = d0 - e0;

        d0_32 = i32x2_extract_h(de_64[0]);
        e0_32 = i32x2_extract_l(de_64[0]);
        d0_32_minus_e0_32 = d0_32 - e0_32;

        q = (mu_b0 * ar->v[j] + mu * d0_minus_e0) & LIMB_MASK;

        q_32 = mu_b0_32 * ar_32[j] + mu_32 * d0_32_minus_e0_32;

        // t0 = ajb0 + d0
        // t1 = qp0 + e0

        // aq = ar[j], q
        aq = i64x2_make(ar->v[j], q);
        aq_64 = i32x2_make(ar_32[j], q_32); // May be unnecessary

        // t0| |t1| |de|  |shr|
        // --|-|--|-|--|--|--|
        // aj|*|b0|+|d0|>>|32|
        // q |*|p0|+|e0|>>|32|
        i128 aqbp0 = u64x2_mul(aq, bp[0]);

        t01 = u64x2_shr(
            i64x2_add(aqbp0, de[0]),
            32
        );

        //t01_64 = u32x2_high(
            //vmlal_u32(de[0], aq_64, bp_32[0])
        //);

        for (int i = 1; i < NUM_LIMBS; i ++) {
            // t01 + de
            i128 t01de = i64x2_add(t01, de[i]);
            
            // Old code
            // p0 = ajbi + t0 + di
            // p1 = qpi + t1 + ei
            p01 = i64x2_add(
                t01de,
                u64x2_mul(aq, bp[i])
            );

            // t0 = p0 / 2^32; t1 = p1 / 2^32
            t01 = u64x2_shr(p01, 32);

            // d[i-1] = p0 mod 2^32
            // e[i-1] = p1 mod 2^32
            de[i - 1] = i128_and(p01, mask);

            // New code
            i128 p01_new = vmlal_u32(t01de, aq_64, bp_32[i]);

            //t01_64 = u32x2_high(p01_new);

        }

        de[NUM_LIMBS - 1] = t01;
    }

    BigInt d = bigint_new();
    BigInt e = bigint_new();
    for (int i = 0; i < NUM_LIMBS; i ++) {
        d.v[i] = i64x2_extract_h(de[i]);
        e.v[i] = i64x2_extract_l(de[i]);
    }

    BigInt result = bigint_new();
    if (bigint_gt(&e, &d)) {
        BigInt e_minus_d = bigint_sub(&e, &d);
        result = bigint_sub(p, &e_minus_d);
    } else {
        result = bigint_sub(&d, &e);
    }

    return result;
}

/// Backup of a version that incorrectly uses 64x2-bit vectors instead of 32x2-bit vectors
BigInt mont_mul_bak2(
    BigInt *ar,
    BigInt *br,
    BigInt *p,
    uint64_t mu
) {
    i128 mask = _mm_set_epi32(0, 0xFFFFFFFF, 0, 0xFFFFFFFF);

    i64 de[8] = {
        i64_zero(),
        i64_zero(),
        i64_zero(),
        i64_zero(),
        i64_zero(),
        i64_zero(),
        i64_zero(),
        i64_zero(),
    };

    uint32_t mu_b0 = mu * br->v[0];

    i64 bp[8];
    for (int i = 0; i < NUM_LIMBS; i ++) {
        bp[i] = i32x2_make((uint32_t)br->v[i], (uint32_t)p->v[i]);
    }

    uint32_t d0, e0, d0_minus_e0, q;
    i128 aq, t01;

    for (int j = 0; j < NUM_LIMBS; j ++) {
        // Compute q
        d0 = i32x2_extract_h(de[0]);
        e0 = i32x2_extract_l(de[0]);

        d0_minus_e0 = d0 - e0;

        // q = (mub0)aj + mu(d0 - e0) mod 2^32
        q = mu_b0 * ((uint32_t)ar->v[j]) + mu * d0_minus_e0;

        // t0 = ajb0 + d0
        // t1 = qp0 + e0

        // aq = ar[j], q
        aq = i64x2_make(ar->v[j], (uint64_t) q);

        t01 = vmlal_u32(aq, bp[0], de[0]);

        t01 = u64x2_shr(t01, 32);

        i128 p01;
        for (int i = 1; i < NUM_LIMBS; i ++) {
            // p0 = ajbi + t0 + di
            // p1 = qpi + t1 + ei
            p01 = i64x2_add(
               vmlal_u32(aq, bp[i], de[i]),
               t01
            );

            // t0 = p0 / 2^32
            // t1 = p1 / 2^32
            t01 = u64x2_shr(p01, 32);

            // d[i-1] = p0 mod 2^32
            // e[i-1] = p1 mod 2^32
            
            // p01 holds two 64-bit lanes.
            // Create a mask to zero out the high 32 bits in each lane.
            // Mask off the high parts:
            __m128i masked = _mm_and_si128(p01, mask);
            // Shuffle to pack the lower 32 bits of each 64-bit lane into the lower 64 bits:
            __m128i result128 = _mm_shuffle_epi32(masked, _MM_SHUFFLE(2, 0, 2, 0));

            // Now extract the lower 64 bits to assign to de[i-1] (which is of type __m64).
            de[i - 1] = _mm_cvtsi64_m64(_mm_cvtsi128_si64(result128));

        }
        __m128i shuffled = _mm_shuffle_epi32(t01, _MM_SHUFFLE(2, 0, 2, 0));
        // Then cast to __m64 to take just the lower 64 bits.
        de[NUM_LIMBS - 1] = *(__m64*)&shuffled;
    }

    BigInt d = bigint_new();
    BigInt e = bigint_new();

    for (int i = 0; i < NUM_LIMBS; i ++) {
        d.v[i] = (uint64_t) i32x2_extract_h(de[i]);
        e.v[i] = (uint64_t) i32x2_extract_l(de[i]);
    }

    if (bigint_gt(&e, &d)) {
        BigInt e_minus_d = bigint_sub(&e, &d);
        return bigint_sub(p, &e_minus_d);
    }
    return bigint_sub(&d, &e);
}

// Given a 64x2 vector i128 `a`, return a 32x2 vector i64 where each 32-bit
// lane contains the high 32 bits of the corresponding 64-bit lane of `a`.
static inline i64 u32x2_high(i128 a) {
    // Extract the low and high 64-bit lanes from the 128-bit vector.
    uint64_t a_lo = i64x2_extract_l(a);
    uint64_t a_hi = i64x2_extract_h(a);

    // The high 32 bits of each 64-bit lane are obtained by shifting right by 32 bits.
    uint32_t lo_high = (uint32_t)(a_lo >> 32);
    uint32_t hi_high = (uint32_t)(a_hi >> 32);

    // Pack the two 32-bit values into a 64-bit vector.
    // Note: i32x2_make(e1, e0) puts e1 in the high 32 bits and e0 in the low 32 bits.
    return i32x2_make(hi_high, lo_high);
}

static inline i128 u64x2_mul(i128 a, i128 b) {
    return _mm_mul_epu32(a, b);
}

*/
