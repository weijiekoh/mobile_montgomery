# Montgomery multiplication benchmarks

This repo contains implementations and benchmarks of Montgomery multiplication
algorithms, and benchmarks for non-WASM platforms.

The goal is to find the most suitable algorithm and implementation for mobile
devices, particulary those which implement NEON instructions.

## Install requirements

In Ubuntu Linux:

```bash
sudo apt install gcc make gcc-aarch64-linux-gnu qemu-aarch64
```

## Algorithms implemented

- [x] The CIOS method by [Acar
  (1996)](https://www.microsoft.com/en-us/research/wp-content/uploads/1996/01/j37acmon.pdf).
- [x] The NEON-optimised method in [BM17](https://eprint.iacr.org/2017/1057.pdf) by Joppe Bos.
- [ ] Montgomery squaring variants of all of the above algorithms.

The following algorithms are not yet implemented:

- [ ] The [gnark optimisation](https://hackmd.io/@gnark/modular_multiplication) of CIOS.
- [ ] Mitscha-Baude's [reduced-radix FIOS method](https://github.com/mitschabaude/montgomery/blob/main/doc/zprize22.md#13-x-30-bit-multiplication).
- [ ] Niall Emmart's [floating-point-based method](https://ieeexplore.ieee.org/document/8464792/).
- [ ] Montgomery squaring variants of all of the above algorithms.

## Quick start

To build and run on an x86 machine:

```bash
make
make run_tests
make run_benchmarks
```

To run the benchmarks on an ARM device:

```bash
make run_benchmarks_neon
```

Alternatively, cross-compile the benchmarks on your desktop machine, copy the
following binaries to the device, and run them.

```
build/benchmarks/acar/benchmark_neon
build/benchmarks/bm17/benchmark_neon
```

They have been tested with a Raspberry Pi 5, specifically the CanaKit 8GB version.

`lscpu`:

```
Architecture:             aarch64
  CPU op-mode(s):         32-bit, 64-bit
  Byte Order:             Little Endian
CPU(s):                   4
  On-line CPU(s) list:    0-3
Vendor ID:                ARM
  Model name:             Cortex-A76
    Model:                1
    Thread(s) per core:   1
    Core(s) per cluster:  4
    Socket(s):            -
    Cluster(s):           1
    Stepping:             r4p1
    CPU(s) scaling MHz:   75%
    CPU max MHz:          2400.0000
    CPU min MHz:          1500.0000
    BogoMIPS:             108.00
    Flags:                fp asimd evtstrm aes pmull sha1 sha2 crc32 atomics fphp asimdhp cpuid asimdrdm lrcpc dcpop asimddp
Caches (sum of all):      
  L1d:                    256 KiB (4 instances)
  L1i:                    256 KiB (4 instances)
  L2:                     2 MiB (4 instances)
  L3:                     2 MiB (1 instance)
Vulnerabilities:          
  Gather data sampling:   Not affected
  Itlb multihit:          Not affected
  L1tf:                   Not affected
  Mds:                    Not affected
  Meltdown:               Not affected
  Mmio stale data:        Not affected
  Reg file data sampling: Not affected
  Retbleed:               Not affected
  Spec rstack overflow:   Not affected
  Spec store bypass:      Mitigation; Speculative Store Bypass disabled via prctl
  Spectre v1:             Mitigation; __user pointer sanitization
  Spectre v2:             Mitigation; CSV2, BHB
  Srbds:                  Not affected
  Tsx async abort:        Not affected
```


`/proc/cpuinfo`:

```
$ cat /proc/cpuinfo 
processor       : 0
BogoMIPS        : 108.00
Features        : fp asimd evtstrm aes pmull sha1 sha2 crc32 atomics fphp asimdhp cpuid asimdrdm lrcpc dcpop asimddp
CPU implementer : 0x41
CPU architecture: 8
CPU variant     : 0x4
CPU part        : 0xd0b
CPU revision    : 1

processor       : 1
BogoMIPS        : 108.00
Features        : fp asimd evtstrm aes pmull sha1 sha2 crc32 atomics fphp asimdhp cpuid asimdrdm lrcpc dcpop asimddp
CPU implementer : 0x41
CPU architecture: 8
CPU variant     : 0x4
CPU part        : 0xd0b
CPU revision    : 1

processor       : 2
BogoMIPS        : 108.00
Features        : fp asimd evtstrm aes pmull sha1 sha2 crc32 atomics fphp asimdhp cpuid asimdrdm lrcpc dcpop asimddp
CPU implementer : 0x41
CPU architecture: 8
CPU variant     : 0x4
CPU part        : 0xd0b
CPU revision    : 1

processor       : 3
BogoMIPS        : 108.00
Features        : fp asimd evtstrm aes pmull sha1 sha2 crc32 atomics fphp asimdhp cpuid asimdrdm lrcpc dcpop asimddp
CPU implementer : 0x41
CPU architecture: 8
CPU variant     : 0x4
CPU part        : 0xd0b
CPU revision    : 1

Revision        : d04170
Serial          : 94e9857859362cfa
Model           : Raspberry Pi 5 Model B Rev 1.0
```
