#!/bin/bash

# Create or update spack
if test ! -d spack; then
  git clone --depth 1 --branch develop https://github.com/spack/spack
else
  git -C spack pull --ff-only origin develop
fi

# Use the E4S binary cache to speed up installs
spack/bin/spack mirror add E4S https://cache.e4s.io
spack/bin/spack buildcache keys --install --trust
