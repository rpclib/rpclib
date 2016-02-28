#! /bin/sh

RELEASE_OUTPUT_DIR=release
SRC=src

ARCH64_DIR=arch_64
ARCH32_DIR=arch_32

cd $RELEASE_OUTPUT_DIR

echo "Getting sources"
git clone --depth=1 --branch=branch https://github.com/sztomi/callme.git src

echo "Building for 64-bit"
# 64-bit
mkdir ARCH64_DIR && cd $_
cmake ../src
make -j6
cpack
cpack -G DEB
