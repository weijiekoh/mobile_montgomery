#include <stdint.h>

// Ported from
// https://github.com/ingonyama-zk/ingo_skyscraper/blob/b39705c89e81dd8623aa06ecd0a86045efa9bfab/src/mul_cios_opt.rs
static inline unsigned __int128 carrying_mul_add_slim(uint64_t lhs, uint64_t rhs, uint64_t add) {
    unsigned __int128 wide = (unsigned __int128)lhs * (unsigned __int128)rhs;
    uint64_t slim = (uint64_t)wide + add;
    uint64_t high = (uint64_t)(wide >> 64);
    unsigned __int128 result =
        ((unsigned __int128)high << 64) | (unsigned __int128)slim;

    return result;
}

static inline unsigned __int128 carrying_mul_add(uint64_t lhs, uint64_t rhs, uint64_t add, uint64_t carry) {
    unsigned __int128 wide = (unsigned __int128)lhs * (unsigned __int128)rhs;
    wide += (unsigned __int128)add + (unsigned __int128)carry;
    return wide;
}

void mont_mul_no_reduce(
    BigInt *ar,
    BigInt *br,
    BigInt *p,
    uint64_t n0,
    uint64_t *res
) {
    uint64_t car1;
    uint64_t car2;
    uint64_t m;
    unsigned __int128 r;

    for (int i = 0; i < NUM_LIMBS; i ++) {
        if (i == 0) {
            r = carrying_mul_add_slim(ar->v[0], br->v[i], res[0]);
        } else {
            r = carrying_mul_add(ar->v[0], br->v[i], res[0], 0);
        }
        res[0] = r & LIMB_MASK;
        car2 = r >> BITS_PER_LIMB;

        m = res[0] * n0;

        r = carrying_mul_add(m, p->v[0], res[0], 0);
        car1 = r >> BITS_PER_LIMB;

        for (int j = 1; j < NUM_LIMBS; j ++) {
            unsigned __int128 temp_mult = (unsigned __int128)ar->v[j] * (unsigned __int128)br->v[i];
            unsigned __int128 temp_res;
            if (i == 0) {
                uint64_t temp_add = res[j] + car2;

                temp_res = temp_mult + (unsigned __int128) temp_add;
            } else {
                unsigned __int128 temp_add = (unsigned __int128) res[j] + (unsigned __int128) car2;
                temp_res = temp_mult + temp_add;
            }
            res[j] = temp_res & LIMB_MASK;
            car2 = temp_res >> BITS_PER_LIMB;

            r = carrying_mul_add(m, p->v[j], res[j], car1);
            res[j - 1] = r & LIMB_MASK;
            car1 = r >> BITS_PER_LIMB;
        }
        r = (unsigned __int128)car1 + (unsigned __int128)car2;
        res[NUM_LIMBS - 1] = r & LIMB_MASK;
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

    // If t < p, return t
    if (!t_gt_p) {
        BigInt res = bigint_new();
        for (int i = 0; i < NUM_LIMBS; i ++) {
            res.v[i] = t[i];
        }
        return res;
    }

    uint64_t t_wide[NUM_LIMBS + 1] = {0};
    for (int i = 0; i < NUM_LIMBS + 1; i ++) {
        t_wide[i] = t[i];
    }

    uint64_t result[NUM_LIMBS + 1] = {0};
    uint64_t borrow = 0;

    for (int i = 0; i < NUM_LIMBS; i++) {
        result[i] = t_wide[i] - p->v[i] - borrow;
        borrow = (t_wide[i] < (p->v[i] + borrow)) ? 1 : 0;
    }

    BigInt res;
    res = bigint_new();
    for (int i = 0; i < NUM_LIMBS; i ++) {
        res.v[i] = result[i];
    }
    return res;
}
