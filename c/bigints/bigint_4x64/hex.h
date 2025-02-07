#include <inttypes.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

/*
 * Convert a BigInt to a 65-character big-endian hexadecimal string, with a null terminator.
 */
char* bigint_to_hex(const BigInt *val) {
    static char hex_str[65];  // static buffer

    hex_str[64] = '\0';
    char *ptr = hex_str;
    // Process limbs in reverse order (most significant first)
    for (int i = NUM_LIMBS - 1; i >= 0; i--) {
        sprintf(ptr, "%016" PRIx64, val->v[i]); // Uppercase hex with padding
        ptr += 16;
    }

    return hex_str;
}

/*
 * Convert a 64-character big-endian hexadecimal string to a BigInt.
 */
int bigint_from_hex(const char *hex_str, BigInt *val) {
    if (!hex_str || !val) return -1; // Null pointer error

    // Check that the string length is exactly 64 characters
    if (strlen(hex_str) != 64) {
        printf("strlen: %lu\n", strlen(hex_str));

        return -2; // Invalid length
    }

    for (int i = 0; i < NUM_LIMBS; i++) {
        const char *start = hex_str + (3 - i) * 16; // Extract 16-char chunks
        char tmp[17];
        memcpy(tmp, start, 16);
        tmp[16] = '\0';
        
        // Validate hex characters
        for (int j = 0; j < 16; j++) {
            if (!isxdigit(tmp[j])) return -3; // Invalid hex digit
        }
        
        // Convert to uint64_t
        char *endptr;
        errno = 0;
        uint64_t limb = strtoull(tmp, &endptr, 16);
        if (endptr != tmp + 16 || errno == ERANGE) return -4; // Parse error
        val->v[i] = limb;
    }

    return 0; // Success
}
