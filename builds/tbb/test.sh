#!/bin/bash

. spack/share/spack/setup-env.sh
spack load boost elfutils libiberty
rm -f packages.build.log test.log

while IFS= read -r package; do
  echo testing $package

  spack load $package
  if test $? != 0; then
    echo Failed to load module for $package >&2
    spack unload $package
    continue
  fi

  rm -f build.log
  perl testsuite/scripts/build/build.pl --njobs=${BUILD_TEST_NUM_JOBS} --purge --no-run-tests >>test.log 2>&1
  if test $? != 0; then
    echo Failed to build with $package >&2
    spack unload $package
    continue
  fi
  cat build.log >> packages.build.log
  spack unload $package
done < packages.versions.log
