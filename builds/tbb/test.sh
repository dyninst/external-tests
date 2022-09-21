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

  # TBB was the last thing loaded by spack, so it's the first entry in CMAKE_PREFIX_PATH
  tbb_loc=$(echo $CMAKE_PREFIX_PATH | awk '{split($0,x,":"); print x[1]}')

  rm -f build.log
  perl testsuite/scripts/build/build.pl --njobs=2 --purge --no-run-tests --dyninst-cmake-args="-DTBB_ROOT_DIR=\"${tbb_loc}/cmake\""
  if test $? != 0; then
    echo Failed to run tests with $package >&2
    spack unload $package
    continue
  fi
  cat build.log >> packages.build.log
  spack unload $package
done < packages.versions.log
