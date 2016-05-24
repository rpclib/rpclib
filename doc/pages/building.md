# Building `callme`

## Default configuration

The default configuration is the one intended for end-users. If you want to hack on `callme`, you might be interested in some of the Advanced options.

### Unix-type systems

This includes most (all?) Linux distributions and cygwin. Building `callme` is very similar to other cmake-based projects:

```
git clone git@github.com:sztomi/callme.git
cd callme
mkdir build && cd build
cmake ..
make
```

And that's it. If all goes well, your build output will be in the `output` directory.

### Microsoft Windows

On Windows with MSVC it is very similar, except you will probably want to

```
git clone git@github.com:sztomi/callme.git
cd callme
mkdir build && cd build
cmake ..
```

... and open the generated solution file in Visual Studio or build it using msbuild.

## Advanced options

There are some compilation options that affect the build output. These options can be set using ccmake, cmake-gui or on the cmake command line.

| Name | Default value | Usage
|------|---------------|------
|`CALLME_BUILD_TESTS` | OFF |Builds the unit tests of the library. You might want to turn this on if you are using an unreleased version.
|`CALLME_GENERATE_COMPDB` | OFF | Generates a json compilation database for use with clang-based tools (such as clang-tidy, YCM etc.)
|`CALLME_BUILD_EXAMPLES` | ON | Builds the collection of example programs that demonstrate the features of `callme`.
| `CALLME_ENABLE_LOGGING` | OFF |Enables the internal logging of `callme`. This slightly affects performance. Currently the logging is not very configurable (for example, everything goes to stdout), but there are plans to make it easier to integrate with your application. Use this feature for debugging purposes.
|`CALLME_ENABLE_COVERAGE` | OFF | This enables passing the code coverage generation flag when building with g++. It is used on Travis to provide coverage monitoring in tandem with Coveralls.io.
|`CALLME_INSTALL_MSGPACK` | ON | Install msgpack headers with the install target. `callme` currently ships some minor patches for msgpack, so this is recommended to be left on!
|`CALLME_FORCE_M64` | OFF | Force -m64 for g++ and clang++. Your build tools must support cross-compilation if this is not your native environment.
|`CALLME_FORCE_M32` | OFF | Force -m32 for g++ and clang++. Your build tools must support cross-compilation if this is not your native environment.
