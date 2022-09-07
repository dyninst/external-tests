#!/bin/bash

for f in *.results.tar.gz; do
  rm -f $f;
done

rm -f build.log install.log dyninst testsuite
