#include "../simd/simd.h"

void mont_mul_no_reduce(
    BigInt *ar,
    BigInt *br,
    BigInt *p,
    uint64_t mu,
    BigInt *d,
    BigInt *e
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

    for (int i = 0; i < NUM_LIMBS; i ++) {
        d->v[i] = i64x2_extract_h(de[i]);
        e->v[i] = i64x2_extract_l(de[i]);
    }
}

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
    BigInt d = bigint_new();
    BigInt e = bigint_new();
    mont_mul_no_reduce(ar, br, p, mu, &d, &e);

    BigInt result = bigint_new();
    if (bigint_gt(&e, &d)) {
        BigInt e_minus_d = bigint_sub(&e, &d);
        result = bigint_sub(p, &e_minus_d);
    } else {
        result = bigint_sub(&d, &e);
    }

    return result;
}
