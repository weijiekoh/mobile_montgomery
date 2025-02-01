/*
 * Returns a new BigInt initialized to zero.
 */
BigInt bigint_new() {
    BigInt result = {{0, 0, 0, 0, 0, 0, 0, 0}}; // Initialize all elements to zero
    return result;
}

/*
 * Returns true if each limb of a and b are equal. Otherwise, returns false.
 */
bool bigint_eq(const BigInt *a, const BigInt *b) {
    for (int i = 0; i < NUM_LIMBS; i++) {
        if (a->v[i] != b->v[i]) {
            return false;
        }
    }
    return true;
}

bool bigint_gt(
    BigInt* a,
    BigInt *b
) {
    int i;
    for (int idx = 0; idx < NUM_LIMBS; idx ++) {
        i = NUM_LIMBS - 1 - idx;
        if (a->v[i] < b->v[i]) {
            return false;
        } else if (a->v[i] > b->v[i]) {
            return true;
        }
    }
    return true;
}

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

