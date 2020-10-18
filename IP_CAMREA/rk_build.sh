#!/bin/sh
mkdir build
cd build
export TOOLCHAIN_ROOT=/usr/bin/aarch64-linux-gnu-
export CC="${TOOLCHAIN_ROOT}gcc"
export CXX="${TOOLCHAIN_ROOT}g++"
cmake G "Unix Makefiles" DTARGET=aarch64 ..
make clean
make -j$(nproc)
