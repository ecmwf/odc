#!/bin/bash

set -uex

# A unique working directory

wd=$(pwd)
test_wd=$(pwd)/test_odb_exit_codes

mkdir -p ${test_wd}
cd ${test_wd}

# In case we are resuming from a previous failed run, which has left output in the directory
rm *.csv *.odb foobar || true

# Create some test data

cat > data-1-2.csv <<EOF
col1:INTEGER,col2:REAL,col3:DOUBLE,col4:INTEGER,col5:BITFIELD[a:1;b:2;c:5],col6:INTEGER
1,1.23,4.56,7,999,0
123,0.0,0.0,321,888,0
321,0.0,0.0,123,777,0
0,3.25,0.0,0,666,0
0,0.0,3.25,0,555,0
EOF

odc import data-1-2.csv data-1.odb
odc import data-1-2.csv data-2.odb

[[ $(odc count data-1.odb) -eq 5 ]] || (echo "Unexpected number of rows in ODB data-1.odb" ; false)
[[ $(odc count data-2.odb) -eq 5 ]] || (echo "Unexpected number of rows in ODB data-2.odb" ; false)

cat > data-3.csv <<EOF
date@hdr:INTEGER,lat@hdr:REAL,lon@hdr:REAL,obsvalue@body:REAL
20210401,38.560001,-121.300003,101340.000000
20210401,38.560001,-121.300003,290.200012
20210401,38.560001,-121.300003,279.200012
20210401,38.560001,-121.300003,0.482687
20210401,38.560001,-121.300003,0.005781
EOF

odc import data-3.csv data-3.odb

[[ $(odc count data-3.odb) -eq 5 ]] || (echo "Unexpected number of rows in ODB data-3.odb" ; false)

# Helper functions

expect_error () {
    exit_code="${exit_code-0}"
    [[ "$exit_code" -eq 1 ]] || (echo "Unexpected exit code: $exit_code != 1" ; false)
    unset exit_code
}

expect_success () {
    exit_code="${exit_code-1}"
    [[ "$exit_code" -eq 0 ]] || (echo "Unexpected exit code: $exit_code != 0" ; false)
    unset exit_code
}

# Compact tool

odc compact || exit_code=$? ; expect_error
odc compact data-1.odb || exit_code=$? ; expect_error
odc compact data-1.odb data-compacted.odb && exit_code=$? ; expect_success
odc compact data-1.odb data-compacted.odb foobar || exit_code=$? ; expect_error
odc compact foobar data-compacted.odb || exit_code=$? ; expect_error
odc help compact && exit_code=$? ; expect_success

# Compare tool

odc compare || exit_code=$? ; expect_error
odc compare data-1.odb || exit_code=$? ; expect_error
odc compare data-1.odb data-2.odb && exit_code=$? ; expect_success
odc compare -dontCheckMissing data-1.odb data-2.odb && exit_code=$? ; expect_success
odc compare data-1.odb data-2.odb foobar || exit_code=$? ; expect_error
odc compare foobar data-2.odb || exit_code=$? ; expect_error
odc compare data-1.odb foobar || exit_code=$? ; expect_error
odc help compare && exit_code=$? ; expect_success

# Count tool

odc count || exit_code=$? ; expect_error
odc count data-1.odb && exit_code=$? ; expect_success
odc count foobar || exit_code=$? ; expect_error
odc count data-1.odb foobar || exit_code=$? ; expect_error
odc help count && exit_code=$? ; expect_success

# Index tool

odc index || exit_code=$? ; expect_error
#
# TODO: Check why this command does not work
#
# odc index data-3.odb && exit_code=$? ; expect_success
# odc index data-3.odb data-3.odb.idx && exit_code=$? ; expect_success
#
odc index data-3.odb data-3.odb.idx foobar || exit_code=$? ; expect_error
odc index foobar || exit_code=$? ; expect_error
odc help index && exit_code=$? ; expect_success

# Fixrowsize tool

odc fixrowsize || exit_code=$? ; expect_error
odc fixrowsize data-1.odb || exit_code=$? ; expect_error
odc fixrowsize data-1.odb data-fixed.odb && exit_code=$? ; expect_success
odc fixrowsize data-1.odb data-fixed.odb foobar || exit_code=$? ; expect_error
odc fixrowsize foobar data-fixed.odb || exit_code=$? ; expect_error
odc help fixrowsize && exit_code=$? ; expect_success

# Import tool

odc import || exit_code=$? ; expect_error
odc import data-3.csv || exit_code=$? ; expect_error
odc import data-3.csv data-4.odb && exit_code=$? ; expect_success
odc import data-3.csv data-4.odb foobar || exit_code=$? ; expect_error
odc import -d , data-3.csv data-4.odb && exit_code=$? ; expect_success
odc import foobar data-4.odb || exit_code=$? ; expect_error
odc help import && exit_code=$? ; expect_success

# List tool

odc ls || exit_code=$? ; expect_error
odc ls data-1.odb && exit_code=$? ; expect_success
odc ls data-1.odb foobar || exit_code=$? ; expect_error
odc ls -o data-1.txt data-1.odb && exit_code=$? ; expect_success
odc ls foobar || exit_code=$? ; expect_error
odc help ls && exit_code=$? ; expect_success

# Mdset tool

odc mdset || exit_code=$? ; expect_error
odc mdset "col6:INTEGER=1" || exit_code=$? ; expect_error
odc mdset "col6:INTEGER=1" data-1.odb || exit_code=$? ; expect_error
odc mdset "col6:INTEGER=1" data-1.odb data-set.odb && exit_code=$? ; expect_success
odc mdset "col6:INTEGER=1" data-1.odb data-set.odb foobar || exit_code=$? ; expect_error
odc mdset "col6:INTEGER=1" foobar data-set.odb || exit_code=$? ; expect_error
odc help mdset && exit_code=$? ; expect_success

# Merge tool

odc merge || exit_code=$? ; expect_error
odc merge -o data-merged.odb || exit_code=$? ; expect_error
odc merge -o data-merged.odb data-1.odb || exit_code=$? ; expect_error
odc merge -o data-merged.odb data-1.odb data-3.odb && exit_code=$? ; expect_success
odc merge -o data-merged.odb data-1.odb foobar || exit_code=$? ; expect_error
odc merge -o data-merged.odb foobar data-3.odb || exit_code=$? ; expect_error
odc merge data-1.odb data-3.odb && exit_code=$? ; expect_success
odc help merge && exit_code=$? ; expect_success

# Header tool

odc header || exit_code=$? ; expect_error
odc header data-1.odb && exit_code=$? ; expect_success
odc header -ddl data-1.odb && exit_code=$? ; expect_success
odc header data-1.odb data-2.odb || exit_code=$? ; expect_error
odc header data-1.odb foobar || exit_code=$? ; expect_error
odc header foobar || exit_code=$? ; expect_error
odc help header && exit_code=$? ; expect_success

# SQL tool

odc sql || exit_code=$? ; expect_error
odc sql -i data-1.odb || exit_code=$? ; expect_error
odc sql -i data-1.odb "select col1" && exit_code=$? ; expect_success
odc sql -T -i data-1.odb "select col1" && exit_code=$? ; expect_success
odc sql -i data-1.odb "select col1" foobar || exit_code=$? ; expect_error
odc sql -i foobar "select col1" || exit_code=$? ; expect_error
odc help sql && exit_code=$? ; expect_success

# Set tool

odc set || exit_code=$? ; expect_error
odc set "col1=1" || exit_code=$? ; expect_error
odc set "col1=1" data-1.odb || exit_code=$? ; expect_error
odc set "col1=1" data-1.odb data-set.odb && exit_code=$? ; expect_success
odc set "col1=1" data-1.odb data-set.odb foobar || exit_code=$? ; expect_error
odc set "col1=1" data-1.odb data-set.odb foobar || exit_code=$? ; expect_error
odc set "colX=1" data-1.odb data-set.odb || exit_code=$? ; expect_error
odc help set && exit_code=$? ; expect_success

# Split tool

odc split || exit_code=$? ; expect_error
odc split data-1.odb || exit_code=$? ; expect_error
odc split data-1.odb data-1.{col1}.{col2}.odb && exit_code=$? ; expect_success
odc split data-1.odb data-1.{col1}.{col2}.odb foobar || exit_code=$? ; expect_error
odc split data-1.odb foobar || exit_code=$? ; expect_error
odc split foobar data-1.{col1}.{col2}.odb || exit_code=$? ; expect_error
odc help split && exit_code=$? ; expect_success

# XYV tool

odc xyv || exit_code=$? ; expect_error
odc xyv data-3.odb || exit_code=$? ; expect_error
odc xyv data-3.odb "obsvalue@body" || exit_code=$? ; expect_error
odc xyv data-3.odb "obsvalue@body" data-xyv.odb && exit_code=$? ; expect_success
odc xyv data-3.odb "obsvalue@body" data-xyv.odb foobar || exit_code=$? ; expect_error
odc xyv foobar "obsvalue@body" data-xyv.odb || exit_code=$? ; expect_error
odc xyv data-3.odb foobar data-xyv.odb || exit_code=$? ; expect_error
odc help xyv && exit_code=$? ; expect_success

# Clean up

cd ${wd}
rm -rf ${test_wd}
