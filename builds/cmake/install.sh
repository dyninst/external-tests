#!/bin/bash

build_jobs=$1
if test x"${build_jobs}" = "x"; then
  build_jobs=1
fi

# Fetch the versions
git clone --depth=1 https://github.com/kitware/cmake
cd cmake
git fetch --tags
declare -a versions=$(git tag --list | grep -v "beta" | sed 's/boost-//' | sort -rV)
cd /

for v in ${versions}; do
  if [[ $v < $min_version ]]; then continue; fi
  if [ -d $v ]; then continue; fi
#  if [[ $v = "2019" ]]
  echo $v >>versions.txt
  wget --no-check-certificate https://github.com/Kitware/CMake/archive/refs/tags/${v}.tar.gz
  tar -xf ${v}.tar.gz
  cd CMake-${v//v/}
  mkdir build; cd $_
  cmake .. -DCMAKE_INSTALL_PREFIX=$PWD -DCMAKE_BUILD_TYPE=Release
  cmake --build . --parallel ${build_jobs}
  cmake --install .
  rm -rf bin/{ccmake,cpack,ctest,ctresalloc} share/cmake-${v//v/}/Help
  strip bin/cmake
  tar -cf cmake-${v}.tar bin share
  bzip2 --best -z cmake-${v}.tar
  mkdir /${v}/
  mv cmake-${v}.tar.bz2 /${v}/
  cd /
  rm -rf CMake-${v//v/}/ ${v}.tar.gz
done
