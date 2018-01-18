#!/bin/bash

set -uex

# A unique working directory

wd=$(pwd)
test_wd=$(pwd)/test_odb_sql_like

mkdir -p ${test_wd}
cd ${test_wd}

# In case we are resuming from a previous failed run, which has left output in the directory
rm *.odb || true


# Create some test data

cat > data.csv <<EOF
col1:STRING,col2:INTEGER
'abra',1
'cadabra',2
'czary',3
'mary',4
EOF

odb import data.csv data.odb

# Test select query

odb sql 'select col2 where col1 like "ra"' -i data.odb -f odb -o outfile.odb

cat > expect.csv <<EOF
col2:INTEGER
1
2
EOF

odb import expect.csv expect.odb

odb compare outfile.odb expect.odb

# Clean up

cd ${wd}
rm -rf ${test_wd}
