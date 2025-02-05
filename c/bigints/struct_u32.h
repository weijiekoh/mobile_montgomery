// NUM_LIMBS x LIMB_SIZE -bit limbs in little-endian form.
// Each limb is stored in a 32-bit variable.
typedef struct {
    uint32_t v[NUM_LIMBS];
} BigInt;

