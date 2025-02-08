#if defined(__clang__)
  #define NO_OPT __attribute__((optnone))
#elif defined(__GNUC__)
  #define NO_OPT __attribute__((optimize("O0")))
#else
  #define NO_OPT
#endif

#if defined(__clang__)
  #define DO_OPT
#elif defined(__GNUC__)
  #define DO_OPT __attribute__((optimize("O3")))
#else
  #define DO_OPT
#endif

static inline __attribute__((always_inline))
uint64_t black_box(int input) {
    // This tells the compiler that `input` is being used in some unknown way,
    // preventing it from optimizing away the value.
    __asm__ volatile("" : : "r"(input) : "memory");
    return input;
}
