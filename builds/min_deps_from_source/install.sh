#!/bin/bash

git clone --depth=1 git://sourceware.org/git/elfutils.git elfutils-src
git clone --depth=1 --recursive https://github.com/boostorg/boost.git boost-src
git clone --depth=1 --branch=cmake_modernization https://github.com/dyninst/dyninst dyninst-src
git clone --depth=1 https://github.com/oneapi-src/oneTBB.git tbb-src
