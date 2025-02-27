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
    uint64_t *t  // t must be an array of NUM_LIMBS+1 limbs
) {
    uint64_t prods[NUM_LIMBS] = {0};
    for (int i = 0; i < NUM_LIMBS; i++) {
        //printf("\n# Iteration i: %d\n", i);

        // Inner loop #1
        for (int j = 0; j < NUM_LIMBS; j++) {
            prods[j] = (uint64_t)ar->v[i] * (uint64_t) br->v[j];
        }

        t[0] += lo(prods[0]);
        t[1] += hi(prods[0]); t[1] += lo(prods[1]);
        t[2] += hi(prods[1]); t[2] += lo(prods[2]);
        t[3] += hi(prods[2]); t[3] += lo(prods[3]);
        t[4] += hi(prods[3]); t[4] += lo(prods[4]);
        t[5] += hi(prods[4]); t[5] += lo(prods[5]);
        t[6] += hi(prods[5]); t[6] += lo(prods[6]);
        t[7] += hi(prods[6]); t[7] += lo(prods[7]);
        t[8] += hi(prods[7]);

        // Compute m
        uint64_t m = (t[0] * n0) & LIMB_MASK;

        // Inner loop #2
        for (int j = 0; j < NUM_LIMBS; j++) {
            prods[j] = m * p->v[j];
            t[j] += lo(prods[j]);
        }

        // t[0] doesn't need to be set
        t[1] += hi(prods[0]); //t[1] += lo(prods[1]); 
        t[2] += hi(prods[1]); //t[2] += lo(prods[2]); 
        t[3] += hi(prods[2]); //t[3] += lo(prods[3]);
        t[4] += hi(prods[3]); //t[4] += lo(prods[4]); 
        t[5] += hi(prods[4]); //t[5] += lo(prods[5]);
        t[6] += hi(prods[5]); //t[6] += lo(prods[6]);
        t[7] += hi(prods[6]); //t[7] += lo(prods[7]);
        t[8] += hi(prods[7]);

        carry_propagate(t, 9);

        // Shift
        for (int j = 0; j < NUM_LIMBS; j++) {
            t[j] = t[j+1];
        }

        t[NUM_LIMBS] = 0;
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
