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
do_install intel-tbb $(perl parse_versions.pl intel-tbb 2018.6)

# OneAPI
do_install intel-oneapi-tbb $(perl parse_versions.pl intel-oneapi-tbb)
