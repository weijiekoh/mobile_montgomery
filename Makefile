CC     = gcc
ARM_CC     = aarch64-linux-gnu-gcc
CFLAGS = -O3 -Wall
CFLAGS_SSE = $(CFLAGS) -msse4.1
# Flags for the BM17 implementation
CFLAGS_BM17 = $(CFLAGS_SSE)

all: clean mkdir tests benchmarks

clean:
	rm -rf build/*

# Tests
tests: tests_simd tests_bigints tests_acar_mont tests_bm17_mont

run_tests:
	build/tests/simd/sse4.1
	build/tests/bigints/bigint_8x32/bigint
	build/tests/acar/mont
	build/tests/bm17/mont

## tests/simd
tests_simd: tests_simd_sse4.1 tests_simd_neon

### tests/simd/sse4.1
tests_simd_sse4.1: N := sse4.1
tests_simd_sse4.1:
	mkdir -p build/tests/simd
	echo $(CC) $(CFLAGS_SSE) tests/simd/$(N).c -o build/tests/simd/$(N)
	$(CC) $(CFLAGS_SSE) tests/simd/$(N).c -o build/tests/simd/$(N)

run_tests_simd_sse4.1:
	build/tests/simd/sse4.1

### tests/simd/neon
tests_simd_neon: N := neon
tests_simd_neon:
	mkdir -p build/tests/simd
	echo $(ARM_CC) $(CFLAGS_NEON) tests/simd/$(N).c -o build/tests/simd/$(N)
	$(ARM_CC) $(CFLAGS_NEON) tests/simd/$(N).c -o build/tests/simd/$(N)

run_tests_simd_neon:
	build/tests/simd/neon


## tests/bigints
tests_bigints: tests_bigints_bigint_8x32

### tests/bigints/bigint_8x32
tests_bigints_bigint_8x32: N := bigint
tests_bigints_bigint_8x32:
	mkdir -p build/tests/bigints/bigint_8x32
	echo $(CC) $(CFLAGS) tests/bigints/bigint_8x32/$(N).c -o build/tests/bigints/bigint_8x32/$(N)
	$(CC) $(CFLAGS) tests/bigints/bigint_8x32/$(N).c -o build/tests/bigints/bigint_8x32/$(N)

run_tests_bigints_bigint_8x32:
	build/tests/bigints/bigint_8x32/bigint

## tests/acar/mont
tests_acar_mont: N := mont
tests_acar_mont:
	mkdir -p build/tests/acar
	echo $(CC) $(CFLAGS) tests/acar/$(N).c -o build/tests/acar/$(N)
	$(CC) $(CFLAGS) tests/acar/$(N).c -o build/tests/acar/$(N)

run_tests_acar_mont:
	build/tests/acar/mont

## tests/bm17/mont
tests_bm17_mont: N := mont
tests_bm17_mont:
	mkdir -p build/tests/bm17
	echo $(CC) $(CFLAGS_BM17) tests/bm17/$(N).c -o build/tests/bm17/$(N)
	$(CC) $(CFLAGS_BM17) tests/bm17/$(N).c -o build/tests/bm17/$(N)

run_tests_bm17_mont:
	build/tests/bm17/mont

# Benchmarks
benchmarks: benchmarks_acar benchmarks_bm17

run_benchmarks:
	build/benchmarks/acar/benchmark
	build/benchmarks/bm17/benchmark

## Acar
benchmarks_acar: N := benchmark
benchmarks_acar:
	mkdir -p build/benchmarks/acar
	echo $(CC) $(CFLAGS) benchmarks/acar/$(N).c -o build/benchmarks/acar/$(N)
	$(CC) $(CFLAGS) benchmarks/acar/$(N).c -o build/benchmarks/acar/$(N)

run_benchmarks_acar:
	build/benchmarks/acar/benchmark

## BM17
benchmarks_bm17: N := benchmark
benchmarks_bm17:
	mkdir -p build/benchmarks/bm17
	echo $(CC) $(CFLAGS_BM17) benchmarks/bm17/$(N).c -o build/benchmarks/bm17/$(N)
	$(CC) $(CFLAGS_BM17) benchmarks/bm17/$(N).c -o build/benchmarks/bm17/$(N)

run_benchmarks_bm17:
	build/benchmarks/bm17/benchmark

%:
	@:
