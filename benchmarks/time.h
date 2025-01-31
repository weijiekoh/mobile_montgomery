#include <stdio.h>
#include <stdint.h>
#include <time.h>

// Returns the current time in microseconds
double get_now_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    uint64_t milliseconds = (uint64_t)(ts.tv_sec) * 1000 +
                             (uint64_t)(ts.tv_nsec) / 1000000;
    return milliseconds;
}
