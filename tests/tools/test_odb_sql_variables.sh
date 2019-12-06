#!/bin/bash

set -uex

# A unique working directory

wd=$(pwd)
test_wd=$(pwd)/test_odb_sql_variables

mkdir -p ${test_wd}
cd ${test_wd}

# In case we are resuming from a previous failed run, which has left output in the directory
rm *.odb || true


# Test that variables defined in the schema can be used in queries.
# See ODB-127

cat > schema.hh <<EOF
SET \$var1 = 123;
SET \$var2 = 3.24;
EOF


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

odc sql '#include "schema.hh"; select * where col1=$var1' -i data.odb -f odb -o outfile.odb

cat > expect.csv <<EOF
col1:INTEGER,col2:REAL,col3:DOUBLE,col4:INTEGER
123,0.0,0.0,321
EOF
odc import expect.csv expect.odb
odc compare outfile.odb expect.odb

# --

odc sql '#include "schema.hh"; select * where col2=$var1' -i data.odb -f odb -o outfile.odb

[[ -e outfile.odb && ! -s outfile.odb ]] # Expect zero sized file

# --

odc sql '#include "schema.hh"; select * where col3=$var1' -i data.odb -f odb -o outfile.odb

[[ -e outfile.odb && ! -s outfile.odb ]] # Expect zero sized file

# --

odc sql '#include "schema.hh"; select * where col4=$var1' -i data.odb -f odb -o outfile.odb

cat > expect.csv <<EOF
col1:INTEGER,col2:REAL,col3:DOUBLE,col4:INTEGER
321,0.0,0.0,123
EOF
odc import expect.csv expect.odb
odc compare outfile.odb expect.odb

# --

odc sql '#include "schema.hh"; select * where col1<$var2' -i data.odb -f odb -o outfile.odb

cat > expect.csv <<EOF
col1:INTEGER,col2:REAL,col3:DOUBLE,col4:INTEGER
1,1.23,4.56,7
0,3.25,0.0,0
0,0.0,3.25,0
EOF
odc import expect.csv expect.odb
odc compare outfile.odb expect.odb

# --

odc sql '#include "schema.hh"; select * where col2<$var2' -i data.odb -f odb -o outfile.odb

cat > expect.csv <<EOF
col1:INTEGER,col2:REAL,col3:DOUBLE,col4:INTEGER
1,1.23,4.56,7
123,0.0,0.0,321
321,0.0,0.0,123
0,0.0,3.25,0
EOF
odc import expect.csv expect.odb
odc compare outfile.odb expect.odb

# --

odc sql '#include "schema.hh"; select * where col3<$var2' -i data.odb -f odb -o outfile.odb

cat > expect.csv <<EOF
col1:INTEGER,col2:REAL,col3:DOUBLE,col4:INTEGER
123,0.0,0.0,321
321,0.0,0.0,123
0,3.25,0.0,0
EOF
odc import expect.csv expect.odb
odc compare outfile.odb expect.odb

# --

odc sql '#include "schema.hh"; select * where col4<$var2' -i data.odb -f odb -o outfile.odb

cat > expect.csv <<EOF
col1:INTEGER,col2:REAL,col3:DOUBLE,col4:INTEGER
0,3.25,0.0,0
0,0.0,3.25,0
EOF
odc import expect.csv expect.odb
odc compare outfile.odb expect.odb


# Clean up

cd ${wd}
rm -rf ${test_wd}
