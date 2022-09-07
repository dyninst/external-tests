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
