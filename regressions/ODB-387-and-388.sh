#!/bin/bash

set -uex

# A unique working directory

wd=$(pwd)
test_wd=$(pwd)/test_odb_387_388

mkdir -p ${test_wd}
cd ${test_wd}

# Test doing a replace

odc mdset "expver='    9876'" ../odb_387_mdset.odb temporary1.odb

# Demonstrate that we have changed a column

odc compare ../odb_387_mdset.odb temporary1.odb && exit -1

odc compare -excludeColumns expver ../odb_387_mdset.odb temporary1.odb

# The file sizes are the same

if [[ $(stat -c%s ../odb_387_mdset.odb) -ne $(stat -c%s temporary1.odb) ]]; then
    exit -1
fi

# Change another column

odc mdset "stream=123456" temporary1.odb temporary2.odb

# Demonstrate that we can exclude only one, or a range of columns

odc compare ../odb_387_mdset.odb temporary1.odb && exit -1

odc compare temporary1.odb temporary2.odb && exit -1

odc compare -excludeColumns stream temporary1.odb temporary2.odb

odc compare -excludeColumns stream ../odb_387_mdset.odb temporary2.odb && exit -1

odc compare -excludeColumns stream,expver ../odb_387_mdset.odb temporary2.odb

# The file sizes are still the same

if [[ $(stat -c%s ../odb_387_mdset.odb) -ne $(stat -c%s temporary2.odb) ]]; then
    exit -1
fi

# Clean up

cd ${wd}
rm -rf ${test_wd}
