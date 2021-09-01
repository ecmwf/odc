#!/bin/bash

set -uex

# A unique working directory

wd=$(pwd)
test_wd=$(pwd)/usage_examples

mkdir -p ${test_wd}
cd ${test_wd}

# In case we are resuming from a previous failed run, which has left output in the directory
rm *.odb *.txt || true

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

# Test C encode row-major example

${wd}/odc-c-encode-row-major 2> encode-row-major-usage.txt || exit_code=$? ; expect_error

cat > encode-row-major-usage-expected.txt <<EOF
Usage:
    odc-c-encode-row-major <odb2 output file>

EOF

cmp encode-row-major-usage.txt encode-row-major-usage-expected.txt

${wd}/odc-c-encode-row-major ${test_wd}/test-1.odb > encode-row-major.txt && exit_code=$? ; expect_success

cat > encode-row-major-expected.txt <<EOF
Written 20 rows to $test_wd/test-1.odb
EOF

cmp encode-row-major.txt encode-row-major-expected.txt

# Test C encode custom example

${wd}/odc-c-encode-custom 2> encode-custom-usage.txt || exit_code=$? ; expect_error

cat > encode-custom-usage-expected.txt <<EOF
Usage:
    odc-c-encode-custom <odb2 output file>

EOF

cmp encode-custom-usage.txt encode-custom-usage-expected.txt

${wd}/odc-c-encode-custom ${test_wd}/test-2.odb > encode-custom.txt && exit_code=$? ; expect_success

cat > encode-custom-expected.txt <<EOF
Written 20 rows to $test_wd/test-2.odb
EOF

cmp encode-custom.txt encode-custom-expected.txt

# Test C list program example

${wd}/odc-c-ls 2> ls-usage.txt || exit_code=$? ; expect_error

cat > ls-usage-expected.txt <<EOF
Usage:
    odc-c-ls <odb2 file>

EOF

cmp ls-usage.txt ls-usage-expected.txt

${wd}/odc-c-ls ${test_wd}/test-1.odb > ls.txt && exit_code=$? ; expect_success

current_date=$(date +"%Y%m%d")

cat > ls-expected.txt <<EOF
1. expver    	2. date@hdr  	3. statid@hdr	4. wigos@hdr         	5. obsvalue@body	6. integer_missing	7. double_missing	8. bitfield_column	
xxxx	$current_date	stat00	0-12345-0-67800	0.000000	1234    	12.340000	0000001	
xxxx	$current_date	stat01	0-12345-0-67801	12.345600	4321    	43.209999	0001011	
xxxx	$current_date	stat02	0-12345-0-67802	24.691200	.	.	1101011	
xxxx	$current_date	stat03	0-12345-0-67803	37.036800	1234    	12.340000	0000001	
xxxx	$current_date	stat04	0-12345-0-67804	49.382401	4321    	43.209999	0001011	
xxxx	$current_date	stat05	0-12345-0-67805	61.728001	.	.	1101011	
xxxx	$current_date	stat06	0-12345-0-67806	74.073601	1234    	12.340000	0000001	
xxxx	$current_date	stat07	0-12345-0-67807	86.419197	4321    	43.209999	0001011	
xxxx	$current_date	stat08	0-12345-0-67808	98.764801	.	.	1101011	
xxxx	$current_date	stat09	0-12345-0-67809	111.110397	1234    	12.340000	0000001	
xxxx	$current_date	stat10	0-12345-0-67810	123.456001	4321    	43.209999	0001011	
xxxx	$current_date	stat11	0-12345-0-67811	135.801605	.	.	1101011	
xxxx	$current_date	stat12	0-12345-0-67812	148.147202	1234    	12.340000	0000001	
xxxx	$current_date	stat13	0-12345-0-67813	160.492798	4321    	43.209999	0001011	
xxxx	$current_date	stat14	0-12345-0-67814	172.838394	.	.	1101011	
xxxx	$current_date	stat15	0-12345-0-67815	185.184006	1234    	12.340000	0000001	
xxxx	$current_date	stat16	0-12345-0-67816	197.529602	4321    	43.209999	0001011	
xxxx	$current_date	stat17	0-12345-0-67817	209.875198	.	.	1101011	
xxxx	$current_date	stat18	0-12345-0-67818	222.220795	1234    	12.340000	0000001	
xxxx	$current_date	stat19	0-12345-0-67819	234.566406	4321    	43.209999	0001011	
EOF

cmp ls.txt ls-expected.txt

# Test C header stats example

${wd}/odc-c-header 2> header-usage.txt || exit_code=$? ; expect_error

cat > header-usage-expected.txt <<EOF
Usage:
    odc-c-header <odb2 file 1> [<odb2 file 2> ...]

EOF

cmp header-usage.txt header-usage-expected.txt

${wd}/odc-c-header ${test_wd}/test-1.odb ${test_wd}/test-2.odb > header.txt && exit_code=$? ; expect_success

odc_version=$(odc --version | head -n 1 | sed "s/^ODBAPI Version: \([0-9.]\)/\1/")

cat > header-expected.txt <<EOF
File: $test_wd/test-1.odb
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

File: $test_wd/test-2.odb
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

EOF

cmp header.txt header-expected.txt

# Clean up

cd ${wd}
rm -rf ${test_wd}
