#include <stdint.h>

typedef struct {
    uint64_t hi;  // Higher 64 bits
    uint64_t lo;  // Lower 64 bits
} uint128_t;

#ifdef __ARM_NEON
/*
 * Computes (hi, lo) = a + (b * c) + d.
 */
uint128_t abcd(uint64_t a, uint64_t b, uint64_t c, uint64_t d) {
    uint64_t lo, hi;
    __asm__ (
        "mul   %0, %2, %3\n\t"      // lo = b * c (lower 64 bits)
        "umulh %1, %2, %3\n\t"      // hi = b * c (upper 64 bits)
        "adds  %0, %0, %4\n\t"      // lo = lo + a; set condition flags
        "adc   %1, %1, xzr\n\t"     // hi = hi + (carry from adding a)
        "adds  %0, %0, %5\n\t"      // lo = lo + d; set condition flags
        "adc   %1, %1, xzr\n\t"     // hi = hi + (carry from adding d)
        : "=&r" (lo), "=&r" (hi)
        : "r" (b), "r" (c), "r" (a), "r" (d)
        : "cc"
    );
    return (uint128_t){ hi, lo };
}

/*
 * Computes (hi, lo) = a + b.
 *
 * The 65–bit result has its lower 64 bits in lo and its (carry) in hi.
 */
uint128_t add(uint64_t a, uint64_t b) {
    uint64_t lo = a;
    uint64_t hi;
    __asm__ (
        "adds %0, %0, %2\n\t"      // lo = a + b; flags set with carry if overflow
        "adc  %1, xzr, xzr\n\t"     // hi = 0 + 0 + carry (i.e. hi = carry)
        : "+r" (lo), "=r" (hi)
        : "r" (b)
        : "cc"
    );
    return (uint128_t){ hi, lo };
}

/*
 * Computes (hi, lo) = a * b.
 */
uint128_t mul(uint64_t a, uint64_t b) {
    uint64_t lo, hi;
    __asm__ (
        "mul   %0, %2, %3\n\t"     // lo = a * b (lower 64 bits)
        "umulh %1, %2, %3\n\t"     // hi = a * b (upper 64 bits)
        : "=&r" (lo), "=&r" (hi)
        : "r" (a), "r" (b)
        : "cc"
    );
    return (uint128_t){ hi, lo };
}

/*
 * Computes (hi, lo) = (a * b) + c.
 */
uint128_t abc(uint64_t a, uint64_t b, uint64_t c) {
    uint64_t lo, hi;
    __asm__ (
        "mul   %0, %2, %3\n\t"     // lo = a * b (lower 64 bits)
        "umulh %1, %2, %3\n\t"     // hi = a * b (upper 64 bits)
        "adds  %0, %0, %4\n\t"     // lo = lo + c; set condition flags
        "adc   %1, %1, xzr\n\t"    // hi = hi + (carry from addition)
        : "=&r" (lo), "=&r" (hi)
        : "r" (a), "r" (b), "r" (c)
        : "cc"
    );
    return (uint128_t){ hi, lo };
}

/*
 * Computes (hi, lo) = a - b - c.
 *
 * We first subtract b from a using "subs" (which sets flags),
 * then subtract c using "subs" and propagate the borrow via "sbc".
 * By initializing hi to 0 and then doing "sbc" with xzr,
 * hi becomes 0 if no borrow occurred, or all ones (−1 mod 2^64)
 * if a borrow happened.
 */
uint128_t sub_3(uint64_t a, uint64_t b, uint64_t c) {
    uint64_t lo = a;
    uint64_t hi = 0;
    __asm__ (
        "subs %0, %0, %2\n\t"      // lo = a - b; set condition flags
        "sbc  %1, xzr, xzr\n\t"     // hi = 0 - 0 - (1 - C); hi becomes 0 (if no borrow) or -1 (if borrow)
        "subs %0, %0, %3\n\t"      // lo = (a - b) - c; update flags
        "sbc  %1, %1, xzr\n\t"     // hi = hi - 0 - (1 - C)
        : "+r" (lo), "+r" (hi)
        : "r" (b), "r" (c)
        : "cc"
    );
    return (uint128_t){ hi, lo };
}
#endif

#ifdef __amd64__
static inline uint128_t abcd(uint64_t a, uint64_t b, uint64_t c, uint64_t d) {
    // Initialize 'lo' with 'b' because the MUL instruction
    // implicitly uses RAX (here represented by 'lo') as one operand.
    uint64_t lo = b;
    uint64_t hi = 0;  // This will receive the high 64 bits.

    __asm__ (
        /* 
         * At this point:
         *   - RAX (lo) contains b.
         *   - We want to compute: (hi, lo) = b * c,
         *     then add a and then d.
         */
        "mul %[c]         \n\t" // Unsigned multiply: RAX * c -> RDX:RAX
        "add %[a], %%rax  \n\t" // Add a to the low 64 bits (RAX); flags set.
        "adc $0, %%rdx    \n\t" // Propagate carry into high 64 bits (RDX).
        "add %[d], %%rax  \n\t" // Add d to the low 64 bits.
        "adc $0, %%rdx    \n\t" // Propagate carry into high 64 bits.
        : "+a" (lo), "+d" (hi)   // Output operands: RAX and RDX.
        : [c] "r" (c),          // Input: c in any register.
          [a] "r" (a),          // Input: a in any register.
          [d] "r" (d)           // Input: d in any register.
        : "cc"                  // Clobbers condition codes.
    );

    return (uint128_t){ .hi = hi, .lo = lo };
}


// 128-bit addition: returns (hi, lo) = a + b
uint128_t add(uint64_t a, uint64_t b) {
    uint64_t lo = a;
    uint64_t hi = 0; // Must initialize hi to 0 for proper carry propagation.
    __asm__(
        "add    %[b], %%rax \n\t" // RAX = a + b; flags updated
        "adc    $0, %%rdx   \n\t" // RDX = 0 + carry (0 or 1)
        : "+a"(lo), "+d"(hi)       // RAX is lo, RDX is hi (both read and written)
        : [b] "r"(b)              // b in any register
        : "cc"                    // Clobbers condition flags
    );
    return (uint128_t){ .hi = hi, .lo = lo };
}

// 128-bit multiplication: returns (hi, lo) = a * b
uint128_t mul(uint64_t a, uint64_t b) {
    uint64_t lo = a;
    uint64_t hi;
    __asm__(
        "mul    %[b] \n\t"        // Unsigned multiply: RAX * b -> RDX:RAX
        : "+a"(lo), "=d"(hi)        // RAX is input/output (lo); RDX is output (hi)
        : [b] "r"(b)              // b in any register
        : "cc"                    // Clobbers condition codes
    );
    return (uint128_t){ .hi = hi, .lo = lo };
}

// Computes (hi, lo) = a * b + c.
uint128_t abc(uint64_t a, uint64_t b, uint64_t c) {
    uint64_t lo = a;  // Place 'a' in RAX.
    uint64_t hi;      // RDX will receive the high 64 bits.
    
    __asm__(
        "mul    %[b]         \n\t" // Multiply RAX (a) by 'b'; result in RDX:RAX.
        "add    %[c], %%rax   \n\t" // Add 'c' to the low 64 bits in RAX.
        "adc    $0, %%rdx     \n\t" // Add carry (if any) from the addition to RDX.
        : "+a"(lo), "=d"(hi)         // Output: lo in RAX (modified), hi in RDX.
        : [b] "r"(b), [c] "r"(c)      // Inputs: b and c in any general-purpose registers.
        : "cc"                      // Clobbers the condition flags.
    );
    
    return (uint128_t){ .hi = hi, .lo = lo };
}

// Computes (hi, lo) = a - b - c.
uint128_t sub_3(uint64_t a, uint64_t b, uint64_t c) {
    uint64_t lo = a;  // Place 'a' in RAX.
    uint64_t hi = 0;  // Initialize high part to zero.
    
    __asm__(
        "sub %[b], %%rax \n\t"  // RAX = a - b; sets flags (borrow if underflow).
        "sbb $0, %%rdx   \n\t"  // RDX = 0 - borrow (0 or 1) -> hi becomes 0 or 0xFFFFFFFFFFFFFFFF.
        "sub %[c], %%rax \n\t"  // RAX = (a - b) - c; update flags.
        "sbb $0, %%rdx   \n\t"  // RDX = hi - borrow from previous subtraction.
        : "+a" (lo), "+d" (hi)   // Output operands: RAX (lo) and RDX (hi) are both read/write.
        : [b] "r" (b), [c] "r" (c)  // Input operands: b and c in any general-purpose register.
        : "cc"                  // Clobbers condition flags.
    );
    
    return (uint128_t){ .hi = hi, .lo = lo };
}
#endif

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

    uint64_t c = 0;
    uint64_t s = 0;
    uint64_t m = 0;
    uint128_t r;

    for (int i = 0; i < NUM_LIMBS; i ++) {
        c = 0;

        for (int j = 0; j < NUM_LIMBS; j ++) {
            r = abcd(t[j], ar->v[i], br->v[j], c);
            c = r.hi;
            t[j] = r.lo;
        }

        // (c, s) = t[NUM_LIMBS] + c
        r = add(t[NUM_LIMBS], c);
        c = r.hi;
        s = r.lo;
        t[NUM_LIMBS] = s;
        t[NUM_LIMBS + 1] = c;

        // m = t[0] * n0 mod 2^w
        m = t[0] * n0;

        r = abc(m, p->v[0], t[0]);
        c = r.hi;
        s = r.lo;

        for (int j = 1; j < NUM_LIMBS; j ++) {
            r = abcd(t[j], m, p->v[j], c);
            c = r.hi;
            s = r.lo;
            t[j - 1] = s;
        }
        // (c, s) = t[NUM_LIMBS] + c
        r = add(t[NUM_LIMBS], c);
        c = r.hi;
        s = r.lo;

        t[NUM_LIMBS - 1] = s;
        t[NUM_LIMBS] = t[NUM_LIMBS + 1] + c;
    }

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
