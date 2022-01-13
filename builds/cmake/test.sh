#!/bin/bash

module unload cmake

. spack/share/spack/setup-env.sh

while IFS= read -r package; do
  echo testing $package

  spack/bin/spack load $package
  if test $? != 0; then
    echo Failed to load module for $package
    exit -1
  fi

  ./run gcc gcc --purge
  if test $? != 0; then
    echo Failed to run tests with $package
    exit -1
  fi
done < install.log
