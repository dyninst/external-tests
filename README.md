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

A test that cannot run in this configuration, or on this host, is still
listed, and ctest reports it as Skipped. `ctest -V` shows the reason.

## Adding a test

Every test is added with `dyninst_add_test`, from
`cmake/common/ctest/DyninstAddTest.cmake`. It always creates the test: if
the test cannot be built or run in this configuration, ctest reports it
as skipped and says why. Tests are written in C++.

    dyninst_add_test(e_parser
      SOURCE_FILES parser.cpp
      DEPS Dyninst::symtabAPI
    )

`cmake/common` is a copy of Dyninst's, kept unchanged so that a test is
written the same way in both repositories; a change to it is made in
Dyninst and copied here. [`cmake/common/README.md`](cmake/common/README.md)
describes the functions in full: their keywords, defining a requirement,
skipping, and where a test's output goes. There are no test types here,
so tests have no `TYPE`.

### Requirements

The requirements are defined in `cmake/DyninstTestRequirements.cmake`,
each with the reason a test prints when it is not met:

| Requirement      | Met when                                  |
| :--------------- | :---------------------------------------- |
| `static_exe`     | `DYNINST_ENABLE_STATIC_TESTS`             |
| `static_rewrite` | the host is not aarch64                   |
