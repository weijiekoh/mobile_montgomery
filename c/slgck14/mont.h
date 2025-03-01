#include <assert.h>
#include "../simd/simd.h"
#include "../transpose.h"

/// Returns the higher 32 bits.
static inline uint64_t hi(uint64_t v) {
    return v >> BITS_PER_LIMB;
}

/// Returns the lower 32 bits.
static inline uint64_t lo(uint64_t v) {
    return v & LIMB_MASK;
}

/// Propagate carries in an array of limbs.
/// 'x' has 'len' entries.
static inline void carry_propagate(uint64_t *x, int len) {
    uint64_t carry = 0;
    for (int i = 0; i < len; i++) {
        x[i] += carry;
        carry = x[i] >> BITS_PER_LIMB;
        x[i] &= LIMB_MASK;
    }
}

// Hwajeong Seo, et al
// From Montgomery Modular Multiplication on ARM-NEON Revisited
// https://eprint.iacr.org/2014/760.pdf
void mont_mul_no_reduce(
    i64 vai[NUM_LIMBS],
    i64 transposed_b[NUM_LIMBS],
    i64 transposed_p[4],
    uint64_t n0,
    uint64_t *t
) {
    // SIMD
    uint64_t carry_s;
    i128 res02 = i128_zero();
    i128 res13 = i128_zero();
    i128 res46 = i128_zero();
    i128 res57 = i128_zero();

    i128 res02_mask = i64x2_make(0xffffffffffffffff, LIMB_MASK);

    i128 vrac[4] = {i128_zero()};

    i64 vb40 = transposed_b[0];
    i64 vb62 = transposed_b[1];
    i64 vb51 = transposed_b[2];
    i64 vb73 = transposed_b[3];

    i64 vp40 = transposed_p[0];
    i64 vp62 = transposed_p[1];
    i64 vp51 = transposed_p[2];
    i64 vp73 = transposed_p[3];

    i64 v00 = i64_zero();
    i64 v01 = i64_zero();
    i64 v10 = i64_zero();
    i64 v11 = i64_zero();
    i64 v20 = i64_zero();
    i64 v21 = i64_zero();
    i64 v30 = i64_zero();
    i64 v31 = i64_zero();
    uint64_t rhi7 = 0;

    i128 vv0_0 = i128_zero();
    i128 vv0_1 = i128_zero();
    i128 vv1_0 = i128_zero();
    i128 vv1_1 = i128_zero();

    i32x4 zero = i32x4_zero();
    uint64_t t8 = 0;

    i128 v20_add_10 = i128_zero();
    i128 v21_add_11 = i128_zero();
    i128 v00_add_30 = i128_zero();
    i128 v01_add_31 = i128_zero();

    uint64_t r0 = 0;
    i128 add = i128_zero();

    uint32x4x2_t vv0;
    uint32x4x2_t vv1;

    uint64_t m_s = 0;

    for (int i = 0; i < NUM_LIMBS; i++) {
        // 4x vmull
        vrac[0] = i64x2_mul(vai[i], vb40);
        vrac[1] = i64x2_mul(vai[i], vb62);
        vrac[2] = i64x2_mul(vai[i], vb51);
        vrac[3] = i64x2_mul(vai[i], vb73);

        // Transpose
        vv0 = vtrnq_u32((uint32x4_t) vrac[1], (uint32x4_t) vrac[0]);
        vv1 = vtrnq_u32((uint32x4_t) vrac[3], (uint32x4_t) vrac[2]);

        vv0_0 = (i128) vv0.val[0];
        vv0_1 = (i128) vv0.val[1];
        vv1_0 = (i128) vv1.val[0];
        vv1_1 = (i128) vv1.val[1];

        // Fix up vv1_1 so that 0 is added to rlo0
        vv1_1 = (i128) vextq_u32((uint32x4_t) vv1_1, zero, 1);

        v00 = (i64) i64x2_extract_l(vv0_0); // rlo0, rlo2
        v01 = (i64) i64x2_extract_h(vv0_0); // rlo4, rlo6
        v10 = (i64) i64x2_extract_l(vv0_1); // rhi0, rhi2
        v11 = (i64) i64x2_extract_h(vv0_1); // rhi4, rhi6
        v20 = (i64) i64x2_extract_l(vv1_0); // rlo1, rlo3
        v21 = (i64) i64x2_extract_h(vv1_0); // rlo5, rlo7
        v30 = (i64) i64x2_extract_l(vv1_1); // 0000, rhi1
        v31 = (i64) i64x2_extract_h(vv1_1); // rhi3, rhi5
        rhi7 = i32x4_extract_1((i32x4) vrac[3]);

        v00_add_30 = (i128) i64x2_widening_add(v00, v30); // rlo0 + rhi1; rlo2 + rhi3
        v20_add_10 = (i128) i64x2_widening_add(v20, v10); // rlo1 + rhi0; rlo3 + rhi2
        v01_add_31 = (i128) i64x2_widening_add(v01, v31); // rlo4 + rhi3; rlo6 + rhi5
        v21_add_11 = (i128) i64x2_widening_add(v21, v11); // rlo5 + rhi4; rlo7 + rhi6
        t8 += rhi7;

        res02 = i64x2_add(res02, v00_add_30);
        res13 = i64x2_add(res13, v20_add_10);
        res46 = i64x2_add(res46, v01_add_31);
        res57 = i64x2_add(res57, v21_add_11);

        // Partial carry
        r0 = i64x2_extract_l(res02);
        carry_s = r0 >> BITS_PER_LIMB; 
        res02 = i128_and(res02, res02_mask);
        add = i64x2_make(0, carry_s);
        res13 = i64x2_add(res13, add);

        m_s = ((r0 & LIMB_MASK) * n0) & LIMB_MASK;
        i64 mm = i32x2_make(m_s, m_s);

        // 4x VMUL
        // TODO: use VMLAL?
        vrac[0] = i64x2_mul(vp40, mm);
        vrac[1] = i64x2_mul(vp62, mm);
        vrac[2] = i64x2_mul(vp51, mm);
        vrac[3] = i64x2_mul(vp73, mm);

        // Transpose
        vv0 = vtrnq_u32((uint32x4_t) vrac[1], (uint32x4_t) vrac[0]);
        vv1 = vtrnq_u32((uint32x4_t) vrac[3], (uint32x4_t) vrac[2]);

        vv0_0 = (i128) vv0.val[0];
        vv0_1 = (i128) vv0.val[1];
        vv1_0 = (i128) vv1.val[0];
        vv1_1 = (i128) vv1.val[1];

        // Fix up vv1_1 so that 0 is added to rlo0
        vv1_1 = (i128) vextq_u32((uint32x4_t) vv1_1, zero, 1);

        v00 = (i64) i64x2_extract_l(vv0_0); // rlo0, rlo2
        v01 = (i64) i64x2_extract_h(vv0_0); // rlo4, rlo6
        v10 = (i64) i64x2_extract_l(vv0_1); // rhi0, rhi2
        v11 = (i64) i64x2_extract_h(vv0_1); // rhi4, rhi6
        v20 = (i64) i64x2_extract_l(vv1_0); // rlo1, rlo3
        v21 = (i64) i64x2_extract_h(vv1_0); // rlo5, rlo7
        v30 = (i64) i64x2_extract_l(vv1_1); // 0000, rhi1
        v31 = (i64) i64x2_extract_h(vv1_1); // rhi3, rhi5
        rhi7 = i32x4_extract_1((i32x4) vrac[3]);

        v00_add_30 = (i128) i64x2_widening_add(v00, v30); // rlo0 + rhi1; rlo2 + rhi3
        v20_add_10 = (i128) i64x2_widening_add(v20, v10); // rlo1 + rhi0; rlo3 + rhi2
        v01_add_31 = (i128) i64x2_widening_add(v01, v31); // rlo4 + rhi3; rlo6 + rhi5
        v21_add_11 = (i128) i64x2_widening_add(v21, v11); // rlo5 + rhi4; rlo7 + rhi6
        t8 += rhi7;

        res02 = i64x2_add(res02, v00_add_30);
        res13 = i64x2_add(res13, v20_add_10);
        res46 = i64x2_add(res46, v01_add_31);
        res57 = i64x2_add(res57, v21_add_11);

        // Partial carry
        r0 = i64x2_extract_l(res02);
        carry_s = r0 >> BITS_PER_LIMB; 
        res02 = i128_and(res02, res02_mask);
        add = i64x2_make(0, carry_s);
        res13 = i64x2_add(res13, add);

        // Shift
        i128 temp = res13;
        res13 = extq((i32x4) res46, (i32x4) res02, 2);
        res02 = temp;
        
        temp = res57;
        i128 last = i64x2_make(0, t8);
        res57 = extq((i32x4) last, (i32x4) res46, 2);
        res46 = temp;
        t8 = 0;
    }

    t[0] = i64x2_extract_l(res02);
    t[2] = i64x2_extract_h(res02);
    t[1] = i64x2_extract_l(res13);
    t[3] = i64x2_extract_h(res13);
    t[4] = i64x2_extract_l(res46);
    t[6] = i64x2_extract_h(res46);
    t[5] = i64x2_extract_l(res57);
    t[7] = i64x2_extract_h(res57);
    t[NUM_LIMBS] = 0;

    carry_propagate(t, NUM_LIMBS+1);
}

BigInt mont_mul(
    i64 vai[NUM_LIMBS],
    i64 transposed_b[4],
    BigInt *p,
    i64 transposed_p[4],
    uint64_t n0
) {
    uint64_t t[NUM_LIMBS + 1] = {0};

    mont_mul_no_reduce(vai, transposed_b, transposed_p, n0, t);

    bool t_gt_p = false;
    for (int idx = 0; idx < NUM_LIMBS; idx ++) {
        int i = NUM_LIMBS - idx;
        uint64_t pi = 0;
        if (i < NUM_LIMBS) {
            pi = p->v[i];
        };

        if (t[i] < pi) {
            break;
        } else if (t[i] > pi) {
            t_gt_p = true;
            break;
        }
    }

    if (!t_gt_p) {
        BigInt res;
        res = bigint_new();
        for (int i = 0; i < NUM_LIMBS; i ++) {
            res.v[i] = t[i];
        }
        return res;
    }

    uint64_t result[NUM_LIMBS] = {0};
    uint64_t borrow = 0;

    for (int i = 0; i < NUM_LIMBS; i ++) {
        uint64_t lhs_limb = t[i];
        uint64_t rhs_limb = 0;
        if (i < NUM_LIMBS) {
            rhs_limb = p->v[i];
        }
        uint64_t diff = lhs_limb - rhs_limb - borrow;
        result[i] = diff & LIMB_MASK;
        borrow = (diff >> BITS_PER_LIMB) & 1;
    }

    BigInt res;
    res = bigint_new();
    for (int i = 0; i < NUM_LIMBS; i ++) {
        res.v[i] = result[i];
    }

    return res;
}

/*
    uint64_t prods[NUM_LIMBS] = {0};
    for (int i = 0; i < NUM_LIMBS; i++) {
        for (int j = 0; j < NUM_LIMBS; j++) {
            prods[j] = (uint64_t)ar->v[i] * (uint64_t) br->v[j];
            t[j]     = lo(prod[j]);
            t[j + 1] = hi(prod[j]);
        }

        uint64_t m = t[0] * n0;

        for (int j = 0; j < NUM_LIMBS; j++) {
            uint64_t prod = m * p->v[j];
            t[j + 1] = hi(prod[j]);
        }

        t[0] += lo(prod[0]);
        t[1] += lo(prod[1]);

    }
*/

