#! /bin/sh

RELEASE_OUTPUT_DIR=release
SRC=src

ARCH64_DIR=arch_64
ARCH32_DIR=arch_32
BUILD_DIR=build
CMAKE_FLAGS="-DCALLME_BUILD_TESTS=OFF -DCALLME_BUILD_EXAMPLES=OFF"

# cleanup
[ -d $RELEASE_OUTPUT_DIR ] && rm -rf $RELEASE_OUTPUT_DIR
[ -d $ARCH32_DIR ] && rm -rf $ARCH32_DIR
[ -d $ARCH64_DIR ] && rm -rf $ARCH64_DIR

# create dirs
mkdir -p $RELEASE_OUTPUT_DIR && cd $RELEASE_OUTPUT_DIR
mkdir -p ARCH64_DIR
mkdir -p ARCH32_DIR

git clone --depth=1 --branch=dev https://github.com/sztomi/callme.git src

# 64-bit
mkdir -p $BUILD_DIR && cd $BUILD_DIR
cmake ../src -DCALLME_FORCE_M64=ON $CMAKE_FLAGS
make -j6
cpack
cpack -G DEB
cd ..

mv $BUILD_DIR/output/pkg/* $ARCH64_DIR
rm -rf build

# 32-bit
mkdir -p $BUILD_DIR && cd $BUILD_DIR
cmake ../src -DCALLME_FORCE_M32=ON $CMAKE_FLAGS
make -j6
cpack
cpack -G DEB
cd ..

mv $BUILD_DIR/output/pkg/* $ARCH32_DIR
rm -rf build
