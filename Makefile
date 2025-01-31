CC     = gcc
CFLAGS = -O3 -Wall

# Flags for the Acar implementation
CFLAGS_ACAR = $(CFLAGS)
TARGET = bigint
SRC    = tests/acar/bigint.c

all: $(TARGET)

TIME := $(shell which time)

all: clean mkdir tests benchmarks

clean:
	rm -rf build/*

# Tests
tests: tests_acar_bigint tests_acar_mont

run_tests:
	build/tests/acar/bigint
	build/tests/acar/mont

tests_acar_bigint: N := bigint
tests_acar_bigint:
	mkdir -p build/tests/acar
	echo $(CC) $(CFLAGS_ACAR) tests/acar/$(N).c -o build/tests/acar/$(N)
	$(CC) $(CFLAGS_ACAR) tests/acar/$(N).c -o build/tests/acar/$(N)

run_tests_acar_bigint:
	build/tests/acar/bigint

tests_acar_mont: N := mont
tests_acar_mont:
	mkdir -p build/tests/acar
	echo $(CC) $(CFLAGS_ACAR) tests/acar/$(N).c -o build/tests/acar/$(N)
	$(CC) $(CFLAGS_ACAR) tests/acar/$(N).c -o build/tests/acar/$(N)

run_tests_acar_mont:
	build/tests/acar/mont


# Benchmarks
benchmarks: benchmarks_acar

run_benchmarks:
	build/benchmarks/acar/benchmark

benchmarks_acar: N := benchmark
benchmarks_acar:
	mkdir -p build/benchmarks/acar
	echo $(CC) $(CFLAGS_ACAR) benchmarks/acar/$(N).c -o build/benchmarks/acar/$(N)
	$(CC) $(CFLAGS_ACAR) benchmarks/acar/$(N).c -o build/benchmarks/acar/$(N)

run_benchmarks_acar:
	build/benchmarks/acar/benchmark

%:
	@:
