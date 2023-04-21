#!/bin/bash

build_jobs=$1
if test x"${build_jobs}" = "x"; then
  build_jobs=1
fi

function build_with_make {
  make tbb_build_dir=install work_dir=working -j${build_jobs}
  mkdir -p /$1/lib
  cp -R include /$1/
  cp working_release/*.so* /$1/lib
  cd cmake
  cat << END >CMakeLists.txt
cmake_minimum_required(VERSION 3.13.0)
project(TBB-build)
include(TBBMakeConfig.cmake)
tbb_make_config(
  TBB_ROOT /$1
  CONFIG_FOR_SOURCE
  TBB_RELEASE_DIR /$1/lib
)
END
  cmake .
}

function build_with_cmake {
  cmake . -DCMAKE_INSTALL_PREFIX=/$1 -DTBB_TEST=OFF -DCMAKE_BUILD_TYPE=Release
  cmake --build . --parallel ${build_jobs}
  cmake --install .
}

# Fetch the versions in ascending order
git clone --depth=1 https://github.com/oneapi-src/oneTBB.git
cd oneTBB
git fetch --tags
declare -a versions=$(git tag --list | grep -P "^\d{4}|^v" | grep -v "\-" | sort -V)

# Get the minimum version from Dyninst
cd /
rm -f dependencies.versions
wget --no-check-certificate https://raw.githubusercontent.com/dyninst/dyninst/master/docker/dependencies.versions
min_version=$(grep tbb dependencies.versions | awk '{split($0,a,":"); print a[2]}')

# For versions 2019.X, the archives are named 2019_UX
year=$(echo $min_version | awk '{split($0,a,"."); print a[1]}')
if [[ $year < "2020" ]]; then
  min_version=$(echo $min_version | sed 's/\./_U/')
fi

# Cleanup
rm -rf oneTBB/ dependencies.versions

for v in ${versions}; do
  if [[ $v < $min_version ]]; then continue; fi
  if [ -d $v ]; then continue; fi
  echo ${v} >> /versions.txt
  wget --no-check-certificate https://github.com/oneapi-src/oneTBB/archive/refs/tags/${v}.tar.gz
  tar -xf ${v}.tar.gz
  dir_name=$(echo ${v} | sed 's/v//g')
  cd oneTBB-${dir_name}/

  if [[ $v < "v2021.1.1" ]]; then
    build_with_make $v
  else
    build_with_cmake $v
  fi
  
  cd /
  rm -rf ${v}.tar.gz oneTBB-${dir_name}
done
