#!/bin/bash

set -e

build_jobs=$1
if test x"${build_jobs}" = "x"; then
  build_jobs=1
fi

git clone --depth=1 --branch=cmake_modernization https://github.com/dyninst/dyninst

rm -rf build; mkdir build; cd $_

while read -r version; do
  echo Build Dyninst with CMake $version
  tar -xf /cmake-$version.tar.bz2
  bin/cmake /dyninst -DDYNINST_WARNINGS_AS_ERRORS=ON
  bin/cmake --build . --parallel $build_jobs
  rm -rf *
done </versions.txt