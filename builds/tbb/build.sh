#!/bin/bash

function run() {
  package=$1
  shift

  for v in "$@"; do
    echo "$package\@$v" >>${package}-install.log
    echo "Installing $package\@$v"
    spack install -j2 "$package\@$v"
    if test $? != 0; then
      echo "Failed to install $package\@$v" >&2
    fi
  done
}

. spack/share/spack/setup-env.sh
spack env activate .

rm -f intel-tbb-install.log intel-oneapi-tbb-install.log install.log

# Intel TBB
declare -a versions=(
  master 2021.6.0-rc1 2021.5.0 2021.4.0 2021.3.0 \
  2021.2.0 2021.1.1 2020.3 2020.2 2020.1 2020.0 \
  2019.9 2019.8 2019.7 2019.6 2019.5 2019.4 2019.3 \
  2019.2 2019.1 2019 2018.6)
run intel-tbb "${versions[@]}"

## OneAPI
versions=(2021.6.0 2021.5.1 2021.5.0 2021.4.0 2021.3.0 2021.2.0 2021.1.1)
run intel-oneapi-tbb "${versions[@]}"

cat intel-tbb-install.log intel-oneapi-tbb-install.log >> install.log
