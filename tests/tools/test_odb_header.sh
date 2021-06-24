#!/bin/bash

set -uex

# A unique working directory

wd=$(pwd)
test_wd=$(pwd)/test_odb_header

mkdir -p ${test_wd}
cd ${test_wd}

# In case we are resuming from a previous failed run, which has left output in the directory
rm *.odb || true

# Create some test data

cat > data.csv <<EOF
integer_column:INTEGER,double_column:DOUBLE,integer_missing:INTEGER,double_missing:DOUBLE
0,0.0,2147483647,-2147483647
EOF

odc import data.csv data.odb

[[ $(odc count data.odb) -eq 1 ]] || (echo "Unexpected number of rows in ODB data.odb" ; false)

# Header tool

odc header data.odb > header.txt

cat > header-expected.txt <<EOF

Header 1. Begin offset: 0, end offset: 372, number of rows in block: 1, byteOrder: same
0. name: integer_column, type: INTEGER, codec: constant, value=0.000000, hasMissing=false
1. name: double_column, type: DOUBLE, codec: constant, value=0.000000, hasMissing=false
2. name: integer_missing, type: INTEGER, codec: constant_or_missing, value=NULL, hasMissing=true, missingValue=2147483647.000000
3. name: double_missing, type: DOUBLE, codec: real_constant_or_missing, value=NULL, hasMissing=true, missingValue=-2147483647.000000
EOF

cmp header.txt header-expected.txt

# Clean up

cd ${wd}
rm -rf ${test_wd}
