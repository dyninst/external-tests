#!/bin/bash

set -e

num_jobs=1
log_file="/dev/null"

while [[ $# -gt 0 ]]; do
  case $1 in
    -n|--num-jobs)
      num_jobs="$2"
      shift # past argument
      shift # past value
      ;;
    -l|--log-file)
      log_file="$2"
      shift # past argument
      shift # past value
      ;;
  esac
done

# Update sources
for d in elfutils boost tbb; do
  git -C /$d-src pull
done

cd /elfutils-src
autoreconf -i -f
./configure --disable-libdebuginfod --disable-debuginfod --prefix=/elfutils --enable-maintainer-mode >>$log_file 2>&1
make install -j${num_jobs}

cd /boost-src
./bootstrap.sh --with-libraries=atomic,chrono,date_time,filesystem,thread,timer --prefix=/boost >>$log_file 2>&1
./b2 --prefix=/boost --layout=versioned variant=release link=shared threading=multi runtime-link=shared install -j${num_jobs} >>$log_file 2>&1

cd /tbb-src
cmake . -DCMAKE_INSTALL_PREFIX=/tbb -DTBB_TEST=OFF -DCMAKE_BUILD_TYPE=Release >>$log_file 2>&1
cmake --build . --parallel ${num_jobs} >>$log_file 2>&1
cmake --install . >>$log_file 2>&1

git clone --depth=1 --branch=master https://github.com/dyninst/dyninst /dyninst
cd /dyninst
cmake . -DCMAKE_INSTALL_PREFIX=$PWD/install -DBoost_ROOT_DIR=/boost -DTBB_ROOT_DIR=/tbb -DElfUtils_ROOT_DIR=/elfutils -DDYNINST_WARNINGS_AS_ERRORS=ON >>$log_file 2>&1
cmake --build . --parallel $num_jobs >>$log_file 2>&1
cmake --install . >>$log_file 2>&1

git clone --depth=1 --branch=master https://github.com/dyninst/testsuite /testsuite
cd /testsuite
cmake . -DDyninst_DIR=/dyninst/install/lib/cmake/Dyninst -DBoost_ROOT_DIR=/boost -DTBB_ROOT_DIR=/tbb -DElfUtils_ROOT_DIR=/elfutils >>$log_file 2>&1
cmake --build . --parallel $num_jobs >>$log_file 2>&1
