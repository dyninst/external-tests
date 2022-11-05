#!/bin/bash

if test ! -e dyninst; then
  git clone --depth 1 --branch master https://github.com/dyninst/dyninst
fi

if test ! -e testsuite; then
  git clone --depth 1 --branch master https://github.com/dyninst/testsuite
fi

# Set default number of build/make jobs
if test x${BUILD_TEST_NUM_JOBS} == x; then
	export BUILD_TEST_NUM_JOBS=2
fi

declare -a packages=(tbb)
declare -a commands=(build.sh test.sh upload.sh cleanup.sh)

for p in "${packages[@]}"; do
  echo Processing package $p
  cd $p
  bash setup.sh ./dyninst ./testsuite
  for c in "${commands[@]}"; do
    echo Executing $c
    bash $c
    if test $? -ne 0; then
      echo "$p/$c failed" && exit -1
    fi
  done
  cd ..
done
