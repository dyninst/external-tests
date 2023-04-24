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

git clone --depth=1 --branch=cmake_modernization https://github.com/dyninst/dyninst

mkdir build
cd build

while read -r version; do
  echo Build Dyninst with Elfutils $version | tee -a $log_file
  cmake /dyninst -DElfUtils_ROOT_DIR=/$version -DDYNINST_WARNINGS_AS_ERRORS=ON >>$log_file 2>&1
  cmake --build . --parallel $num_jobs >>$log_file 2>&1
  rm -rf *
done </versions.txt
