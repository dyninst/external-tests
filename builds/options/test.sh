#!/bin/bash

build_jobs=$1
if test x"${build_jobs}" = "x"; then
  build_jobs=1
fi

git clone --depth 1 --branch cmake_modernization https://github.com/dyninst/testsuite testsuite/src
perl build.pl --num-jobs=${build_jobs}
