#!/bin/bash

build_jobs=$1
if test x"${build_jobs}" = "x"; then
  build_jobs=1
fi

versions=(2019_U9 2020_U1 2020_U2 2020_U3 v2020.3.1 v2020.3.2)
for v in "${versions[@]}"; do
  if [ -d $v ]; then continue; fi
  echo ${v} >> /versions.txt
  wget --no-check-certificate https://github.com/oneapi-src/oneTBB/archive/refs/tags/${v}.tar.gz
  tar -xf ${v}.tar.gz
  dir_name=$(echo ${v} | sed 's/v//g')
  cd oneTBB-${dir_name}/
  make tbb_build_dir=install work_dir=working -j${build_jobs}
  mkdir -p /${v}/lib
  cp -R include /${v}/
  cp working_release/*.so* /${v}/lib
  cd cmake
  cat << END >CMakeLists.txt
cmake_minimum_required(VERSION 3.13.0)
project(TBB-build)
include(TBBMakeConfig.cmake)
tbb_make_config(
  TBB_ROOT /${v}
  CONFIG_FOR_SOURCE
  TBB_RELEASE_DIR /${v}/lib
)
END
  cmake .
  cd /
  rm -rf ${v}.tar.gz oneTBB-${dir_name}
done

versions=(v2021.1.1 v2021.2.0 v2021.2.1 v2021.3.0 v2021.4.0 v2021.5.0 v2021.6.0 v2021.7.0 v2021.8.0)
for v in "${versions[@]}"; do
  if [ -d $v ]; then continue; fi
  echo ${v} >> /versions.txt
  wget --no-check-certificate https://github.com/oneapi-src/oneTBB/archive/refs/tags/${v}.tar.gz
  tar -xf ${v}.tar.gz
  dir_name=$(echo ${v} | sed 's/v//g')
  cd oneTBB-${dir_name}/
  mkdir build
  cd build
  cmake .. -DCMAKE_INSTALL_PREFIX=/${v} -DTBB_TEST=OFF -DCMAKE_BUILD_TYPE=Release
  cmake --build . --parallel ${build_jobs}
  cmake --install .
  cd /
  rm -rf ${v}.tar.gz oneTBB-${dir_name}
done
