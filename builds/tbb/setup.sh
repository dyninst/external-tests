#!/bin/bash

if test x"$1" = "x" -o x"$2" = "x"; then
  echo "Usage: $0 dyninst_dir testsuite_dir"
  exit -1
fi

# Make links to dyninst/testsuite
ln -s $1 dyninst
ln -s $2 testsuite

# Grab a fresh spack
git clone --depth 1 --branch develop https://github.com/spack/spack

# Install dependencies
. spack/share/spack/setup-env.sh
spack external find --not-buildable gcc autoconf bzip2 git tar xz perl cmake m4 ncurses
spack install cmake
spack install perl
spack install boost+atomic+chrono+date_time+filesystem+system+thread+timer+container+random+exception
spack install elfutils
spack install libiberty+pic
