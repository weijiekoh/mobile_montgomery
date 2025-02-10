typedef unsigned __int128 uint128_t;

/*
 * Computes a + (b * c) + d.
 */
static inline uint128_t abcd(uint64_t a, uint64_t b, uint64_t c, uint64_t d) {
    return (uint128_t)b * c + a + d;
}

/*
 * Computes (a * b) + c.
 */
static inline uint128_t abc(uint64_t a, uint64_t b, uint64_t c) {
    return (uint128_t)a * b + c;
}

/*
 * Computes a + b.
 */
static inline uint128_t add(uint64_t a, uint64_t b) {
    return (uint128_t)a + b;
}

/*
 * Returns the higher 64 bits of x.
 */
static inline uint64_t hi(uint128_t x) {
    return x >> 64;
}

/*
 * Returns the lower 64 bits of x.
 */
static inline uint64_t lo(uint128_t x) {
    return x & 0xffffffffffffffff;
}
