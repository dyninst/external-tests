#!/bin/bash

build_jobs=$1
if test x"${build_jobs}" = "x"; then
  build_jobs=1
fi

git clone --depth 1 --branch master https://github.com/dyninst/testsuite testsuite/src
export BUILD_TEST_NUM_JOBS=${build_jobs}
perl build.pl
