CC     = gcc
ARM_CC     = aarch64-linux-gnu-gcc
CFLAGS = -O3 -Wall
CFLAGS_SSE = $(CFLAGS) -msse4.1
CFLAGS_NEON = $(CFLAGS) -static
EMULATOR = qemu-aarch64

# Flags for the BM17 implementation
CFLAGS_BM17 = $(CFLAGS_SSE)

all: clean mkdir tests benchmarks

clean:
	rm -rf build/*

# Tests
tests: tests_simd tests_bigints tests_acar_mont tests_acar_mont_neon tests_bh23_mont tests_bh23_mont_neon tests_bm17_mont tests_bm17_mont_neon

run_tests:
	run_tests_simd_sse4.1
	emulate_tests_simd_neon
	build/tests/bigints/bigint_8x32/bigint
	build/tests/acar/mont
	build/tests/bm17/mont


## tests/simd
tests_simd: tests_simd_sse4.1 tests_simd_neon

### tests/simd_sse4.1
tests_simd_sse4.1: N := simd_sse4.1
tests_simd_sse4.1:
	mkdir -p build/tests/
	echo $(CC) $(CFLAGS_SSE) tests/simd.c -o build/tests/$(N)
	$(CC) $(CFLAGS_SSE) tests/simd.c -o build/tests/$(N)

run_tests_simd_sse4.1:
	build/tests/simd_sse4.1

### tests/simd_neon
tests_simd_neon: N := simd_neon
tests_simd_neon:
	mkdir -p build/tests/
	echo $(ARM_CC) $(CFLAGS_NEON) tests/simd.c -o build/tests/$(N)
	$(ARM_CC) $(CFLAGS_NEON) tests/simd.c -o build/tests/$(N)

run_tests_simd_neon:
	build/tests/simd_neon

emulate_tests_simd_neon:
	$(EMULATOR) build/tests/simd_neon

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

## tests/acar/mont_neon
tests_acar_mont_neon: N := mont
tests_acar_mont_neon:
	mkdir -p build/tests/acar
	echo $(ARM_CC) $(CFLAGS_NEON) tests/acar/$(N).c -o build/tests/acar/$(N)_neon
	$(ARM_CC) $(CFLAGS_NEON) tests/acar/$(N).c -o build/tests/acar/$(N)_neon

run_tests_acar_mont_neon:
	build/tests/acar/mont_neon

## tests/bh23/mont
tests_bh23_mont: N := mont
tests_bh23_mont:
	mkdir -p build/tests/bh23
	echo $(CC) $(CFLAGS) tests/bh23/$(N).c -o build/tests/bh23/$(N)
	$(CC) $(CFLAGS) tests/bh23/$(N).c -o build/tests/bh23/$(N)

run_tests_bh23_mont:
	build/tests/bh23/mont

## tests/bh23/mont_neon
tests_bh23_mont_neon: N := mont
tests_bh23_mont_neon:
	mkdir -p build/tests/bh23
	echo $(ARM_CC) $(CFLAGS_NEON) tests/bh23/$(N).c -o build/tests/bh23/$(N)_neon
	$(ARM_CC) $(CFLAGS_NEON) tests/bh23/$(N).c -o build/tests/bh23/$(N)_neon

run_tests_bh23_mont_neon:
	build/tests/bh23/mont_neon

## tests/bm17/mont
tests_bm17_mont: N := mont
tests_bm17_mont:
	mkdir -p build/tests/bm17
	echo $(CC) $(CFLAGS_BM17) tests/bm17/$(N).c -o build/tests/bm17/$(N)
	$(CC) $(CFLAGS_BM17) tests/bm17/$(N).c -o build/tests/bm17/$(N)

run_tests_bm17_mont:
	build/tests/bm17/mont

## tests/bm17/mont_neon
tests_bm17_mont_neon: N := mont
tests_bm17_mont_neon:
	mkdir -p build/tests/bm17
	echo $(ARM_CC) $(CFLAGS_NEON) tests/bm17/$(N).c -o build/tests/bm17/$(N)_neon
	$(ARM_CC) $(CFLAGS_NEON) tests/bm17/$(N).c -o build/tests/bm17/$(N)_neon

# Benchmarks
benchmarks: benchmarks_acar benchmarks_acar_neon benchmarks_bh23 benchmarks_bh23_neon benchmarks_bm17 benchmarks_bm17_neon

run_benchmarks:
	build/benchmarks/acar/benchmark
	build/benchmarks/bm17/benchmark

run_benchmarks_neon:
	build/benchmarks/acar/benchmark_neon
	build/benchmarks/bh23/benchmark_neon
	build/benchmarks/bm17/benchmark_neon

emulate_benchmarks_neon:
	$(EMULATOR) build/benchmarks/acar/benchmark_neon
	$(EMULATOR) build/benchmarks/bm17/benchmark_neon

## Acar
benchmarks_acar: N := benchmark
benchmarks_acar:
	mkdir -p build/benchmarks/acar
	echo $(CC) $(CFLAGS) benchmarks/acar/$(N).c -o build/benchmarks/acar/$(N)
	$(CC) $(CFLAGS) benchmarks/acar/$(N).c -o build/benchmarks/acar/$(N)

run_benchmarks_acar:
	build/benchmarks/acar/benchmark

benchmarks_acar_neon: N := benchmark
benchmarks_acar_neon:
	mkdir -p build/benchmarks/acar
	echo $(ARM_CC) $(CFLAGS_NEON) benchmarks/acar/$(N).c -o build/benchmarks/acar/$(N)_neon
	$(ARM_CC) $(CFLAGS_NEON) benchmarks/acar/$(N).c -o build/benchmarks/acar/$(N)_neon

run_benchmarks_acar_neon:
	build/benchmarks/acar/benchmark_neon

# BH23
benchmarks_bh23: N := benchmark
benchmarks_bh23:
	mkdir -p build/benchmarks/bh23
	echo $(CC) $(CFLAGS) benchmarks/bh23/$(N).c -o build/benchmarks/bh23/$(N)
	$(CC) $(CFLAGS) benchmarks/bh23/$(N).c -o build/benchmarks/bh23/$(N)

run_benchmarks_bh23:
	build/benchmarks/bh23/benchmark

benchmarks_bh23_neon: N := benchmark
benchmarks_bh23_neon:
	mkdir -p build/benchmarks/bh23	
	echo $(ARM_CC) $(CFLAGS_NEON) benchmarks/bh23/$(N).c -o build/benchmarks/bh23/$(N)_neon
	$(ARM_CC) $(CFLAGS_NEON) benchmarks/bh23/$(N).c -o build/benchmarks/bh23/$(N)_neon

run_benchmarks_bh23_neon:
	build/benchmarks/bh23/benchmark_neon

## BM17
benchmarks_bm17: N := benchmark
benchmarks_bm17:
	mkdir -p build/benchmarks/bm17
	echo $(CC) $(CFLAGS_BM17) benchmarks/bm17/$(N).c -o build/benchmarks/bm17/$(N)
	$(CC) $(CFLAGS_BM17) benchmarks/bm17/$(N).c -o build/benchmarks/bm17/$(N)

run_benchmarks_bm17:
	build/benchmarks/bm17/benchmark

benchmarks_bm17_neon: N := benchmark
benchmarks_bm17_neon:
	mkdir -p build/benchmarks/bm17
	echo $(ARM_CC) $(CFLAGS_NEON) benchmarks/bm17/$(N).c -o build/benchmarks/bm17/$(N)_neon
	$(ARM_CC) $(CFLAGS_NEON) benchmarks/bm17/$(N).c -o build/benchmarks/bm17/$(N)_neon

run_benchmarks_bm17_neon:
	build/benchmarks/bm17/benchmark_neon

%:
	@:
