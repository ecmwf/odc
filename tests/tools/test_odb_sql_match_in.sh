#!/bin/bash

set -uex

# A unique working directory

wd=$(pwd)
test_wd=$(pwd)/test_odb_sql_match_in

mkdir -p ${test_wd}
cd ${test_wd}

# In case we are resuming from a previous failed run, which has left output in the directory
rm *.odb || true


# Create some test data

cat > data1.csv <<EOF
col1:INTEGER,col2:INTEGER
1,200
3,400
5,600
7,800
9,123
EOF

cat > data2.csv <<EOF
col1:INTEGER,col2:INTEGER
5,6
1,2
7,8
EOF

odb import data1.csv data1.odb
odb import data2.csv data2.odb

# Test select query

odb sql 'select col2 where match (col1) in query (select col1 from "data2.odb")' -i data1.odb -f odb -o outfile.odb

cat > expect.csv <<EOF
col2:INTEGER
200
600
800
EOF

odb import expect.csv expect.odb

odb compare outfile.odb expect.odb

# Clean up

cd ${wd}
rm -rf ${test_wd}
