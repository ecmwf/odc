#!/bin/bash

set -uex

# A unique working directory

wd=$(pwd)
test_wd=$(pwd)/test_odb_sql_split

mkdir -p ${test_wd}
cd ${test_wd}

# In case we are resuming from a previous failed run, which has left output in the directory
rm *.odb || true

odc sql "select lat,lon,varno,obsvalue" -i ../../2000010106.odb -f odb -o "2000010106_varno_{varno}.odb"

nfiles=$(ls -lh *.odb | wc -l)

if [[ $nfiles -ne 16 ]]; then
    echo "Got $nfiles output files. Expected 16"
    exit -1
fi

for i in 1,12326 110,10914 112,13609 119,3084678 123,13609 2,8384 206,8881 29,1317 3,61640 39,32 4,61640 41,10192 42,10192 58,9381 7,1349 9,13609; do

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
#rm -rf ${test_wd}
