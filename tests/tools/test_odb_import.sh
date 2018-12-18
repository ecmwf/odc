#!/bin/bash

set -uex

# A unique working directory

wd=$(pwd)
test_wd=$(pwd)/test_odb_import

mkdir -p ${test_wd}
cd ${test_wd}

# In case we are resuming from a previous failed run, which has left output in the directory
rm *.odb || true

# Create some test data

cat > data.csv <<EOF
col1:INTEGER,col2:REAL,col3:DOUBLE,col4:INTEGER,col5:BITFIELD[a:1;b:2;c:5]
1,1.23,4.56,7,999
123,0.0,0.0,321,888
321,0.0,0.0,123,777
0,3.25,0.0,0,666
0,0.0,3.25,0,555
EOF

odc import data.csv data.odb

[[ $(odc count data.odb) -eq 5 ]] || (echo "Unexpected number of rows in ODB" ; false)

# Check that we get what we are expecting

# n.b. It is important that TABS are literally correct in this output.
# Beware editors that change them by default

cat > odb_ls_reference <<EOF
col1	col2	col3	col4	col5
1	1.230000	4.560000	7	999
123	0.000000	0.000000	321	888
321	0.000000	0.000000	123	777
0	3.250000	0.000000	0	666
0	0.000000	3.250000	0	555
EOF

odc ls data.odb
odc ls data.odb > odb_ls

cmp odb_ls_reference odb_ls

# Clean up

cd ${wd}
rm -rf ${test_wd}
