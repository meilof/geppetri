# Geppetri

Implementation of the system described in [this paper](https://eprint.iacr.org/2017/013)

## Prerequisites

 - A Unix-like environment (e.g., Linux, mingw-w64)
 - GCC compiler, GMP development libraries
 - CodeBlocks IDE (optional, for re-building on Windows)
 - Python 2.7 with Twisted and GMPY
 
 For instance, on Amazon Linux the following commands install the required dependencies:
 
 ```
 sudo yum install gcc-c++ gmp-devel
 pip install --upgrade pip
 sudm pip install twisted gmpy scipy
 ```

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

 - On Unix, compile the C++ library and tools by running run ``make`` from the main directory
 
 - On Windows (64-bits), binaries are included so building is not necessary; to re-build, open `qaps.workspace` in CodeBlocks and choosing "Rebuild Workspace" (also works on Unix)

 - Install our customized VIFF version:
 
```
cd tueviff-fp-inlinecb
python setup.py install
```

## Running examples

 - For generic examples, see `simple.py`, `simple2.py`, and `simple3.py` in the `viff` directory. Edit and run `test.sh` from the `bin` directory to run these examples
 - For the "aggregate" and "logrank" examples from the paper, edit and run test_anon.sh and test_stat.sh, respectively

## Acknowledgements

This work is part of projects that have received funding from the European Union’s Horizon 2020 research and innovation programme under grant agreement No 643964 ([SUPERCLOUD](https://supercloud-project.eu/)) and No 731583 ([SODA](https://www.soda-project.eu/)).
