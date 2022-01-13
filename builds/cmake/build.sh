#!/bin/bash

#Install all versions since min_version
#NB: The script will not try to re-install existing versions
min_version=3.21.0
perl install.pl cmake $min_version
