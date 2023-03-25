#!/bin/bash

build_jobs=$1
if test x"${build_jobs}" = "x"; then
  build_jobs=1
fi

declare -a versions=(
  v3.26.0 v3.25.3 v3.25.2 v3.25.1 v3.25.0 v3.24.4 v3.24.3 v3.24.2 v3.24.1 v3.24.0
  v3.23.5 v3.23.4 v3.23.3 v3.23.2 v3.23.1 v3.23.0 v3.22.6 v3.22.5 v3.22.4 v3.22.3
  v3.22.2 v3.22.1 v3.22.0 v3.21.7 v3.21.6 v3.21.5 v3.21.4 v3.21.3 v3.21.2 v3.21.1
  v3.21.0 v3.20.6 v3.20.5 v3.20.4 v3.20.3 v3.20.2 v3.20.1 v3.20.0 v3.19.8 v3.19.7
  v3.19.6 v3.19.5 v3.19.4 v3.19.3 v3.19.2 v3.19.1         v3.18.6 v3.18.5 v3.18.4
  v3.18.3 v3.18.2 v3.18.1 v3.18.0 v3.17.5 v3.17.4 v3.17.3 v3.17.2 v3.17.1 v3.17.0
  v3.16.9 v3.16.8 v3.16.7 v3.16.6 v3.16.5 v3.16.4 v3.16.3 v3.16.2 v3.16.1 v3.16.0
  v3.15.7 v3.15.6 v3.15.5 v3.15.4 v3.15.3 v3.15.2 v3.15.1 v3.15.0 v3.14.7 v3.14.6
  v3.14.5 v3.14.4 v3.14.3 v3.14.2 v3.14.1 v3.14.0 v3.13.5 v3.13.4 v3.13.3 v3.13.2
  v3.13.1 v3.13.0
)

for v in "${versions[@]}"; do
  if [ -d $v ]; then continue; fi
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
