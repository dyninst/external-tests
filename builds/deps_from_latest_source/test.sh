#!/bin/bash

set -e

build_jobs=$1
if test x"${build_jobs}" = "x"; then
  build_jobs=1
fi

# Update sources
for d in elfutils boost tbb dyninst; do
  git -C /$d-src pull
done

cd /elfutils-src
autoreconf -i -f
./configure --disable-libdebuginfod --disable-debuginfod --prefix=/elfutils --enable-maintainer-mode
make install -j${build_jobs}

cd /boost-src
./bootstrap.sh --with-libraries=atomic,chrono,date_time,filesystem,thread,timer --prefix=/boost
./b2 --prefix=/boost --layout=versioned variant=release link=shared threading=multi runtime-link=shared install -j${build_jobs}

cd /tbb-src
cmake . -DCMAKE_INSTALL_PREFIX=/tbb -DTBB_TEST=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel ${build_jobs}
cmake --install .

cd /dyninst-src
cmake . -DBoost_ROOT_DIR=/boost -DTBB_ROOT_DIR=/tbb -DElfUtils_ROOT_DIR=/elfutils -DDYNINST_WARNINGS_AS_ERRORS=ON
cmake --build . --parallel $build_jobs
