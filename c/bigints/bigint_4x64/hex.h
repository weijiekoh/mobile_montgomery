#include <string.h>

/*
 * Convert a BigInt to a 65-character big-endian hexadecimal string, with a null terminator.
 */
char* bigint_to_hex(const BigInt *val) {
    static char hex_str[65];  // static buffer

    long long mask = 0xFFFFFFFFULL;
    for (int i = 0; i < NUM_LIMBS; i++) {
        uint32_t limb = (uint32_t)(val->v[NUM_LIMBS - 1 - i] & mask);
        sprintf(hex_str + i * NUM_LIMBS, "%08x", limb);
    }

    hex_str[64] = '\0';
    return hex_str;
}

// Helper function to convert 8 hex characters to a uint32_t
int hex_to_uint32(const char *hex_str, uint32_t *value) {
    uint32_t result = 0;
    for (int i = 0; i < 8; i++) {
        char c = hex_str[i];
        uint8_t digit;

        if (c >= '0' && c <= '9') {
            digit = (uint8_t)(c - '0');
        } else if (c >= 'a' && c <= 'f') {
            digit = (uint8_t)(c - 'a' + 10);
        } else if (c >= 'A' && c <= 'F') {
            digit = (uint8_t)(c - 'A' + 10);
        } else {
            return -1; // Invalid character
        }

        result = (result << 4) | digit;
    }
    *value = result;
    return 0;
}

/*
 * Convert a 64-character big-endian hexadecimal string to a BigInt.
 */
// Function to convert hex string to BigInt
int bigint_from_hex(const char *hex_str, BigInt *val) {
    if (!hex_str || !val) return -1; // Null pointer error

    // Check that the string length is exactly 64 characters
    if (strlen(hex_str) != 64) {
        printf("strlen: %lu\n", strlen(hex_str));

        return -2; // Invalid length
    }

    // Process each limb
    for (int i = 0; i < NUM_LIMBS; i++) {
        // Get the substring for this limb
        const char *limb_str = hex_str + i * 8;
        uint32_t limb_value;

        // Convert the 8-character hex substring to a uint32_t
        if (hex_to_uint32(limb_str, &limb_value) != 0) {
            return -3; // Invalid hex character encountered
        }

        // Since limbs are stored in little-endian order, assign in reverse
        val->v[NUM_LIMBS - 1 - i] = (uint64_t)limb_value;
    }

    return 0; // Success
}
