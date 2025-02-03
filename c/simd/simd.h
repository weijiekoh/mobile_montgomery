#pragma once

#ifdef __SSE4_1__
    #include "sse4.1.h"
#endif

#ifdef __ARM_NEON
    #include "neon.h"
#endif
