#!/bin/bash

set -e

build_jobs=$1
if test x"${build_jobs}" = "x"; then
  build_jobs=1
fi

# Versions are double-dotted, so we need to make them lexicographically comparable
#  e.g., v3.9.1 -> 30901
function format_cmake_version {
  echo $1 | sed 's/v//g' | awk 'split($0,a,"."){printf "%s%02d%02d", a[1], a[2], a[3]}'
}

# Fetch the versions
git clone --depth=1 https:///github.com/kitware/cmake
cd cmake
git fetch --tags
declare -a versions=$(git tag --list | grep -v "\-rc" | sort -rV)

# Get the minimum version from Dyninst
cd /
rm -f dependencies.versions
wget --no-check-certificate https://raw.githubusercontent.com/dyninst/dyninst/cmake_modernization/docker/dependencies.versions
min_version=$(grep cmake dependencies.versions | awk '{split($0,a,":"); print a[2]}')
min_version=$(format_cmake_version $min_version)

# Clean up
cd /
rm -rf dependencies.versions cmake

for v in ${versions}; do
  v_formatted=$(format_cmake_version $v)
  if [[ $v_formatted < $min_version ]]; then continue; fi
  if [[ $v_formatted == "31900" ]]; then continue; fi
  if [[ -f $v.tar.bz2 ]]; then continue; fi
  echo $v >>versions.txt
  wget --no-check-certificate https://github.com/Kitware/CMake/archive/refs/tags/${v}.tar.gz
  cp /sources/${v}.tar.gz .
  tar -xf ${v}.tar.gz
  cd CMake-${v//v/}
  mkdir build; cd $_
  cmake .. -DCMAKE_INSTALL_PREFIX=$PWD -DCMAKE_BUILD_TYPE=Release
  cmake --build . --parallel ${build_jobs}
  cmake --install .
  rm -rf bin/{ccmake,cpack,ctest,ctresalloc} share/cmake-${v//v/}/Help
  strip bin/cmake
  tar -zcf cmake-${v}.tar.bz2 bin share
  mv cmake-${v}.tar.bz2 /
  cd /
  rm -rf CMake-${v//v/}/ ${v}.tar.gz
done
