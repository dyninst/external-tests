#!/bin/bash

build_jobs=$1
if test x"${build_jobs}" = "x"; then
  build_jobs=1
fi

# Fetch the versions
git clone --depth=1 https://github.com/boostorg/boost
cd boost
git fetch --tags
declare -a versions=$(git tag --list | grep -v "beta" | sed 's/boost-//' | sort -rV)

# Get the minimum version from Dyninst
cd /
wget --no-check-certificate https://raw.githubusercontent.com/dyninst/dyninst/master/docker/dependencies.versions
min_version=$(grep boost dependencies.versions | awk '{split($0,a,":"); print a[2]}')

for v in ${versions}; do
  if [[ $v < $min_version ]]; then continue; fi
  if [[ -f $v.tar.bz2 ]]; then continue; fi
  echo $v >>versions.txt
  wget --no-check-certificate https://boostorg.jfrog.io/artifactory/main/release/$v/source/boost_${v//./_}.tar.bz2
  tar -xf boost_${v//./_}.tar.bz2
  cd boost_${v//./_}
  ./bootstrap.sh --with-libraries=atomic,chrono,date_time,filesystem,thread,timer --prefix=/$v
  ./b2 --prefix=/$v --layout=versioned variant=release link=shared threading=multi runtime-link=shared install -j${build_jobs}
  cd /
  rm -rf boost_${v//./_} boost_${v//./_}.tar.bz2
  tar -zcf ${v}.tar.bz2 ${v}/
  rm -rf ${v}/
done

# Clean up
rm -rf boost/ dependencies.versions*
