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

git clone --depth 1 --branch master https://github.com/dyninst/testsuite testsuite/src
perl build.pl --num-jobs=$num_jobs --log-file=$log_file
