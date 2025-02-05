#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define NUM_LIMBS 5
#define BITS_PER_LIMB 51
#define LIMB_MASK 0X7FFFFFFFFFFFF

// Includes all the BigInt-related functions (excluding Montgomery
// multiplication). To use them, just include this file.
#include "../struct_u64.h"
#include "../arith.h"
