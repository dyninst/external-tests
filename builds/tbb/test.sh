#!/bin/bash

. spack/share/spack/setup-env.sh
spack load boost elfutils libiberty
rm -f packages.build.log

while IFS= read -r package; do
  echo testing $package

  spack load $package
  if test $? != 0; then
    echo Failed to load module for $package >&2
    spack unload $package
    continue
  fi

  rm -f build.log
  perl testsuite/scripts/build/build.pl --njobs=2 --purge --no-run-tests
  if test $? != 0; then
    echo Failed to run tests with $package >&2
    spack unload $package
    continue
  fi
  cat build.log >> packages.build.log
  spack unload $package
done < packages.versions.log
