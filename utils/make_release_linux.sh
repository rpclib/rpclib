#! /bin/sh

RELEASE_OUTPUT_DIR=release
SRC=src

BUILD_DIR=build
CMAKE_FLAGS="-DCALLME_BUILD_TESTS=OFF -DCALLME_BUILD_EXAMPLES=OFF"

# cleanup
[ -d $RELEASE_OUTPUT_DIR ] && rm -rf $RELEASE_OUTPUT_DIR

# create dirs
mkdir -p $RELEASE_OUTPUT_DIR && cd $RELEASE_OUTPUT_DIR

git clone --depth=1 --branch=dev https://github.com/sztomi/callme.git $SRC

# 64-bit
mkdir -p $BUILD_DIR && cd $BUILD_DIR
cmake ../$SRC -DCALLME_FORCE_M64=ON $CMAKE_FLAGS
make -j8
cpack -G TGZ
cpack -G STGZ
cpack -G DEB
cd ..

mv $BUILD_DIR/output/pkg/* .
rm -rf $BUILD_DIR

# 32-bit
mkdir -p $BUILD_DIR && cd $BUILD_DIR
cmake ../src -DCALLME_FORCE_M32=ON $CMAKE_FLAGS
make -j8
cpack -G TGZ
cpack -G STGZ
cpack -G DEB
cd ..

mv $BUILD_DIR/output/pkg/* .
rm -rf $BUILD_DIR
rm -rf $SRC
