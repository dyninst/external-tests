#!/bin/bash

files=$(find /lib64/ -type f -name "*.so.*" | xargs rpm -qf | sort | uniq)

for f in ${files[*]}; do
  dnf debuginfo-install -y $f
done
