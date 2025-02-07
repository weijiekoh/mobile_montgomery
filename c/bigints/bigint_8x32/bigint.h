#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define NUM_LIMBS 8
#define BITS_PER_LIMB 32
#define LIMB_MASK 0xFFFFFFFF

// Includes all the BigInt-related functions (excluding Montgomery
// multiplication). To use them, just include this file.
#include "../struct_u64.h"
#include "../arith.h"

// TODO: declare this more elegantly in arith.h
BigInt bigint_sub(
    BigInt* a,
    BigInt *b
) {
    BigInt res = bigint_new();
    uint64_t borrow = 0;

    uint64_t diff;

    for (int i = 0; i < NUM_LIMBS; i ++) {
        diff = a->v[i] - b->v[i] - borrow;
        res.v[i] = diff & LIMB_MASK;
        borrow = (diff >> BITS_PER_LIMB) & 1;
    }

    return res;
}
