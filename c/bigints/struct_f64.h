// NUM_LIMBS x LIMB_SIZE -bit limbs in little-endian form.
// Each limb is stored in the mantissa of a 64-bit double.
typedef struct {
    double v[NUM_LIMBS];
} BigInt;
