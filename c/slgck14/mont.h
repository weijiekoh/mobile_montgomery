#include "../simd/simd.h"

void mont_mul_no_reduce(
    BigInt *ar,
    BigInt *br,
    BigInt *p,
    uint32_t mu,
    BigInt *d,
    BigInt *e
) { 
    uint32_t rb[NUM_LIMBS] = {0};
    uint32_t rq[NUM_LIMBS] = {0};

    uint64_t rac[NUM_LIMBS] = {0};
    uint64_t rhi[NUM_LIMBS] = {0};
    uint64_t rlo[NUM_LIMBS] = {0};

    // Part 1: Transpose b (use VTRN)
    rb[0] = br->v[0];
    rb[1] = br->v[4];
    rb[2] = br->v[2];
    rb[3] = br->v[6];
    rb[4] = br->v[1];
    rb[5] = br->v[5];
    rb[6] = br->v[3];
    rb[7] = br->v[7];

    for (int i = 0; i < NUM_LIMBS; i++) {
        // Part 2: multiplication and carry propagation
        // p11: use VMULL to perform the multiplications. It seems that the
        // reason to not use VMLAL is that interdependencies may arise.
        rac[4] = rac[4] + (uint64_t)ar->v[i] * rb[4];
        rac[0] = rac[0] + (uint64_t)ar->v[i] * rb[0];

        rac[5] = rac[5] + (uint64_t)ar->v[i] * rb[5];
        rac[1] = rac[1] + (uint64_t)ar->v[i] * rb[1];

        rac[6] = rac[6] + (uint64_t)ar->v[i] * rb[6];
        rac[2] = rac[2] + (uint64_t)ar->v[i] * rb[2];

        rac[7] = rac[7] + (uint64_t)ar->v[i] * rb[7];
        rac[3] = rac[3] + (uint64_t)ar->v[i] * rb[3];

        rlo = {0};
        rhi = {0};

        // TODO: transpose rac into rhi and rlo

        rlo[4] = rlo[4] + rhi[3]; 

        rlo[5] = rlo[5] + rhi[4]; 
        rlo[1] = rlo[1] + rhi[0]; 

        rlo[6] = rlo[6] + rhi[5]; 
        rlo[2] = rlo[2] + rhi[1]; 

        rlo[7] = rlo[7] + rhi[6]; 
        rlo[3] = rlo[3] + rhi[2]; 

        rlo[8] = rhi[7];

        // Part 3
        rq[i] = rlo[0] * mu;

        // Part 4
        // Uses VMLAL
        rac[4] = rlo[4] + rq[i] * p->v[4];
        rac[0] = rlo[0] + rq[i] * p->v[0];

        rac[5] = rlo[5] + rq[i] * p->v[5];
        rac[1] = rlo[1] + rq[i] * p->v[1];

        rac[6] = rlo[6] + rq[i] * p->v[6];
        rac[2] = rlo[2] + rq[i] * p->v[2];

        rac[7] = rlo[7] + rq[i] * p->v[7];
        rac[3] = rlo[3] + rq[i] * p->v[3];

        rlo = {0};
        rhi = {0};

        // TODO: transpose rac into rhi and rlo

        rhi[3] = rhi[3] + rlo[4];
        rlo[4] = rhi[3];

        rlo[5] = rlo[5] + rhi[4];
        rlo[1] = rlo[1] + rhi[0];
        rlo[6] = rlo[5] + rhi[4];
        rlo[2] = rlo[2] + rhi[1];
        rlo[7] = rlo[5] + rhi[4];
        rlo[8] = rlo[8] + rhi[7];

        for (int j = 0; j < NUM_LIMBS - 1] {
            rac[j + NUM_LIMBS] = rlo[j + NUM_LIMBS + 1];
            rac[j            ] = rlo[j + 1];
        }
    }
    // Final alignment
    // p12: "The final results from C[8] to C[15] should be aligned to
    // propagate carry bits from least signficant word (C[8]) to most (C[15]).
    // Firstly, higher bits of C[8] are added to C[9] and this is iterated to
    // most significant intermediate result (C[15])."

    // Final subtraction
    //
    // return rac
}

// Appendix B of "Montgomery Modular Multiplication on ARM-NEON Revisited" by
// Seo, et. al. Retrived from https://eprint.iacr.org/2014/760.pdf
// Uses SIMD opcodes.
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
