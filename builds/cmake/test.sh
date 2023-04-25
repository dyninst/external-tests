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

git clone --depth=1 --branch=master https://github.com/dyninst/dyninst

rm -rf build; mkdir build; cd $_

while read -r version; do
  echo Build Dyninst with CMake $version | tee -a $log_file
  tar -xf /cmake-$version.tar.bz2
  bin/cmake /dyninst -DDYNINST_WARNINGS_AS_ERRORS=ON >>$log_file 2>&1
  bin/cmake --build . --parallel $num_jobs >>$log_file 2>&1
  rm -rf *
done </versions.txt