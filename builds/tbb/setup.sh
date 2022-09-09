#!/bin/bash

# Make links to dyninst/testsuite
dyninst_dir=$1
testsuite_dir=$2
ln -s $dyninst_dir dyninst
ln -s $testsuite_dir testsuite

# Grab a fresh spack
git clone --depth 1 --branch develop https://github.com/spack/spack

# Use the E4S binary cache to speed up installs
spack/bin/spack mirror add E4S https://cache.e4s.io
spack/bin/spack buildcache keys --install --trust

# Install dependencies
. spack/share/spack/setup-env.sh
spack env create -d .
spack config add 'concretizer:unify:true'
spack env activate .
spack external find --not-buildable gcc autoconf bzip2 git tar xz perl cmake m4 ncurses
spack add cmake
spack add perl
spack add boost+atomic+chrono+date_time+filesystem+system+thread+timer+container+random+exception
spack add elfutils
spack add libiberty+pic
spack install --reuse
