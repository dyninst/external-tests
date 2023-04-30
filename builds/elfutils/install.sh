#!/bin/bash

build_jobs=$1
if test x"${build_jobs}" = "x"; then
  build_jobs=1
fi

# Fetch the versions in ascending order
git clone --depth=1 git://sourceware.org/git/elfutils.git
cd elfutils
git fetch --tags
declare -a versions=$(git tag --list | grep "elfutils" | sed 's/elfutils-//' | sort -V)

# Get the minimum version from Dyninst
cd /
min_version=$(grep elfutils dependencies.versions | awk '{split($0,a,":"); print a[2]}')

for v in ${versions}; do
  if [[ $v < $min_version ]]; then continue; fi
  if [[ -d /${v} ]]; then continue; fi
  echo $v >>versions.txt
  wget --no-check-certificate https://sourceware.org/elfutils/ftp/${v}/elfutils-${v}.tar.bz2
  tar -xf elfutils-${v}.tar.bz2
  cd elfutils-${v}/
  mkdir build; cd $_
  ../configure --disable-libdebuginfod --disable-debuginfod --prefix=/${v}
  make install -j${build_jobs}
  cd /
  rm -rf elfutils-${v}/ elfutils-${v}.tar.bz2
done

# Clean up
rm -rf /elfutils
