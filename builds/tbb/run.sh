#! /bin/bash

ln -s /dyninst/src dyninst
ln -s /testsuite/src testsuite
ln -s /spack spack

base_dir=$PWD

. ./spack/share/spack/setup-env.sh
rm -f install.log test.log

for s in $(perl parse_specs.pl); do
  cd $base_dir
  echo -n "$s... "
  spack install -j${BUILD_TEST_NUM_JOBS} $s >>install.log 2>&1
  if test $? != 0; then
    echo "Failed to install" >&2
    continue
  fi
  
  spack load $s
  if test $? != 0; then
    echo "Failed to load module" >&2
    spack unload $s
    continue
  fi
  
  rm -rf dyninst-build-$s; mkdir dyninst-build-$s; cd dyninst-build-$s
  cmake ../dyninst -DCMAKE_INSTALL_PREFIX=$PWD -DDYNINST_WARNINGS_AS_ERRORS=ON >/dev/null 2>&1
  cmake --build . --parallel ${BUILD_TEST_NUM_JOBS} >/dev/null 2>&1
  cmake --install . >/dev/null 2>&1
  if test $? != 0; then
    echo "Failed" >&2
    spack unload $s
    continue
  fi
  
  cd $base_dir; rm -rf testsuite-build-$s; mkdir testsuite-build-$s; cd testsuite-build-$s
  cmake ../testsuite -DCMAKE_INSTALL_PREFIX=$PWD -DDyninst_DIR=$(realpath $PWD/../dyninst-build-$s/lib/cmake/Dyninst) >/dev/null 2>&1
  cmake --build . --parallel ${BUILD_TEST_NUM_JOBS} >/dev/null 2>&1
  cmake --install . >/dev/null 2>&1
  if test $? != 0; then
    echo "Failed" >&2
    spack unload $s
    continue
  fi

  # Don't care if these fail
  cd $base_dir
  rm -rf dyninst-build-$s testsuite-build-$s
  spack unload $s
  spack uninstall -y $s >/dev/null 2>&1
  echo "OK"
done
