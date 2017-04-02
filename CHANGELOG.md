# Changelog

### 2.0.0

This is an interface-breaking release (hence the major version number increase).

Released on 2017-04-02.

Changes since 1.0.0:

  * **Added C++11 compatibility** (huge thanks to Github user mbr0wn!)
  * Added global timeout for blocking calls in the client
  * Replaced the internal pimpl_ptr with std::unique_ptr for better stability
  * Fixed a build problem with newer clang versions
  * Contains some preliminary VS2013 work (but it's very far from ready)


### 1.0.0

This is the first, complete release of rpclib.

Released on 2017-03-11.

Changes since 1.0.0-preview:

  * A buffer overflow bug was fixed in the server
  * New unit tests were added
  * CMake no longer assumes libc++ when clang is used
  * Documentation fixes
  * Added the ability to pass extra flags to the build without changing the
    CMakeLists.txt
  * Fixed requiring RPCLIB\_MSGPACK macro when not using Findrpclib.cmake
  * Created [conan](https://conan.io) package
  * A [benchmark suite](https://github.com/rpclib/benchmarks) was implemented

### 1.0.0-preview1

This is the first public release of rpclib! This release is a preview which means that the actual 1.0.0 will be similar, but hopefully improved. Any feedback is welcome!

Released on 2016-08-28.
