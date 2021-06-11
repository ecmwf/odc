#!/bin/bash

set -uex

# A unique working directory

wd=$(pwd)
test_wd=$(pwd)/test_odb_sql_format

mkdir -p ${test_wd}
cd ${test_wd}

# In case we are resuming from a previous failed run, which has left output in the directory
rm *.odb || true

# Test that sql command takes specified output format into account
#   See ODB-521

# Create some test data

cat > data.csv <<EOF
col1:INTEGER,col2:REAL,col3:DOUBLE,col4:INTEGER
1,1.23,4.56,7
123,0.0,0.0,321
321,0.0,0.0,123
0,3.25,0.0,0
0,0.0,3.25,0
EOF

odc import data.csv data.odb

# Runs some selects, to see that the data is correct

odc sql 'select * where col1=1' -i data.odb -f odb -o outfile.odb

cat > expect.csv <<EOF
col1:INTEGER,col2:REAL,col3:DOUBLE,col4:INTEGER
1,1.23,4.56,7
EOF
odc import expect.csv expect.odb
odc compare outfile.odb expect.odb

# --

# Check default output

odc sql 'select *' -i data.odb > ascii_out1.txt
odc sql 'select *' -i data.odb -f default > ascii_out2.txt
odc sql 'select *' -i data.odb -o odb_out1.odb
odc sql 'select *' -i data.odb -o odb_out2.odb -f default

cat > ascii_reference.txt <<EOF
          col1	          col2	          col3	          col4
             1	      1.230000	      4.560000	             7
           123	      0.000000	      0.000000	           321
           321	      0.000000	      0.000000	           123
             0	      3.250000	      0.000000	             0
             0	      0.000000	      3.250000	             0
EOF

cmp ascii_out1.txt ascii_reference.txt
cmp ascii_out2.txt ascii_reference.txt
odc compare odb_out1.odb data.odb
odc compare odb_out2.odb data.odb

# Check wide output

odc sql 'select *' -i data.odb -f wide > wide_out1.txt
odc sql 'select *' -i data.odb -o wide_out2.txt -f wide

cat > wide_reference.txt <<EOF
  col1:integer	     col2:real	   col3:double	  col4:integer
             1	      1.230000	      4.560000	             7
           123	      0.000000	      0.000000	           321
           321	      0.000000	      0.000000	           123
             0	      3.250000	      0.000000	             0
             0	      0.000000	      3.250000	             0
EOF

cmp wide_out1.txt wide_reference.txt
cmp wide_out2.txt wide_reference.txt

# Check ascii output

odc sql 'select *' -i data.odb -o ascii_out3.txt -f ascii

cmp ascii_out3.txt ascii_reference.txt

# Check odb output

odc sql 'select *' -i data.odb -o odb_out3.odb -f odb

odc compare odb_out3.odb data.odb

if [[ ! "$(odc sql 'select *' -i data.odb -f odb)" ]]; then
    echo "Got success. Expected 'odb' output format without output file to fail"
    exit -1
fi

# Clean up

cd ${wd}
rm -rf ${test_wd}
