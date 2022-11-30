#! /bin/bash

if test x"${BUILD_TEST_NUM_JOBS}" = x; then
  echo "BUILD_TEST_NUM_JOBS not set"
  exit 1
fi

. ./spack/share/spack/setup-env.sh

for spec in $(perl parse_specs.pl); do
  echo -n "${spec}... "
  spack install -j${BUILD_TEST_NUM_JOBS} ${spec} >/dev/null 2>&1
  if test $? != 0; then
    echo "Failed to install" >&2
    continue
  fi

  spack load ${spec}
  if test $? != 0; then
    echo "Failed to load module" >&2
    spack unload ${spec}
    continue
  fi

  cd /
  rm -rf dyninst-build testsuite-build

  mkdir dyninst-build
  cd dyninst-build
  cmake /dyninst/src -DCMAKE_INSTALL_PREFIX=$PWD -DDYNINST_WARNINGS_AS_ERRORS=ON >/dev/null 2>&1
  cmake --build . --parallel ${BUILD_TEST_NUM_JOBS} >/dev/null 2>&1
  cmake --install . >/dev/null 2>&1
  if test $? != 0; then
    echo "Dyninst build failed" >&2
    spack unload ${spec}
    continue
  fi

  mkdir testsuite-build
  cd testsuite-build
  cmake /testsuite/src -DCMAKE_INSTALL_PREFIX=$PWD -DDyninst_DIR=/dyninst-build/lib/cmake/Dyninst >/dev/null 2>&1
  cmake --build . --parallel ${BUILD_TEST_NUM_JOBS} >/dev/null 2>&1
  cmake --install . >/dev/null 2>&1
  if test $? != 0; then
    echo "Testsuite build failed" >&2
    spack unload ${spec}
    continue
  fi

  # Don't care if these fail
  spack unload ${spec}
  spack uninstall -y ${spec} >/dev/null 2>&1
  echo "OK"
done
