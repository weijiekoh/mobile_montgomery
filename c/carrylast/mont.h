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

void mont_mul_no_reduce(
    BigInt *ar,
    BigInt *br,
    BigInt *p,
    uint64_t n0,
    uint64_t *t
) {
    // Non-SIMD
    uint64_t carry;
    uint64_t prods[NUM_LIMBS];

    // SIMD
    uint64_t carry_s;
    i64 vaiai = i64_zero();
    i128 res02 = i128_zero();
    i128 res13 = i128_zero();
    i128 res46 = i128_zero();
    i128 res57 = i128_zero();

    i128 res02_mask = i64x2_make(0xffffffffffffffff, LIMB_MASK);

    i128 vrac[4] = {i128_zero()};
    i128 v[4] = {i128_zero()};

    i64 vb40 = i32x2_make(br->v[4], br->v[0]);
    i64 vb62 = i32x2_make(br->v[6], br->v[2]);
    i64 vb51 = i32x2_make(br->v[5], br->v[1]);
    i64 vb73 = i32x2_make(br->v[7], br->v[3]);

    i64 vp40 = i32x2_make(p->v[4], p->v[0]);
    i64 vp62 = i32x2_make(p->v[6], p->v[2]);
    i64 vp51 = i32x2_make(p->v[5], p->v[1]);
    i64 vp73 = i32x2_make(p->v[7], p->v[3]);

    i64 v00 = i64_zero();
    i64 v01 = i64_zero();
    i64 v10 = i64_zero();
    i64 v11 = i64_zero();
    i64 v20 = i64_zero();
    i64 v21 = i64_zero();
    uint64_t rhi7 = 0;

    i32x4 zero = i32x4_zero();
    uint64_t t8 = 0;

    i128 v20_add_10 = i128_zero();
    i128 v21_add_11 = i128_zero();
    i128 v00_add_v3_shr_0 = i128_zero();
    i128 v01_add_v3_shr_1 = i128_zero();
    i128 v3_shr = i128_zero();
    i64 v3_shr_0 = i64_zero();
    i64 v3_shr_1 = i64_zero();

    uint64_t r0 = 0;
    i128 add = i128_zero();

    for (int i = 0; i < NUM_LIMBS; i++) {
        // Non-SIMD
        for (int j = 0; j < NUM_LIMBS; j++) {
            prods[j] = (uint64_t) ar->v[i] * (uint64_t) br->v[j];
        }

        t[0] += lo(prods[0]);
        t[1] += hi(prods[0]) + lo(prods[1]);
        t[2] += hi(prods[1]) + lo(prods[2]);
        t[3] += hi(prods[2]) + lo(prods[3]);
        t[4] += hi(prods[3]) + lo(prods[4]);
        t[5] += hi(prods[4]) + lo(prods[5]);
        t[6] += hi(prods[5]) + lo(prods[6]);
        t[7] += hi(prods[6]) + lo(prods[7]);
        t[8] += hi(prods[7]);

        carry = t[0] >> BITS_PER_LIMB;
        t[0] &= LIMB_MASK;
        t[1] += carry;
 
        // SIMD
        vaiai = i32x2_make(ar->v[i], ar->v[i]);

        // 4x vmull
        vrac[0] = i64x2_mul(vaiai, vb40);
        vrac[1] = i64x2_mul(vaiai, vb62);
        vrac[2] = i64x2_mul(vaiai, vb51);
        vrac[3] = i64x2_mul(vaiai, vb73);

        v[0] = (i128) trn1((i32x4) vrac[1], (i32x4) vrac[0]);
        v[1] = (i128) trn2((i32x4) vrac[1], (i32x4) vrac[0]);
        v[2] = (i128) trn1((i32x4) vrac[3], (i32x4) vrac[2]);
        v[3] = (i128) trn2((i32x4) vrac[3], (i32x4) vrac[2]);

        // TODO: try conditional maps? or another way
        v00 = (i64) i64x2_extract_l(v[0]); // rlo0, rlo2
        v01 = (i64) i64x2_extract_h(v[0]); // rlo4, rlo6
        v10 = (i64) i64x2_extract_l(v[1]); // rhi0, rhi2
        v11 = (i64) i64x2_extract_h(v[1]); // rhi4, rhi6
        v20 = (i64) i64x2_extract_l(v[2]); // rlo1, rlo3
        v21 = (i64) i64x2_extract_h(v[2]); // rlo5, rlo7
        rhi7 = i32x4_extract_1((i32x4) vrac[3]);

        v20_add_10 = (i128) i64x2_widening_add(v20, v10);

        v21_add_11 = (i128) i64x2_widening_add(v21, v11);
        v3_shr = (i128) extq((i32x4) v[3], zero, 1);

        v3_shr_0 = (i64) i64x2_extract_l(v3_shr); // 0000, rhi1
        v3_shr_1 = (i64) i64x2_extract_h(v3_shr); // rhi3, rhi5
        v00_add_v3_shr_0 = (i128) i64x2_widening_add(v00, v3_shr_0); // t0, t2
        v01_add_v3_shr_1 = (i128) i64x2_widening_add(v01, v3_shr_1);
        t8 += rhi7;

        res02 = i64x2_add(res02, v00_add_v3_shr_0);
        res13 = i64x2_add(res13, v20_add_10);
        res46 = i64x2_add(res46, v01_add_v3_shr_1);
        res57 = i64x2_add(res57, v21_add_11);

        // Partial carry
        r0 = i64x2_extract_l(res02);
        carry_s = r0 >> BITS_PER_LIMB; 
        res02 = i128_and(res02, res02_mask);
        add = i64x2_make(0, carry_s);
        res13 = i64x2_add(res13, add);

        uint64_t m = (t[0] * n0) & LIMB_MASK;
        for (int j = 0; j < NUM_LIMBS; j++) {
            prods[j] = m * p->v[j];
        }

        t[0] += lo(prods[0]);
        t[1] += hi(prods[0]) + lo(prods[1]);
        t[2] += hi(prods[1]) + lo(prods[2]);
        t[3] += hi(prods[2]) + lo(prods[3]);
        t[4] += hi(prods[3]) + lo(prods[4]);
        t[5] += hi(prods[4]) + lo(prods[5]);
        t[6] += hi(prods[5]) + lo(prods[6]);
        t[7] += hi(prods[6]) + lo(prods[7]);
        t[8] += hi(prods[7]);

        uint64_t m_s = ((r0 & LIMB_MASK) * n0) & LIMB_MASK;
        i64 mm = i32x2_make(m_s, m_s);

        // 4x VMUL
        vrac[0] = i64x2_mul(vp40, mm);
        vrac[1] = i64x2_mul(vp62, mm);
        vrac[2] = i64x2_mul(vp51, mm);
        vrac[3] = i64x2_mul(vp73, mm);

        // 4x VTRN
        // TODO: refactor
        v[0] = (i128) trn1((i32x4) vrac[1], (i32x4) vrac[0]);
        v[1] = (i128) trn2((i32x4) vrac[1], (i32x4) vrac[0]);
        v[2] = (i128) trn1((i32x4) vrac[3], (i32x4) vrac[2]);
        v[3] = (i128) trn2((i32x4) vrac[3], (i32x4) vrac[2]);

        v00 = (i64) i64x2_extract_l(v[0]); // rlo0, rlo2
        v01 = (i64) i64x2_extract_h(v[0]); // rlo4, rlo6
        v10 = (i64) i64x2_extract_l(v[1]); // rhi0, rhi2
        v11 = (i64) i64x2_extract_h(v[1]); // rhi4, rhi6
        v20 = (i64) i64x2_extract_l(v[2]); // rlo1, rlo3
        v21 = (i64) i64x2_extract_h(v[2]); // rlo5, rlo7
        rhi7 = i32x4_extract_1((i32x4) vrac[3]);

        v20_add_10 = (i128) i64x2_widening_add(v20, v10);
        v21_add_11 = (i128) i64x2_widening_add(v21, v11);
        v3_shr = (i128) extq((i32x4) v[3], zero, 1);

        v3_shr_0 = (i64) i64x2_extract_l(v3_shr); // 0000, rhi1
        v3_shr_1 = (i64) i64x2_extract_h(v3_shr); // rhi3, rhi5
        v00_add_v3_shr_0 = (i128) i64x2_widening_add(v00, v3_shr_0); // t0, t2
        v01_add_v3_shr_1 = (i128) i64x2_widening_add(v01, v3_shr_1);
        t8 += rhi7;

        res02 = i64x2_add(res02, v00_add_v3_shr_0);
        res13 = i64x2_add(res13, v20_add_10);
        res46 = i64x2_add(res46, v01_add_v3_shr_1);
        res57 = i64x2_add(res57, v21_add_11);

        // Partial carry
        carry = t[0] >> BITS_PER_LIMB;
        t[0] &= LIMB_MASK;
        t[1] += carry;

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

        for (int j = 0; j < NUM_LIMBS; j++) {
            t[j] = t[j+1];
        }
        t[NUM_LIMBS] = 0;

        //print_i128(res02); printf("\n");
        //print_i128(res13); printf("\n");
        //print_i128(res46); printf("\n");
        //print_i128(res57); printf("\n");
        //printf("t8 = %016lx\n", t8);
        //for (int j = 0; j < NUM_LIMBS + 1; j ++) {
            //printf("t[%d] = %016lx\n", j, t[j]);
        //}
    }
    carry_propagate(t, NUM_LIMBS+1);

    uint64_t rr[NUM_LIMBS + 1] = {0};
             rr[0] = i64x2_extract_l(res02);
             rr[2] = i64x2_extract_h(res02);
             rr[1] = i64x2_extract_l(res13);
             rr[3] = i64x2_extract_h(res13);
             rr[4] = i64x2_extract_l(res46);
             rr[6] = i64x2_extract_h(res46);
             rr[5] = i64x2_extract_l(res57);
             rr[7] = i64x2_extract_h(res57);

    carry_propagate(rr, NUM_LIMBS+1);

    for (int i = 0; i < NUM_LIMBS; i ++) {
        t[i] = rr[i];
    }
}


BigInt mont_mul(
    BigInt *ar,
    BigInt *br,
    BigInt *p,
    uint64_t n0
) {
    uint64_t t[NUM_LIMBS + 1] = {0};

    mont_mul_no_reduce(ar, br, p, n0, t);

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
