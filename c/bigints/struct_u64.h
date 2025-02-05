// NUM_LIMBS x LIMB_SIZE -bit limbs in little-endian form.
// Each limb is stored in a 64-bit variable.
typedef struct {
    uint64_t v[NUM_LIMBS];
} BigInt;
