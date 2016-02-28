#! /bin/sh

RELEASE_OUTPUT_DIR=release
SRC=src

ARCH64_DIR=arch_64
ARCH32_DIR=arch_32

mkdir -p $RELEASE_OUTPUT_DIR && cd $RELEASE_OUTPUT_DIR

echo "Getting sources"
git clone --depth=1 --branch=dev https://github.com/sztomi/callme.git src

echo "Building for 64-bit"
# 64-bit
mkdir -p $ARCH64_DIR && cd $ARCH64_DIR
cmake ../src
make -j6
cpack
cpack -G DEB
