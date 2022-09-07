#!/bin/bash

# Create or update dyninst
if test ! -e dyninst; then
  git clone --depth 1 --branch master https://github.com/dyninst/dyninst
else
  git -C dyninst pull --ff-only origin master
fi

# Create or update the test suite
if test ! -e testsuite; then
  git clone --depth 1 --branch master https://github.com/dyninst/testsuite
else
  git -C testsuite pull --ff-only origin master
fi

# Create or update spack
if test ! -e spack; then
  git clone --depth 1 --branch develop https://github.com/spack/spack

  # Use the E4S binary cache to speed up installs
  spack/bin/spack mirror add E4S https://cache.e4s.io
  spack/bin/spack buildcache keys --install --trust
else
  git -C spack pull --ff-only origin develop
fi

declare -a packages=(cmake tbb)
declare -a commands=(setup.sh build.sh test.sh upload.sh cleanup.sh)

for p in "${packages[@]}"; do
  echo Processing package $p
  cd $p
  for c in "${commands[@]}"; do
    echo Executing $c
    bash $c
    if test $? -ne 0; then
      echo "$p/$c failed" && exit -1
    fi
  done
  cd ..
done
