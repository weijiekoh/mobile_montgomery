#include <string.h>
#include <inttypes.h>

/*
 * Convert a BigInt to a 65-character big-endian hexadecimal string, with a null terminator.
 */
char *bigint_to_hex(BigInt *bigint) {
    // We’ll build a 256-bit number in four 64-bit words.
    // words[0] is the least-significant 64 bits.
    uint64_t words[4] = {0, 0, 0, 0};
    
    // For each limb, add it at the proper bit-offset.
    for (int i = 0; i < NUM_LIMBS; i++) {
        // Since the limb is stored in a double we assume it exactly represents a 51‐bit integer.
        uint64_t limb = (uint64_t)bigint->v[i];
        int shift = i * BITS_PER_LIMB;
        int word_index = shift / 64;       // which 64-bit word to start in
        int bit_offset = shift % 64;         // bit offset within that word
        
        // Because the limbs are non-overlapping (51 bits each) we can simply “OR” the pieces.
        words[word_index] |= (limb << bit_offset);
        
        // If the limb crosses a 64-bit boundary, add the overflow bits into the next word.
        if (bit_offset > 64 - BITS_PER_LIMB) {
            words[word_index + 1] |= (limb >> (64 - bit_offset));
        }
    }
    
    // Now convert the 256-bit integer (in little-endian words) into a 64-digit hexadecimal string.
    // The hex string is in big-endian order, so we print the most significant word first.
    // We use a static buffer (not thread-safe) of 65 bytes (64 hex digits plus null terminator).
    static char hex_str[65];
    sprintf(hex_str, "%016" PRIx64 "%016" PRIx64 "%016" PRIx64 "%016" PRIx64,
            words[3], words[2], words[1], words[0]);
    // Make sure the string is null terminated.
    hex_str[64] = '\0';
    
    return hex_str;
}

/*
 * Convert a 64-character big-endian hexadecimal string to a BigInt.
 *
 * Returns 0 on success, or a negative error code.
 */
int bigint_from_hex(const char *hex_str, BigInt *val) {
    if (!hex_str || !val) return -1; // Null pointer error

    // Check that the string length is exactly 64 characters.
    if (strlen(hex_str) != 64) {
        printf("strlen: %lu\n", (unsigned long)strlen(hex_str));
        return -2; // Invalid length
    }
    
    // Parse the 64-digit hex string into four 64-bit words.
    // The string is big-endian so the first 16 digits are the most significant word.
    uint64_t words[4];
    if (sscanf(hex_str, "%16" SCNx64 "%16" SCNx64 "%16" SCNx64 "%16" SCNx64,
               &words[3], &words[2], &words[1], &words[0]) != 4) {
        return -3; // Parsing error
    }
    
    // Now “unpack” the 256-bit number into five 51-bit limbs.
    // Our bit layout (with bit 0 the least-significant bit) is:
    //   limb0: bits 0..50
    //   limb1: bits 51..101
    //   limb2: bits 102..152
    //   limb3: bits 153..203
    //   limb4: bits 204..254
    //
    // We extract the limbs by masking and shifting appropriately.
    uint64_t limb0 = words[0] & LIMB_MASK;
    
    // limb1: lower 13 bits come from words[0] (bits 51..63),
    // and the upper 38 bits come from the low 38 bits of words[1].
    uint64_t limb1 = (words[0] >> 51) | ((words[1] & ((1ULL << 38) - 1)) << 13);
    
    // limb2: remaining 51 bits starting at bit 102.
    // That is: take the high (64-38)=26 bits of words[1] and the low 25 bits of words[2].
    uint64_t limb2 = (words[1] >> 38) | ((words[2] & ((1ULL << 25) - 1)) << 26);
    
    // limb3: bits 153..203.
    // From words[2]: start at bit 25 (since 64-25=39 bits available), then need 12 more bits from words[3].
    uint64_t limb3 = (words[2] >> 25) | ((words[3] & ((1ULL << 12) - 1)) << 39);
    
    // limb4: bits 204..254, which are just the high bits of words[3].
    uint64_t limb4 = (words[3] >> 12); // Only 51 bits are used.
    
    // Now store the limbs (converted to double) into the BigInt.
    val->v[0] = (double)limb0;
    val->v[1] = (double)limb1;
    val->v[2] = (double)limb2;
    val->v[3] = (double)limb3;
    val->v[4] = (double)limb4;
    
    return 0; // Success
}
