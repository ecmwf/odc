#!/bin/bash

set -uex

# A unique working directory

wd=$(pwd)
test_wd=$(pwd)/test_odb_sql_bitfields

mkdir -p ${test_wd}
cd ${test_wd}

# In case we are resuming from a previous failed run, which has left output in the directory
rm *.odb || true

# Create some test data

cat > data.csv <<EOF
bitfield_column@test:BITFIELD[flag_a:1;flag_b:2;flag_c:5]
999
888
777
666
555
EOF

odc import data.csv data.odb

[[ $(odc count data.odb) -eq 5 ]] || (echo "Unexpected number of rows in ODB" ; false)

# Check that we get what we are expecting

cat > odb_ls_reference <<EOF
bitfield_column@test
999
888
777
666
555
EOF

odc ls data.odb
odc ls data.odb > odb_ls

cmp odb_ls_reference odb_ls

# Check if selecting whole bitfield column works, both with and without table identifier

cat > odb_select_reference <<EOF
bitfield_column@test
                 999
                 888
                 777
                 666
                 555
EOF

odc sql -i data.odb -o odb_select_column -f ascii "select bitfield_column"
odc sql -i data.odb -o odb_select_column_table -f ascii "select bitfield_column@test"

cmp odb_select_reference odb_select_column
cmp odb_select_reference odb_select_column_table

# Check if selecting a specific bitfield flag works, both with and without table identifier

cat > odb_select_reference <<EOF
bitfield_column.flag_a@test
                          1
                          0
                          1
                          0
                          1
EOF

odc sql -i data.odb -o odb_select_flag -f ascii "select bitfield_column.flag_a"
odc sql -i data.odb -o odb_select_flag_table -f ascii "select bitfield_column.flag_a@test"

cmp odb_select_reference odb_select_flag
cmp odb_select_reference odb_select_flag_table

# Check if expanding bitfield column works, both with and without table identifier

cat > odb_select_reference <<EOF
bitfield_column.flag_a@test	bitfield_column.flag_b@test	bitfield_column.flag_c@test
                          1	                          3	                         28
                          0	                          0	                         15
                          1	                          0	                          1
                          0	                          1	                         19
                          1	                          1	                          5
EOF

odc sql -i data.odb -o odb_expand_column -f ascii "select bitfield_column.*"
odc sql -i data.odb -o odb_expand_column_table -f ascii "select bitfield_column.*@test"

cmp odb_select_reference odb_expand_column
cmp odb_select_reference odb_expand_column_table

# Clean up

cd ${wd}
rm -rf ${test_wd}
