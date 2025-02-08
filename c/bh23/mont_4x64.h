#include "../arith_uint128.h"

void mont_mul_no_reduce(
    BigInt *ar,
    BigInt *br,
    BigInt *p,
    uint64_t n0,
    uint64_t *t
) {
    uint64_t c = 0;
    uint64_t s = 0;
    uint64_t m = 0;
    uint128_t r;

    for (int i = 0; i < NUM_LIMBS; i ++) {
        c = 0;

        for (int j = 0; j < NUM_LIMBS; j ++) {
            r = abcd(t[j], ar->v[j], br->v[i], c);
            c = r.hi;
            t[j] = r.lo;
        }
        t[NUM_LIMBS] = c;

        c = 0;
        // m = t[0] * n0 mod 2^w
        m = t[0] * n0;

        r = abc(m, p->v[0], t[0]);
        c = r.hi;

        for (int j = 1; j < NUM_LIMBS; j ++) {
            r = abcd(t[j], m, p->v[j], c);
            c = r.hi;
            s = r.lo;
            t[j - 1] = s;
        }
        t[NUM_LIMBS - 1] = t[NUM_LIMBS] + c;
    }
    t[NUM_LIMBS] = 0;
}

/// Gautam Botrel and Youssef El Housni. Faster Montgomery multiplication and
/// Multi-Scalar-Multiplication for SNARKs. IACR Transactions on Cryptographic
/// Hardware and Embedded Systems ISSN 2569-2925, Vol. 2023, No. 3, pp.
/// 504–521. DOI:10.46586/tches.v2023.i3.504-521
/// https://tches.iacr.org/index.php/TCHES/article/view/10972/10279
/// This is Acar's CIOS algorithm with the "gnark optimisation":
/// (https://hackmd.io/@gnark/modular_multiplication),
/// Does not use SIMD instructions.
BigInt mont_mul(
    BigInt *ar,
    BigInt *br,
    BigInt *p,
    uint64_t n0
) {
    uint64_t t[NUM_LIMBS + 1] = {0};

    mont_mul_no_reduce(ar, br, p, n0, t);

    uint128_t r;

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

    for (int i = 0; i < NUM_LIMBS + 1; i ++) {
        uint64_t lhs_limb = t_wide[i];
        uint64_t rhs_limb = 0;
        if (i < NUM_LIMBS) {
            rhs_limb = p->v[i];
        }
        r = sub_3(lhs_limb, rhs_limb, borrow);
        result[i] = r.lo;
        borrow = r.hi & 1;
    }

    BigInt res;
    res = bigint_new();
    for (int i = 0; i < NUM_LIMBS; i ++) {
        res.v[i] = result[i];
    }
    return res;
}
