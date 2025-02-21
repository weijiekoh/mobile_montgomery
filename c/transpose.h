#include <stdint.h>

// Bit-reverse the lowest nbits of x.
static inline int bit_reverse(int x, int nbits) {
    int rev = 0;
    for (int i = 0; i < nbits; i++) {
        rev = (rev << 1) | (x & 1);
        x >>= 1;
    }
    return rev;
}

// This function takes an input array of 8 uint64_t values (each packing a high and low 32-bit limb)
// and writes into out_hi and out_lo two arrays of 8 uint32_t values each containing the transposed limbs.
// The bit-reversal permutation (on 4-bit indices) is applied on the 16 unpacked 32-bit values so that:
//
//   transposed[0]  = hi0
//   transposed[1]  = hi4
//   transposed[2]  = hi2
//   transposed[3]  = hi6
//   transposed[4]  = hi1
//   transposed[5]  = hi5
//   transposed[6]  = hi3
//   transposed[7]  = hi7
//   transposed[8]  = lo0
//   transposed[9]  = lo4
//   transposed[10] = lo2
//   transposed[11] = lo6
//   transposed[12] = lo1
//   transposed[13] = lo5
//   transposed[14] = lo3
//   transposed[15] = lo7
//
// Finally, the first 8 elements become the hi parts and the last 8 become the lo parts.
void transpose_into_hi_and_lo(
        uint64_t arr[8],
        uint64_t out_hi[8],
        uint64_t out_lo[8]
    ) {
    // Unpack the 8 uint64_t values into 16 uint32_t values.
    uint32_t tmp[16];
    tmp[0]  = (uint32_t)(arr[0] >> 32);  // hi0
    tmp[1]  = (uint32_t)(arr[0]);        // lo0
    tmp[2]  = (uint32_t)(arr[1] >> 32);  // hi1
    tmp[3]  = (uint32_t)(arr[1]);        // lo1
    tmp[4]  = (uint32_t)(arr[2] >> 32);  // hi2
    tmp[5]  = (uint32_t)(arr[2]);        // lo2
    tmp[6]  = (uint32_t)(arr[3] >> 32);  // hi3
    tmp[7]  = (uint32_t)(arr[3]);        // lo3
    tmp[8]  = (uint32_t)(arr[4] >> 32);  // hi4
    tmp[9]  = (uint32_t)(arr[4]);        // lo4
    tmp[10] = (uint32_t)(arr[5] >> 32);  // hi5
    tmp[11] = (uint32_t)(arr[5]);        // lo5
    tmp[12] = (uint32_t)(arr[6] >> 32);  // hi6
    tmp[13] = (uint32_t)(arr[6]);        // lo6
    tmp[14] = (uint32_t)(arr[7] >> 32);  // hi7
    tmp[15] = (uint32_t)(arr[7]);        // lo7

    // Apply the bit-reversal permutation on the 16 values.
    // We'll store the result in a temporary array "transposed".
    uint32_t transposed[16];
    for (int i = 0; i < 16; i++) {
        int j = bit_reverse(i, 4);
        transposed[j] = tmp[i];
    }

    // The first 8 values become the high parts, and the last 8 become the low parts.
    for (int i = 0; i < 8; i++) {
        out_hi[i] = transposed[i];
        out_lo[i] = transposed[i + 8];
    }
}
