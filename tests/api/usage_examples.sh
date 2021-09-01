#!/bin/bash

set -uex

# A unique working directory

wd=$(pwd)
test_wd=$(pwd)/usage_examples

mkdir -p ${test_wd}
cd ${test_wd}

# In case we are resuming from a previous failed run, which has left output in the directory
rm *.csv *.odb *.txt || true

# Create some test data

cat > span.csv <<EOF
key1:INTEGER,key2:INTEGER,key3:STRING,val1:INTEGER,val2:REAL
1,100,foo,1,1.000000
1,100,foo,2,2.000000
1,100,foo,3,3.000000
1,100,foo,4,4.000000
1,100,foo,5,5.000000
1,100,foo,6,6.000000
1,100,foo,7,7.000000
1,100,foo,8,8.000000
1,100,foo,9,9.000000
EOF

odc import span.csv span.odb

[[ $(odc count span.odb) -eq 9 ]] || (echo "Unexpected number of rows in ODB span.odb" ; false)

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

# --

# Test C++ encode custom example

${wd}/odc-cpp-encode-custom 2> encode-custom-usage.txt || exit_code=$? ; expect_error

cat > encode-custom-usage-expected.txt <<EOF
Usage:
    odc-cpp-encode-custom <odb2 output file>

EOF

cmp encode-custom-usage.txt encode-custom-usage-expected.txt

${wd}/odc-cpp-encode-custom ${test_wd}/test.odb > encode-custom.txt && exit_code=$? ; expect_success

cat > encode-custom-expected.txt <<EOF
Written 20 to $test_wd/test.odb
EOF

cmp encode-custom.txt encode-custom-expected.txt

# Test C++ archival index example

${wd}/odc-cpp-index 2> index-usage.txt || exit_code=$? ; expect_error

cat > index-usage-expected.txt <<EOF
Usage:
    odc-cpp-index <odb2 file>

EOF

cmp index-usage.txt index-usage-expected.txt

${wd}/odc-cpp-index ${test_wd}/span.odb > index.txt && exit_code=$? ; expect_success

cat > index-expected.txt <<EOF
Archival unit: offset=0 length=422
  Key: key1=1 key2=100 key3=foo 
EOF

cmp index.txt index-expected.txt

# Test C++ list program example

${wd}/odc-cpp-ls 2> ls-usage.txt || exit_code=$? ; expect_error

cat > ls-usage-expected.txt <<EOF
Usage:
    odc-cpp-ls <odb2 file>

EOF

cmp ls-usage.txt ls-usage-expected.txt

${wd}/odc-cpp-ls ${test_wd}/test.odb > ls.txt && exit_code=$? ; expect_success

current_date=$(date +"%Y%m%d")

cat > ls-expected.txt <<EOF
1. expver    	2. date@hdr  	3. statid@hdr	4. wigos@hdr         	5. obsvalue@body	6. integer_missing	7. double_missing	8. bitfield_column	
xxxx    	$current_date	stat00  	0-12345-0-67800 	0       	1234    	12.34   	0000001	
xxxx    	$current_date	stat01  	0-12345-0-67801 	12.3456 	4321    	43.21   	0001011	
xxxx    	$current_date	stat02  	0-12345-0-67802 	24.6912 	.	.	1101011	
xxxx    	$current_date	stat03  	0-12345-0-67803 	37.0368 	1234    	12.34   	0000001	
xxxx    	$current_date	stat04  	0-12345-0-67804 	49.3824 	4321    	43.21   	0001011	
xxxx    	$current_date	stat05  	0-12345-0-67805 	61.728  	.	.	1101011	
xxxx    	$current_date	stat06  	0-12345-0-67806 	74.0736 	1234    	12.34   	0000001	
xxxx    	$current_date	stat07  	0-12345-0-67807 	86.4192 	4321    	43.21   	0001011	
xxxx    	$current_date	stat08  	0-12345-0-67808 	98.7648 	.	.	1101011	
xxxx    	$current_date	stat09  	0-12345-0-67809 	111.11  	1234    	12.34   	0000001	
xxxx    	$current_date	stat10  	0-12345-0-67810 	123.456 	4321    	43.21   	0001011	
xxxx    	$current_date	stat11  	0-12345-0-67811 	135.802 	.	.	1101011	
xxxx    	$current_date	stat12  	0-12345-0-67812 	148.147 	1234    	12.34   	0000001	
xxxx    	$current_date	stat13  	0-12345-0-67813 	160.493 	4321    	43.21   	0001011	
xxxx    	$current_date	stat14  	0-12345-0-67814 	172.838 	.	.	1101011	
xxxx    	$current_date	stat15  	0-12345-0-67815 	185.184 	1234    	12.34   	0000001	
xxxx    	$current_date	stat16  	0-12345-0-67816 	197.53  	4321    	43.21   	0001011	
xxxx    	$current_date	stat17  	0-12345-0-67817 	209.875 	.	.	1101011	
xxxx    	$current_date	stat18  	0-12345-0-67818 	222.221 	1234    	12.34   	0000001	
xxxx    	$current_date	stat19  	0-12345-0-67819 	234.566 	4321    	43.21   	0001011	
EOF

cmp ls.txt ls-expected.txt

# Test C++ header stats example

${wd}/odc-cpp-header 2> header-usage.txt || exit_code=$? ; expect_error

cat > header-usage-expected.txt <<EOF
Usage:
    odc-cpp-header <odb2 file 1> [<odb2 file 2> ...]

EOF

cmp header-usage.txt header-usage-expected.txt

${wd}/odc-cpp-header ${test_wd}/test.odb ${test_wd}/span.odb > header.txt && exit_code=$? ; expect_success

odc_version=$(odc --version | head -n 1 | sed "s/^ODBAPI Version: \([0-9.]\)/\1/")

cat > header-expected.txt <<EOF
File: $test_wd/test.odb
  Frame: 1, Row count: 20, Column count: 8
  Property: encoded_by => odc_example
  Property: encoder => odc version $odc_version
    Column: 1, Name: expver, Type: string, Size: 8
    Column: 2, Name: date@hdr, Type: integer, Size: 8
    Column: 3, Name: statid@hdr, Type: string, Size: 8
    Column: 4, Name: wigos@hdr, Type: string, Size: 16
    Column: 5, Name: obsvalue@body, Type: real, Size: 8
    Column: 6, Name: integer_missing, Type: integer, Size: 8
    Column: 7, Name: double_missing, Type: real, Size: 8
    Column: 8, Name: bitfield_column, Type: bitfield, Size: 8
      Bitfield: 1, Name: flag_a, Offset: 0, Nbits: 1
      Bitfield: 2, Name: flag_b, Offset: 1, Nbits: 2
      Bitfield: 3, Name: flag_c, Offset: 3, Nbits: 3
      Bitfield: 4, Name: flag_d, Offset: 6, Nbits: 1

File: $test_wd/span.odb
  Frame: 1, Row count: 9, Column count: 5
    Column: 1, Name: key1, Type: integer, Size: 8
    Column: 2, Name: key2, Type: integer, Size: 8
    Column: 3, Name: key3, Type: string, Size: 8
    Column: 4, Name: val1, Type: integer, Size: 8
    Column: 5, Name: val2, Type: real, Size: 8

EOF

cmp header.txt header-expected.txt

# Clean up

cd ${wd}
rm -rf ${test_wd}
