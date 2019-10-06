# gcc for MRISC32

This is a fork of [gcc](https://github.com/gcc-mirror/gcc) with support for [MRISC32](https://github.com/mrisc32/mrisc32).

## Status

The MRISC32 back end for gcc is still in early development. Currently you can only compile very simple C programs (no C++, no libc).

## Prerequisites

Ubuntu:
```bash
sudo apt install flex bison libbison-dev libgmp-dev libmpfr-dev libmpc-dev
```

You also have to have an installation of [MRISC32 binutils](https://github.com/mrisc32/binutils-mrisc32) in your PATH.

## Building

### Build the bootstrap version of GCC

Configure and build:

```bash
$ mkdir build
$ cd build
$ ../configure --target=mrisc32-elf --enable-languages=c --without-headers --with-newlib --with-gnu-as --with-gnu-ld
$ make all-gcc
```

Install:

```bash
$ sudo make install-gcc
```

## About this Git repo

The MRISC32 back end for gcc is maintained as a branch that is periodically rebased on top of the latest upstream master branch and force pushed to the fork repository. To update your local clone you need to:

```bash
$ git fetch origin
$ git reset --hard origin/mbitsnbites/mrisc32
```
