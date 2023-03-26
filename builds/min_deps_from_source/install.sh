#!/bin/bash

build_jobs=$1
if test x"${build_jobs}" = "x"; then
  build_jobs=1
fi

version=$(grep elfutils dyninst/docker/dependencies.versions | awk '{split($0,a,":"); print a[2]}')
wget --no-check-certificate https://sourceware.org/elfutils/ftp/${version}/elfutils-${version}.tar.bz2
bunzip2 elfutils-${version}.tar.bz2
tar -xf elfutils-${version}.tar
cd elfutils-${version}/
mkdir build
cd build
../configure --disable-libdebuginfod --disable-debuginfod --prefix=/elfutils
make install -j${build_jobs}
cd /
rm -rf elfutils-${version}/ elfutils-${version}.tar


version=$(grep boost dyninst/docker/dependencies.versions | awk '{split($0,a,":"); print a[2]}')
wget --no-check-certificate https://boostorg.jfrog.io/artifactory/main/release/${version}/source/boost_${version//./_}.tar.bz2
tar -xf boost_${version//./_}.tar.bz2
cd boost_${version//./_}
./bootstrap.sh --with-libraries=atomic,chrono,date_time,filesystem,thread,timer --prefix=/boost
./b2 --prefix=/boost --layout=versioned variant=release link=shared threading=multi runtime-link=shared install
cd /
rm -rf boost_${version//./_} boost_${version//./_}.tar.bz2


version=$(grep tbb dyninst/docker/dependencies.versions | awk '{split($0,a,":"); print a[2]}' | sed 's/\./_U/')
wget --no-check-certificate https://github.com/oneapi-src/oneTBB/archive/refs/tags/${version}.tar.gz
tar -xf ${version}.tar.gz
dir_name=$(echo ${version} | sed 's/v//g')
cd oneTBB-${dir_name}/
make tbb_build_dir=install work_dir=working -j${build_jobs}
mkdir -p /tbb/lib
cp -R include /tbb/
cp working_release/*.so* working_debug/*.so* /tbb/lib
cd cmake
cat << END >CMakeLists.txt
cmake_minimum_required(VERSION 3.13.0)
project(TBB-build)
include(TBBMakeConfig.cmake)
tbb_make_config(
  TBB_ROOT /tbb
  CONFIG_FOR_SOURCE
  TBB_RELEASE_DIR /tbb/lib
  TBB_DEBUG_DIR /tbb/lib
)
END
cmake .
cd /
rm -rf ${version}.tar.gz oneTBB-${dir_name}
