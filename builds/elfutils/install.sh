#!/bin/bash

build_jobs=$1
if x"${build_jobs}" = "x"; then
  build_jobs=1
fi

for v in 0.186 0.187 0.188 0.189; do
  if [ -d $v ]; then continue; fi
  wget --no-check-certificate https://sourceware.org/elfutils/ftp/${v}/elfutils-${v}.tar.bz2
  tar -xf elfutils-${v}.tar.bz2
  cd elfutils-${v}/
  mkdir build; cd $_
  ../configure --enable-libdebuginfod --disable-debuginfod --prefix=/${v}
  make install -j${build_jobs}
  cd /
  rm -rf elfutils-${v}/ elfutils-${v}.tar.bz2
done
