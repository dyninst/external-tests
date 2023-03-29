#!/bin/bash

set -e

build_jobs=$1
if test x"${build_jobs}" = "x"; then
  build_jobs=1
fi

git clone --depth=1 --branch=cmake_modernization https://github.com/dyninst/dyninst

mkdir build
cd build

while read -r version; do
  echo Build Dyninst with Boost $version
  cmake /dyninst -DBoost_ROOT_DIR=/$version -DDYNINST_WARNINGS_AS_ERRORS=ON
  cmake --build . --parallel $build_jobs
  rm -rf *
done </versions.txt
