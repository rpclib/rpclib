# Getting started

This document details the steps of getting everything in place to be able to use `rpclib`.

## What you need

### Compiler

`rpclib` uses some C++14 features, and as such, requires a very recent compiler. The following compilers are tried and tested with `rpclib` at the moment:

  * g++ 5.0
  * clang++ 3.7
  * MSVC 2015 Update 1

TIP: Newer versions of these compilers are expected to work.

### Tools

In addition to a capable compiler, you will need:

  * [CMake 3.x](https://cmake.org)
  * GNU make (on \*nixes) or MSBuild/Visual Studio (on Microsoft Windows).

### Dependencies

`rpclib` has no external library dependencies, i.e. you don't need to install any extra libraries to use it. The library does use third-party code, but it is hidden both during compilation and linking (i.e. it means you don't need to worry about linking against those same libraries).

TIP: See the [Design](design.md) documentation for details on how `rpclib` handles its dependencies.


## Building

`rpclib` uses CMake and has a very conventional build process. If you know how to build a CMake-based library, you know how to build `rpclib`.

If not, you can read the <<building.adoc#,Building>> chapter for step-by-step instructions. You can also visit that document for information learn about advanced building options.

## Your first project with `rpclib`: "Here's my number"

In order to integrate `rpclib` into your project, you will need to have it built and stored somewhere on your system. Place the `rpclib` headers into your include path, and link librpc.a with your executable. The exact process of that depends on your compiler and/or IDE.

### CMake/Clion

`rpclib` ships with a CMake module that can be used to find the library in the system. Using it is pretty simple: place Findrpc.cmake into your CMake module path, then add the following to your CMakeLists.txt:

```cmake
find_package(rpc)
```

### Microsoft Visual Studio

TBD

### XCode

TBD

## Where to go from here

Now that you have an environment where you can compile and link programs with `rpclib`, either
check out the [Primer](primer.md) for a step-by-step tutorial with lots of explanation. If you
prefer short examples with minimal instructions, check out the [Cookbook](cookbook.md).
