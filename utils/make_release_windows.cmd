@echo off
@echo Creating release for Windows

set RELEASE_OUTPUT_DIR="release"
set SRC="src"

set BUILD_DIR="build"
set CMAKE_FLAGS="-DRPCLIB_BUILD_TESTS=OFF -DRPCLIB_BUILD_EXAMPLES=OFF"

REM cleanup
rmdir /s /q %RELEASE_OUTPUT_DIR%
rmdir /s /q %SRC%

REM create dirs
mkdir %RELEASE_OUTPUT_DIR%
cd %$RELEASE_OUTPUT_DIR%

git clone --depth=1 --branch=renaming https://github.com/rpclib/rpclib.git %SRC%

REM 64-bit
mkdir %BUILD_DIR% 
cd %BUILD_DIR%
cmake ../%SRC% -DRPCLIB_FORCE_M64=ON %CMAKE_FLAGS%
cmake --build .
cpack -G TGZ
cpack -G STGZ
cpack -G DEB
cd ..


