#!/bin/bash

. spack/share/spack/setup-env.sh
spack env activate .

while IFS= read -r package; do
  echo testing $package

  spack load $package
  if test $? != 0; then
    echo Failed to load module for $package >&2
    spack unload $package
    continue
  fi

  perl testsuite/scripts/build/build.pl --njobs=2 --purge
  if test $? != 0; then
    echo Failed to run tests with $package >&2
    spack unload $package
    continue
  fi
  spack unload $package
done < install.log
