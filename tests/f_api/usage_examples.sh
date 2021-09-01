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

# Test Fortran encode row-major example

${wd}/odc-fortran-encode-row-major > encode-row-major-usage.txt || exit_code=$? ; expect_error

cat > encode-row-major-usage-expected.txt <<EOF
 Usage:
     odc-fortran-encode-row-major <odb2 output file>
EOF

cmp encode-row-major-usage.txt encode-row-major-usage-expected.txt

${wd}/odc-fortran-encode-row-major ${test_wd}/test-1.odb > encode-row-major.txt && exit_code=$? ; expect_success

cat > encode-row-major-expected.txt <<EOF
Written 20 rows to $test_wd/test-1.odb
EOF

cmp encode-row-major.txt encode-row-major-expected.txt

# Test Fortran encode custom example

${wd}/odc-fortran-encode-custom > encode-custom-usage.txt || exit_code=$? ; expect_error

cat > encode-custom-usage-expected.txt <<EOF
 Usage:
     odc-fortran-encode-custom <odb2 output file>
EOF

cmp encode-custom-usage.txt encode-custom-usage-expected.txt

${wd}/odc-fortran-encode-custom ${test_wd}/test-2.odb > encode-custom.txt && exit_code=$? ; expect_success

cat > encode-custom-expected.txt <<EOF
Written 20 rows to $test_wd/test-2.odb
EOF

cmp encode-custom.txt encode-custom-expected.txt

# Test Fortran list program example

${wd}/odc-fortran-ls > ls-usage.txt || exit_code=$? ; expect_error

cat > ls-usage-expected.txt <<EOF
 Usage:
     odc-fortran-ls <odb2 file>
EOF

cmp ls-usage.txt ls-usage-expected.txt

${wd}/odc-fortran-ls ${test_wd}/test-1.odb > ls.txt && exit_code=$? ; expect_success

# Remove non-printable binary characters from the output
tr -cd '\11\12\15\40-\176' < ls.txt > ls-ascii.txt

current_date=$(date +"%Y%m%d")

cat > ls-expected.txt <<EOF
1. expver	2. date@hdr	3. statid@hdr	4. wigos@hdr	5. obsvalue@body	6. integer_missing	7. double_missing	8. bitfield_column	
xxxx	$current_date	stat00	0-12345-0-67800	0.0000	1234	12.3400	0000001	
xxxx	$current_date	stat01	0-12345-0-67801	12.3456	4321	43.2100	0001011	
xxxx	$current_date	stat02	0-12345-0-67802	24.6912	.	.	1101011	
xxxx	$current_date	stat03	0-12345-0-67803	37.0368	1234	12.3400	0000001	
xxxx	$current_date	stat04	0-12345-0-67804	49.3824	4321	43.2100	0001011	
xxxx	$current_date	stat05	0-12345-0-67805	61.7280	.	.	1101011	
xxxx	$current_date	stat06	0-12345-0-67806	74.0736	1234	12.3400	0000001	
xxxx	$current_date	stat07	0-12345-0-67807	86.4192	4321	43.2100	0001011	
xxxx	$current_date	stat08	0-12345-0-67808	98.7648	.	.	1101011	
xxxx	$current_date	stat09	0-12345-0-67809	111.1104	1234	12.3400	0000001	
xxxx	$current_date	stat10	0-12345-0-67810	123.4560	4321	43.2100	0001011	
xxxx	$current_date	stat11	0-12345-0-67811	135.8016	.	.	1101011	
xxxx	$current_date	stat12	0-12345-0-67812	148.1472	1234	12.3400	0000001	
xxxx	$current_date	stat13	0-12345-0-67813	160.4928	4321	43.2100	0001011	
xxxx	$current_date	stat14	0-12345-0-67814	172.8384	.	.	1101011	
xxxx	$current_date	stat15	0-12345-0-67815	185.1840	1234	12.3400	0000001	
xxxx	$current_date	stat16	0-12345-0-67816	197.5296	4321	43.2100	0001011	
xxxx	$current_date	stat17	0-12345-0-67817	209.8752	.	.	1101011	
xxxx	$current_date	stat18	0-12345-0-67818	222.2208	1234	12.3400	0000001	
xxxx	$current_date	stat19	0-12345-0-67819	234.5664	4321	43.2100	0001011	
EOF

cmp ls-ascii.txt ls-expected.txt

# Test Fortran header stats example

${wd}/odc-fortran-header > header-usage.txt || exit_code=$? ; expect_error

cat > header-usage-expected.txt <<EOF
 Usage:
     odc-fortran-header <odb2 file 1> [<odb2 file 2> ...]
EOF

cmp header-usage.txt header-usage-expected.txt

${wd}/odc-fortran-header ${test_wd}/test-1.odb ${test_wd}/test-2.odb > header.txt && exit_code=$? ; expect_success

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
