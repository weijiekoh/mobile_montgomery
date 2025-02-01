// 8 x 32-bit limbs in little-endian form.
// We use 64-bit variables to store 32-bit limbs because the algorithm we use
// to perform multiplication requires us to compute 64-bit limb products.
typedef struct {
    uint64_t v[NUM_LIMBS];
} BigInt;
