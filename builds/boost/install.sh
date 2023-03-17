#!/bin/bash

for v in 1.71.0 1.72.0 1.73.0 1.74.0 1.75.0 1.76.0 1.77.0 1.78.0 1.79.0 1.80.0 1.81.0; do
  if [ -d $v ]; then continue; fi
  wget --no-check-certificate https://boostorg.jfrog.io/artifactory/main/release/$v/source/boost_${v//./_}.tar.bz2
  tar -xf boost_${v//./_}.tar.bz2
  cd boost_${v//./_}
  ./bootstrap.sh --with-libraries=atomic,chrono,date_time,filesystem,thread,timer --prefix=/$v
  ./b2 --prefix=/$v --layout=versioned variant=release link=shared threading=multi runtime-link=shared install
  cd /
  rm -rf boost_${v//./_} boost_${v//./_}.tar.bz2
done
