#!/bin/bash

# Create or update dyninst
if test ! -d dyninst; then
  git clone --depth 1 --branch master https://github.com/dyninst/dyninst
else
  git -C dyninst pull --ff-only origin master
fi

# Create or update the test suite
if test ! -d testsuite; then
  git clone --depth 1 --branch master https://github.com/dyninst/testsuite
else
  git -C testsuite pull --ff-only origin master
fi

declare -a packages=(cmake)
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
