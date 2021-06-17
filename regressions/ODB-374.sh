#!/bin/bash

set -uex

# A unique working directory

wd=$(pwd)
test_wd=$(pwd)/test_odb_374

mkdir -p ${test_wd}
cd ${test_wd}

# Test doing a replace

odc mdset "expver='    0001'" ../../tests/2000010106.odb temporary1.odb

# And go back to where we were

odc mdset "expver='0018    '" temporary1.odb temporary2.odb

# And check that we made a change, and reverted it

odc compare ../../tests/2000010106.odb temporary2.odb

odc compare ../../tests/2000010106.odb temporary1.odb && exit -1

# Clean up

cd ${wd}
rm -rf ${test_wd}
