/// Returns the higher 32 bits.
static inline uint64_t hi(uint64_t v) {
    return v >> BITS_PER_LIMB;
}

/// Returns the lower 32 bits.
static inline uint64_t lo(uint64_t v) {
    return v & LIMB_MASK;
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

    for (int i = 0; i < NUM_LIMBS; i ++) {
        uint64_t c = 0;
        uint64_t cs;
        for (int j = 0; j < NUM_LIMBS; j ++) {
            cs = t[j] + ar->v[j] * br->v[i] + c;
            c = hi(cs);
            t[j] = lo(cs);
        }
        t[NUM_LIMBS] = c;
        c = 0;

        uint64_t m = (t[0] * n0) & LIMB_MASK;
        cs = t[0] + m * p->v[0];
        c = hi(cs);

        for (int j = 1; j < NUM_LIMBS; j ++) {
            cs = t[j] + m * p->v[j] + c;
            c = hi(cs);
            t[j - 1] = lo(cs);
        }

        t[NUM_LIMBS - 1] = t[NUM_LIMBS] + c;
    }
    t[NUM_LIMBS] = 0;

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

