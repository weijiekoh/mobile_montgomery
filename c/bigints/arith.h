/*
 * Returns a new BigInt initialized to zero.
 */
BigInt bigint_new() {
    BigInt result;
    for (int i = 0; i < NUM_LIMBS; i++) {
        result.v[i] = 0;
    }
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
    BigInt* b
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
