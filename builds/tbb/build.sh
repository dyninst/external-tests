#!/bin/bash

# A list of all package names added/installed here
package_log=packages.versions.log
rm -f $package_log packages.install.log

function do_install() {
  package=$1
  shift
  
  rm -f ${package}-versions.log

  for v in "$@"; do
    echo "Installing $package@$v..."
    spack install -j2 --reuse $package@$v >>packages.install.log 2>&1
    if test $? != 0; then
      echo "Failed to add $package@$v" >&2
      continue
    fi
    echo "$package@$v" >>${package}-versions.log
  done
  
  cat ${package}-versions.log >> $package_log
}

. spack/share/spack/setup-env.sh

# Intel TBB
declare -a versions=(
  master 2021.6.0-rc1 2021.5.0 2021.4.0 2021.3.0 \
  2021.2.0 2021.1.1 2020.3 2020.2 2020.1 2020.0 \
  2019.9 2019.8 2019.7 2019.6 2019.5 2019.4 2019.3 \
  2019.2 2019.1 2019 2018.6)
do_install intel-tbb "${versions[@]}"

# OneAPI
versions=(2021.6.0 2021.5.1 2021.5.0 2021.4.0 2021.3.0 2021.2.0 2021.1.1)
do_install intel-oneapi-tbb "${versions[@]}"
