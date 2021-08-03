#!/bin/bash

set -uex

# A unique working directory

wd=$(pwd)
test_wd=$(pwd)/test_odb_split

mkdir -p ${test_wd}
cd ${test_wd}

# In case we are resuming from a previous failed run, which has left output in the directory
rm *.odb || true

odc split ../../2000010106-reduced.odb "2000010106_varno_{varno}.odb"

nfiles=$(ls -lh *.odb | wc -l)

if [[ $nfiles -ne 10 ]]; then
    echo "Got $nfiles output files. Expected 10"
    exit -1
fi

for i in 1,9 2,183 3,2415 4,2415 9,119 110,9 112,119 119,44519 123,119 206,93; do

    IFS=","
    set $i
    nrows=$(odc count 2000010106_varno_$1.odb)
    if [[ $nrows -ne $2 ]]; then
        echo "Mismatched odb size. Got $nrows rows, expected $2"
        exit -1;
    fi
    unset IFS

done
ls -lh

# Clean up

cd ${wd}
rm -rf ${test_wd}
