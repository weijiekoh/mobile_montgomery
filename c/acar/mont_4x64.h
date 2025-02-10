#include "../arith_uint128.h"

void mont_mul_no_reduce(
    BigInt *ar,
    BigInt *br,
    BigInt *p,
    uint64_t n0,
    uint64_t *t
) {
    uint128_t r;
    uint64_t c = 0;
    uint64_t s = 0;
    uint64_t m = 0;

    for (int i = 0; i < NUM_LIMBS; i ++) {
        c = 0;

        for (int j = 0; j < NUM_LIMBS; j ++) {
            r = abcd(t[j], ar->v[i], br->v[j], c);
            c = hi(r);
            t[j] = lo(r);
        }

        // (c, s) = t[NUM_LIMBS] + c
        r = add(t[NUM_LIMBS], c);
        c = hi(r);
        s = lo(r);
        t[NUM_LIMBS] = s;
        t[NUM_LIMBS + 1] = c;

        // m = t[0] * n0 mod 2^w
        m = t[0] * n0;

        r = abc(m, p->v[0], t[0]);
        c = hi(r);
        s = lo(r);

        for (int j = 1; j < NUM_LIMBS; j ++) {
            r = abcd(t[j], m, p->v[j], c);
            c = hi(r);
            s = lo(r);
            t[j - 1] = s;
        }
        // (c, s) = t[NUM_LIMBS] + c
        r = add(t[NUM_LIMBS], c);
        c = hi(r);
        s = lo(r);

        t[NUM_LIMBS - 1] = s;
        t[NUM_LIMBS] = t[NUM_LIMBS + 1] + c;
    }
}

/// Amine Mrabet, Nadia El-Mrabet, Ronan Lashermes, Jean-Baptiste Rigaud, Belgacem Bouallegue, et
/// al.. High-performance Elliptic Curve Cryptography by Using the CIOS Method for Modular
/// Multiplication. CRiSIS 2016, Sep 2016, Roscoff, France. hal-01383162
/// https://inria.hal.science/hal-01383162/document , page 4
/// Also see Acar, 1996.
/// This is the "classic" CIOS algorithm.
/// Does not implement the gnark optimisation (https://hackmd.io/@gnark/modular_multiplication),
/// but that should be useful.
/// Does not use SIMD instructions.
BigInt mont_mul(
    BigInt *ar,
    BigInt *br,
    BigInt *p,
    uint64_t n0
) {
    uint64_t t[NUM_LIMBS + 2] = {0};

    mont_mul_no_reduce(ar, br, p, n0, t);

    bool t_gt_p = false;
    for (int idx = 0; idx < NUM_LIMBS + 1; idx ++) {
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

    // if t <= p, return t
    if (!t_gt_p) {
        BigInt res = bigint_new();
        for (int i = 0; i < NUM_LIMBS; i ++) {
            res.v[i] = t[i];
        }
        return res;
    }

    // if t > p, return t - p
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
