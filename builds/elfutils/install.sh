#!/bin/bash

build_jobs=$1
if x"${build_jobs}" = "x"; then
  build_jobs=1
fi

# Fetch the versions
git clone --depth=1 git://sourceware.org/git/elfutils.git
cd elfutils
git fetch --tags
declare -a versions=$(git tag --list | grep "elfutils" | sed 's/elfutils-//' | sort -rV)

# Get the minimum version from Dyninst
cd /
rm -f dependencies.versions
wget --no-check-certificate https://raw.githubusercontent.com/dyninst/dyninst/cmake_modernization/docker/dependencies.versions
min_version=$(grep elfutils dependencies.versions | awk '{split($0,a,":"); print a[2]}')

# Clean up
rm -rf /elfutils dependencies.versions

for v in ${versions}; do
  if [[ $v < $min_version ]]; then continue; fi
  echo $v >>versions.txt
  wget --no-check-certificate https://sourceware.org/elfutils/ftp/${v}/elfutils-${v}.tar.bz2
  tar -xf elfutils-${v}.tar.bz2
  cd elfutils-${v}/
  mkdir build; cd $_
  ../configure --enable-libdebuginfod --disable-debuginfod --prefix=/${v}
  make install -j${build_jobs}
  cd /
  rm -rf elfutils-${v}/ elfutils-${v}.tar.bz2
done
