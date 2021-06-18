#!/bin/bash

set -uex

# A unique working directory

wd=$(pwd)
test_wd=$(pwd)/test_odb_529

mkdir -p ${test_wd}
cd ${test_wd}

# Select a bitfield from an existing odb

odc sql "select status.active@body" -i ../../tests/2000010106.odb -f odb -o first.odb

# Check that appropriate columns are there

odc header first.odb | grep INTEGER 
odc header first.odb | grep "status.active@body"

set +e
odc header first.odb | grep "status.active.active@body"
rc=$?
set -e
[[ $rc = 0 ]] && echo "status.active.active should not be found in odb file first.odb" && exit 1

# Attempt to select the output of this select

odc sql "select status.active@body" -i first.odb -f odb -o second.odb

# Check that appropriate columns are there

odc header second.odb | grep INTEGER 
odc header second.odb | grep "status.active@body"

set +e
odc header second.odb | grep "status.active.active@body"
rc=$?
set -e
[[ $rc = 0 ]] && echo "status.active.active should not be found in odb file second.odb" && exit 1

# Clean up

cd ${wd}
rm -rf ${test_wd}
