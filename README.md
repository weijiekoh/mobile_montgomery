# Montgomery multiplication benchmarks

This repo contains implementations and benchmarks of Montgomery multiplication
algorithms, and benchmarks for non-WASM platforms.

The goal is to find the most suitable algorithm and implementation for mobile
devices, particulary those which implement NEON instructions.

## Requirements

- `gcc`
- `make`

## Algorithms implemented

- [x] The CIOS method by [Acar
  (1996)](https://www.microsoft.com/en-us/research/wp-content/uploads/1996/01/j37acmon.pdf)

## Quick start

```bash
make
make run_tests
make run_benchmarks
```
