#include "../simd/simd.h"
/// Algorithm 4 of "Montgomery Arithmetic from a Software Perspective" by Bos and Montgomery
/// Uses SIMD opcodes.
/// Also see:
/// https://github.com/coreboot/vboot/blob/060efa0cf64d4b7ccbe3e88140c9da5f747355ee/firmware/2lib/2modpow_sse2.c#L113
BigInt mont_mul(
    BigInt *ar,
    BigInt *br,
    BigInt *p,
    uint64_t mu
) {
    i128 mask = i64x2_make(LIMB_MASK, LIMB_MASK);

    i128 de[8] = {
        i128_zero(),
        i128_zero(),
        i128_zero(),
        i128_zero(),
        i128_zero(),
        i128_zero(),
        i128_zero(),
        i128_zero(),
    };

    uint64_t mu_b0 = mu * br->v[0];

    i128 bp[8];
    for (int i = 0; i < NUM_LIMBS; i ++) {
        bp[i] = i64x2_make(br->v[i], p->v[i]);
    }

    uint64_t d0, e0, d0_minus_e0, q;
    i128 aq, t01;

    for (int j = 0; j < NUM_LIMBS; j ++) {
        // Compute q
        d0 = i64x2_extract_h(de[0]);
        e0 = i64x2_extract_l(de[0]);

        d0_minus_e0 = d0 - e0;

        // q = (mub0)aj + mu(d0 - e0) mod 2^32
        q = (mu_b0 * ar->v[j] + mu * d0_minus_e0) & LIMB_MASK;

        // t0 = ajb0 + d0
        // t1 = qp0 + e0

        // aq = ar[j], q
        aq = i64x2_make(ar->v[j], q);

        t01 = i64x2_add(
            // ajb0, qp0
            u64x2_mul(aq, bp[0]),
            de[0]
        );

        t01 = u64x2_shr(t01, 32);

        i128 p01;
        for (int i = 1; i < NUM_LIMBS; i ++) {
            // p0 = ajbi + t0 + di
            // p1 = qpi + t1 + ei
            p01 = i64x2_add(
                i64x2_add(t01, de[i]),
                u64x2_mul(aq, bp[i])
            );

            // t0 = p0 / 2^32
            // t1 = p1 / 2^32
            t01 = u64x2_shr(p01, 32);

            // d[i-1] = p0 mod 2^32
            // e[i-1] = p1 mod 2^32
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

    if (bigint_gt(&e, &d)) {
        BigInt e_minus_d = bigint_sub(&e, &d);
        return bigint_sub(p, &e_minus_d);
    }
    return bigint_sub(&d, &e);
}
