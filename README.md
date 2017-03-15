# Geppetri

Implementation of the system described in [this paper](https://eprint.iacr.org/2017/013)

## Prerequisites

 - A Unix-like environment (e.g., Linux, mingw-w64)
 - GCC compiler, GMP development libraries
 - CodeBlocks IDE
 - Python 2.7 with Twisted and GMPY

## Installation instructions

 - After cloning the repository, download the dependencies:

```
git submodule init
git submodule update
```

 - Compile the ate-pairing library with SNARK support:

```
cd ate-pairing
make SUPPORT_SNARK=1
```

 - Compile the C++ library and tools by opening `qaps.workspace` in CodeBlocks and choosing "Rebuild Workspace" (on Windows, binaries are included so this step is optional)

 - Install our customized VIFF version:
 
```
 cd tueviff-fp-inlinecb
python setup.py install
```

## Running examples

 - For generic examples, see `simple.py`, `simple2.py`, and `simple3.py` in the `viff` folder. Edit and run test.sh to run these examples
 - For the "aggregate" and "logrank" examples from the paper, edit and run test_anon.sh and test_stat.sh, respectively
