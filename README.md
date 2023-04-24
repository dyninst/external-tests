# External Tests for [Dyninst](https://github.com/dyninst/dyninst)

These are tests for Dyninst which do not fit within the framework of the [test suite](https://github.com/dyninst/testsuite).

## Building

Before building any of these examples, you need an existing build of Dyninst (see the Dyninst [wiki](https://github.com/dyninst/dyninst/wiki/Building-Dyninst) for details).

To configure the build, you can use

    cmake . -DDyninst_DIR=path/to/Dyninst/install/lib/cmake/Dyninst
    cmake --build .

## Running

Most of the tests are designed to examine some aspect of compiling or linking Dyninst. There are, however, a few tests that have a runtime aspect. These tests can be run using CTest like so:

    ctest .
