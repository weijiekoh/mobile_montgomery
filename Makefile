CC     = gcc
CFLAGS = -O3 -Wall
# Flags for the Bos implementation
CFLAGS_BOS = $(CFLAGS)

all: clean mkdir tests benchmarks

clean:
	rm -rf build/*

# Tests
tests: tests_bigints tests_acar_mont tests_bos_mont

run_tests:
	build/tests/bigints/bigint_8x32/bigint
	build/tests/acar/mont
	build/tests/bos/mont

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

## tests/bos/mont
tests_bos_mont: N := mont
tests_bos_mont:
	mkdir -p build/tests/bos
	echo $(CC) $(CFLAGS) tests/bos/$(N).c -o build/tests/bos/$(N)
	$(CC) $(CFLAGS) tests/bos/$(N).c -o build/tests/bos/$(N)

run_tests_bos_mont:
	build/tests/bos/mont

# Benchmarks
benchmarks: benchmarks_acar

run_benchmarks:
	build/benchmarks/acar/benchmark

benchmarks_acar: N := benchmark
benchmarks_acar:
	mkdir -p build/benchmarks/acar
	echo $(CC) $(CFLAGS) benchmarks/acar/$(N).c -o build/benchmarks/acar/$(N)
	$(CC) $(CFLAGS) benchmarks/acar/$(N).c -o build/benchmarks/acar/$(N)

run_benchmarks_acar:
	build/benchmarks/acar/benchmark

%:
	@:
