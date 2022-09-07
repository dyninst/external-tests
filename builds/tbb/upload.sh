#!/bin/bash

# This cannot be part of 'build.sh' because the 'spack load ...' there
# puts the spack/.../lib directories in LD_LIBRARY_PATH which interferes
# with the system 'curl'.

for f in *.results.tar.gz; do
  curl -F token=ec2798c1fb37cce8d95d8e1d558bd519 -F upload=@$f https://bottle.cs.wisc.edu
done
