# Getting started

This document details the steps of getting everything in place to be able to use `rpclib`.

## What you need

### Compiler

`rpclib` uses some C++14 features, and as such, requires a recent compiler. The following compilers are tried and tested with `rpclib` at the moment:

  * g++ 5.0
  * clang++ 3.7
  * MSVC 2015 Update 2

Newer versions of these compilers are expected to work.

### Tools

In addition to a capable compiler, you will need:

  * [CMake 3.x](https://cmake.org)
  * And either
      * GNU make, or
      * Ninja, or
      * MSBuild/Visual Studio (on Microsoft Windows).

### The library

There are various release packages available for the most popular systems and ideally you should install one of those. If there is no suitable package for your system or you simply prefer to build from source, take a look at the [Compiling](compiling.md) chapter.

!!! info
    `rpclib` uses CMake and has a very conventional build process. If you know how to build a CMake-based library, you know how to build `rpclib`.

#### Dependencies

`rpclib` has no external library dependencies, i.e. you don't need to install any extra libraries to use it. The library does use third-party code, but it is hidden both during compilation and linking (i.e. it means you don't need to worry about linking against those same libraries).

!!! tip
    See the [Design](design.md) documentation for details on how `rpclib` handles its dependencies.


## Setting up your environment

In order to use `rpclib` in your projects, you will need to have it built and stored somewhere on your system. Place the `rpclib` headers into your include path, and link the static library with your executable. The exact process of that depends on your compiler and/or IDE.

## Where to go from here

Now that you have an environment where you can compile and link programs with `rpclib`, either
check out the [Primer](primer.md) for a step-by-step tutorial or the [Cookbook](cookbook.md), if you prefer short examples with minimal instructions.
