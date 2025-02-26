bm17     = gcc
ARM_CC     = aarch64-linux-gnu-gcc
CFLAGS = -O3 -Wall
CFLAGS_SSE = $(CFLAGS) -msse4.1
CFLAGS_NEON = $(CFLAGS) -static
EMULATOR = qemu-aarch64

# Flags for the BM17 implementation
CFLAGS_BM17 = $(CFLAGS_SSE)
CFLAGS_SLGCK14 = $(CFLAGS_SSE)

all: clean mkdir tests benchmarks

clean:
	rm -rf build/*

# Tests
tests: tests_simd tests_bigints tests_acar_mont tests_acar_mont_neon tests_acar_mont_4x64 tests_acar_mont_4x64_neon tests_bh23_mont tests_bh23_mont_neon tests_bh23_mont_4x64 tests_bh23_mont_4x64_neon tests_domb_mont_4x64 tests_domb_mont_4x64_neon tests_bm17_mont tests_bm17_mont_neon  tests_slgck14_mont_neon

run_tests:
	build/tests/simd_sse4.1
	build/tests/simd_neon
	build/tests/bigints/bigint_8x32/bigint
	build/tests/bigints/bigint_4x64/bigint
	build/tests/acar/mont
	build/tests/acar/mont_4x64
	build/tests/bm17/mont
	build/tests/bh23/mont
	build/tests/bh23/mont_4x64
	build/tests/domb/mont_4x64
	build/tests/slgck14/mont

run_tests_neon:
	build/tests/simd_neon
	build/tests/bigints/bigint_8x32/bigint_neon
	build/tests/bigints/bigint_4x64/bigint_neon
	build/tests/acar/mont_neon
	build/tests/acar/mont_4x64_neon
	build/tests/bm17/mont_neon
	build/tests/bh23/mont_neon
	build/tests/bh23/mont_4x64_neon
	build/tests/domb/mont_4x64_neon

## tests/simd
tests_simd: tests_simd_sse4.1 tests_simd_neon

### tests/simd_sse4.1
tests_simd_sse4.1: N := simd_sse4.1
tests_simd_sse4.1:
	mkdir -p build/tests/
	$(CC) $(CFLAGS_SSE) tests/simd.c -o build/tests/$(N)

run_tests_simd_sse4.1:
	build/tests/simd_sse4.1

### tests/simd_neon
tests_simd_neon: N := simd_neon
tests_simd_neon:
	mkdir -p build/tests/
	$(ARM_CC) $(CFLAGS_NEON) tests/simd.c -o build/tests/$(N)

run_tests_simd_neon:
	build/tests/simd_neon

emulate_tests_simd_neon:
	$(EMULATOR) build/tests/simd_neon

## tests/bigints
tests_bigints: tests_bigints_bigint_8x32 tests_bigints_bigint_8x32_neon tests_bigints_bigint_4x64 tests_bigints_bigint_4x64_neon tests_bigints_bigint_5x51 tests_bigints_bigint_5x51_neon

### tests/bigints/bigint_8x32
tests_bigints_bigint_8x32: N := bigint
tests_bigints_bigint_8x32:
	mkdir -p build/tests/bigints/bigint_8x32
	$(CC) $(CFLAGS) tests/bigints/bigint_8x32/$(N).c -o build/tests/bigints/bigint_8x32/$(N)

run_tests_bigints_bigint_8x32:
	build/tests/bigints/bigint_8x32/bigint

tests_bigints_bigint_8x32_neon: N := bigint
tests_bigints_bigint_8x32_neon:
	mkdir -p build/tests/bigints/bigint_8x32
	$(ARM_CC) $(CFLAGS_NEON) tests/bigints/bigint_8x32/$(N).c -o build/tests/bigints/bigint_8x32/$(N)_neon

run_tests_bigints_bigint_8x32_neon:
	build/tests/bigints/bigint_8x32/bigint_neon

### tests/bigints/bigint_4x64
tests_bigints_bigint_4x64: N := bigint
tests_bigints_bigint_4x64:
	mkdir -p build/tests/bigints/bigint_4x64
	$(CC) $(CFLAGS) tests/bigints/bigint_4x64/$(N).c -o build/tests/bigints/bigint_4x64/$(N)

run_tests_bigints_bigint_4x64:
	build/tests/bigints/bigint_4x64/bigint

tests_bigints_bigint_4x64_neon: N := bigint
tests_bigints_bigint_4x64_neon:
	mkdir -p build/tests/bigints/bigint_4x64
	$(ARM_CC) $(CFLAGS_NEON) tests/bigints/bigint_4x64/$(N).c -o build/tests/bigints/bigint_4x64/$(N)_neon

run_tests_bigints_bigint_4x64_neon:
	build/tests/bigints/bigint_4x64/bigint_neon
	
### tests/bigints/bigint_5x51
tests_bigints_bigint_5x51: N := bigint
tests_bigints_bigint_5x51:
	mkdir -p build/tests/bigints/bigint_5x51
	$(CC) $(CFLAGS) tests/bigints/bigint_5x51/$(N).c -o build/tests/bigints/bigint_5x51/$(N)

run_tests_bigints_bigint_5x51:
	build/tests/bigints/bigint_5x51/bigint

tests_bigints_bigint_5x51_neon: N := bigint
tests_bigints_bigint_5x51_neon:
	mkdir -p build/tests/bigints/bigint_5x51
	$(ARM_CC) $(CFLAGS_NEON) tests/bigints/bigint_5x51/$(N).c -o build/tests/bigints/bigint_5x51/$(N)_neon

run_tests_bigints_bigint_5x51_neon:
	build/tests/bigints/bigint_5x51/bigint_neon

## tests/acar/mont
tests_acar_mont: N := mont
tests_acar_mont:
	mkdir -p build/tests/acar
	$(CC) $(CFLAGS) tests/acar/$(N).c -o build/tests/acar/$(N)

run_tests_acar_mont:
	build/tests/acar/mont

## tests/acar/mont_4x64
tests_acar_mont_4x64: N := mont_4x64
tests_acar_mont_4x64:
	mkdir -p build/tests/acar
	$(CC) $(CFLAGS) tests/acar/$(N).c -o build/tests/acar/$(N)

run_tests_acar_mont_4x64:
	build/tests/acar/mont_4x64

emulate_tests_acar_mont_4x64:
	$(EMULATOR) build/tests/acar/mont_4x64

## tests/acar/mont_4x64_neon
tests_acar_mont_4x64_neon: N := mont_4x64
tests_acar_mont_4x64_neon:
	mkdir -p build/tests/acar
	$(ARM_CC) $(CFLAGS_NEON) tests/acar/$(N).c -o build/tests/acar/$(N)_neon

emulate_tests_acar_mont_4x64_neon:
	$(EMULATOR) build/tests/acar/mont_4x64_neon

## tests/acar/mont_neon
tests_acar_mont_neon: N := mont
tests_acar_mont_neon:
	mkdir -p build/tests/acar
	$(ARM_CC) $(CFLAGS_NEON) tests/acar/$(N).c -o build/tests/acar/$(N)_neon

run_tests_acar_mont_neon:
	build/tests/acar/mont_neon

## tests/bh23/mont
tests_bh23_mont: N := mont
tests_bh23_mont:
	mkdir -p build/tests/bh23
	$(CC) $(CFLAGS) tests/bh23/$(N).c -o build/tests/bh23/$(N)

run_tests_bh23_mont:
	build/tests/bh23/mont

## tests/bh23/mont_4x64
tests_bh23_mont_4x64: N := mont_4x64
tests_bh23_mont_4x64:
	mkdir -p build/tests/bh23
	$(CC) $(CFLAGS) tests/bh23/$(N).c -o build/tests/bh23/$(N)

run_tests_bh23_mont_4x64:
	build/tests/bh23/mont_4x64

tests_bh23_mont_4x64_neon: N := mont_4x64
tests_bh23_mont_4x64_neon:
	mkdir -p build/tests/bh23
	$(ARM_CC) $(CFLAGS_NEON) tests/bh23/$(N).c -o build/tests/bh23/$(N)_neon

emulate_tests_bh23_mont_4x64_neon:
	$(EMULATOR) build/tests/bh23/mont_4x64_neon

run_tests_bh23_mont_4x64_neon:
	build/tests/bh23/mont_4x64_neon

## tests/bh23/mont_neon
tests_bh23_mont_neon: N := mont
tests_bh23_mont_neon:
	mkdir -p build/tests/bh23
	$(ARM_CC) $(CFLAGS_NEON) tests/bh23/$(N).c -o build/tests/bh23/$(N)_neon

emulate_tests_bh23_mont_neon:
	$(EMULATOR) build/tests/bh23/mont_neon

run_tests_bh23_mont_neon:
	build/tests/bh23/mont_neon

# tests/domb/mont_4x64
tests_domb_mont_4x64: N := mont_4x64
tests_domb_mont_4x64:
	mkdir -p build/tests/domb
	$(CC) $(CFLAGS) tests/domb/$(N).c -o build/tests/domb/$(N)

run_tests_domb_mont_4x64:
	build/tests/domb/mont_4x64

# tests/domb/mont_4x64_neon
tests_domb_mont_4x64_neon: N := mont_4x64
tests_domb_mont_4x64_neon:
	mkdir -p build/tests/domb
	$(ARM_CC) $(CFLAGS_NEON) tests/domb/$(N).c -o build/tests/domb/$(N)_neon

run_tests_domb_mont_4x64_neon:
	build/tests/domb/mont_4x64_neon

emulate_tests_domb_mont_4x64_neon:
	$(EMULATOR) build/tests/domb/mont_4x64_neon

## tests/bm17/mont
tests_bm17_mont: N := mont
tests_bm17_mont:
	mkdir -p build/tests/bm17
	$(CC) $(CFLAGS_BM17) tests/bm17/$(N).c -o build/tests/bm17/$(N)

run_tests_bm17_mont:
	build/tests/bm17/mont

## tests/bm17/mont_neon
tests_bm17_mont_neon: N := mont
tests_bm17_mont_neon:
	mkdir -p build/tests/bm17
	$(ARM_CC) $(CFLAGS_NEON) tests/bm17/$(N).c -o build/tests/bm17/$(N)_neon

emulate_tests_bm17_mont_neon:
	$(EMULATOR) build/tests/bm17/mont_neon

run_tests_bm17_mont_neon:
	build/tests/bm17/mont_neon

### tests/slgck14/mont
#tests_slgck14_mont: N := mont
#tests_slgck14_mont:
	#mkdir -p build/tests/slgck14
	#$(CC) $(CFLAGS_SLGCK14) tests/slgck14/$(N).c -o build/tests/slgck14/$(N)

#run_tests_slgck14_mont:
	#build/tests/slgck14/mont

## tests/slgck14/mont_neon
tests_slgck14_mont_neon: N := mont
tests_slgck14_mont_neon:
	mkdir -p build/tests/slgck14
	$(ARM_CC) $(CFLAGS_NEON) tests/slgck14/$(N).c -o build/tests/slgck14/$(N)_neon

emulate_tests_slgck14_mont_neon:
	$(EMULATOR) build/tests/slgck14/mont_neon

# Benchmarks
benchmarks: benchmarks_acar benchmarks_acar_neon benchmarks_acar_4x64 benchmarks_acar_4x64_neon benchmarks_bh23 benchmarks_bh23_neon benchmarks_bh23_4x64 benchmarks_bh23_4x64_neon benchmarks_domb_4x64 benchmarks_domb_4x64_neon benchmarks_bm17 benchmarks_bm17_neon benchmarks_slgck14_neon

run_benchmarks:
	build/benchmarks/acar/benchmark
	build/benchmarks/acar/benchmark_4x64
	build/benchmarks/bm17/benchmark
	build/benchmarks/bh23/benchmark
	build/benchmarks/bh23/benchmark_4x64
	build/benchmarks/domb/benchmark_4x64

run_benchmarks_neon:
	build/benchmarks/acar/benchmark_neon
	build/benchmarks/acar/benchmark_4x64_neon
	build/benchmarks/bh23/benchmark_neon
	build/benchmarks/bh23/benchmark_4x64_neon
	build/benchmarks/domb/benchmark_4x64_neon
	build/benchmarks/bm17/benchmark_neon
	build/benchmarks/slgck14/benchmark_neon

emulate_benchmarks_neon:
	$(EMULATOR) build/benchmarks/acar/benchmark_neon
	$(EMULATOR) build/benchmarks/acar/benchmark_4x64_neon
	$(EMULATOR) build/benchmarks/bh23/benchmark_neon
	$(EMULATOR) build/benchmarks/bh23/benchmark_4x64_neon
	$(EMULATOR) build/benchmarks/domb/benchmark_4x64_neon
	$(EMULATOR) build/benchmarks/bm17/benchmark_neon
	$(EMULATOR) build/benchmarks/slgck14/benchmark_neon

## Acar
benchmarks_acar: N := benchmark
benchmarks_acar:
	mkdir -p build/benchmarks/acar
	$(CC) $(CFLAGS) benchmarks/acar/$(N).c -o build/benchmarks/acar/$(N)

run_benchmarks_acar:
	build/benchmarks/acar/benchmark

benchmarks_acar_neon: N := benchmark
benchmarks_acar_neon:
	mkdir -p build/benchmarks/acar
	$(ARM_CC) $(CFLAGS_NEON) benchmarks/acar/$(N).c -o build/benchmarks/acar/$(N)_neon

run_benchmarks_acar_neon:
	build/benchmarks/acar/benchmark_neon

benchmarks_acar_4x64: N := benchmark_4x64
benchmarks_acar_4x64:
	mkdir -p build/benchmarks/acar
	$(CC) $(CFLAGS) benchmarks/acar/$(N).c -o build/benchmarks/acar/$(N)

run_benchmarks_acar_4x64:
	build/benchmarks/acar/benchmark_4x64

benchmarks_acar_4x64_neon: N := benchmark_4x64
benchmarks_acar_4x64_neon:
	mkdir -p build/benchmarks/acar
	$(ARM_CC) $(CFLAGS_NEON) benchmarks/acar/$(N).c -o build/benchmarks/acar/$(N)_neon

run_benchmarks_acar_4x64_neon:
	build/benchmarks/acar/benchmark_4x64_neon

# BH23
benchmarks_bh23: N := benchmark
benchmarks_bh23:
	mkdir -p build/benchmarks/bh23
	$(CC) $(CFLAGS) benchmarks/bh23/$(N).c -o build/benchmarks/bh23/$(N)

run_benchmarks_bh23:
	build/benchmarks/bh23/benchmark

benchmarks_bh23_neon: N := benchmark
benchmarks_bh23_neon:
	mkdir -p build/benchmarks/bh23	
	$(ARM_CC) $(CFLAGS_NEON) benchmarks/bh23/$(N).c -o build/benchmarks/bh23/$(N)_neon

run_benchmarks_bh23_neon:
	build/benchmarks/bh23/benchmark_neon

benchmarks_bh23_4x64: N := benchmark_4x64
benchmarks_bh23_4x64:
	mkdir -p build/benchmarks/bh23
	$(CC) $(CFLAGS) benchmarks/bh23/$(N).c -o build/benchmarks/bh23/$(N)

run_benchmarks_bh23_4x64:
	build/benchmarks/bh23/benchmark_4x64

benchmarks_bh23_4x64_neon: N := benchmark_4x64
benchmarks_bh23_4x64_neon:
	mkdir -p build/benchmarks/bh23
	$(ARM_CC) $(CFLAGS_NEON) benchmarks/bh23/$(N).c -o build/benchmarks/bh23/$(N)_neon

run_benchmarks_bh23_4x64_neon:
	build/benchmarks/bh23/benchmark_4x64_neon

## Domb
benchmarks_domb_4x64: N := benchmark_4x64
benchmarks_domb_4x64:
	mkdir -p build/benchmarks/domb
	$(CC) $(CFLAGS) benchmarks/domb/$(N).c -o build/benchmarks/domb/$(N)

run_benchmarks_domb_4x64:
	build/benchmarks/domb/benchmark_4x64

benchmarks_domb_4x64_neon: N := benchmark_4x64
benchmarks_domb_4x64_neon:
	mkdir -p build/benchmarks/domb
	$(ARM_CC) $(CFLAGS_NEON) benchmarks/domb/$(N).c -o build/benchmarks/domb/$(N)_neon

run_benchmarks_domb_4x64_neon:
	build/benchmarks/domb/benchmark_4x64_neon

## BM17
benchmarks_bm17: N := benchmark
benchmarks_bm17:
	mkdir -p build/benchmarks/bm17
	$(CC) $(CFLAGS_BM17) benchmarks/bm17/$(N).c -o build/benchmarks/bm17/$(N)

run_benchmarks_bm17:
	build/benchmarks/bm17/benchmark

benchmarks_bm17_neon: N := benchmark
benchmarks_bm17_neon:
	mkdir -p build/benchmarks/bm17
	$(ARM_CC) $(CFLAGS_NEON) benchmarks/bm17/$(N).c -o build/benchmarks/bm17/$(N)_neon

run_benchmarks_bm17_neon:
	build/benchmarks/bm17/benchmark_neon

# SLGCK14
benchmarks_slgck14_neon: N := benchmark
benchmarks_slgck14_neon:
	mkdir -p build/benchmarks/slgck14
	$(ARM_CC) $(CFLAGS_NEON) benchmarks/slgck14/$(N).c -o build/benchmarks/slgck14/$(N)_neon

run_benchmarks_slgck14_neon:
	build/benchmarks/slgck14/benchmark_neon

%:
	@:
