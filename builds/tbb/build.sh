#!/bin/bash

# A list of all package names added/installed here
package_log=packages.versions.log
rm -f $package_log

function add_packages() {
  package=$1
  shift
  
  rm -f ${package}-versions.log

  for v in "$@"; do
    echo "$package@$v" >>${package}-versions.log
    echo "Adding $package@$v"
    spack add "$package@$v"
    if test $? != 0; then
      echo "Failed to add $package@$v" >&2
    fi
  done
  
  cat ${package}-versions.log >> $package_log
}

. spack/share/spack/setup-env.sh
spack env activate .

# Intel TBB
declare -a versions=(
  master 2021.6.0-rc1 2021.5.0 2021.4.0 2021.3.0 \
  2021.2.0 2021.1.1 2020.3 2020.2 2020.1 2020.0 \
  2019.9 2019.8 2019.7 2019.6 2019.5 2019.4 2019.3 \
  2019.2 2019.1 2019 2018.6)
add_packages intel-tbb "${versions[@]}"

# OneAPI
versions=(2021.6.0 2021.5.1 2021.5.0 2021.4.0 2021.3.0 2021.2.0 2021.1.1)
add_packages intel-oneapi-tbb "${versions[@]}"

echo "Installing..."
spack install -j2 --reuse >packages.install.log
