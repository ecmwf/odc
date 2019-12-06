#!/bin/bash

set -uex

# A unique working directory

wd=$(pwd)
test_wd=$(pwd)/test_odb_463

mkdir -p ${test_wd}
cd ${test_wd}

# Test doing a split operation

odc split ../odb_463.odb ./out.{reportype}.odb

# Check that all of the expected files are created

[[ "$(ls out.*.odb | wc -l)" != "21" ]] && echo "Incorrect number of files created by split" && exit -1
for rt in 16001 16002 16004 16005 16006 16008 16009 16012 16017 16022 16025 16026 16029 16045 16065 16068 16074 16076 16082 16083 16084; do
    [[ ! -f "out.${rt}.odb" ]] && "expected split output 'out.${rt}.odb' not found" && exit -1
done

cat > expected_wigosid <<EOF
'                                '
''
'0-376-0-502                     '
'0-376-0-507                     '
'0-376-0-520                     '
'0-376-0-521                     '
'0-376-0-528                     '
'0-376-0-530                     '
'0-376-0-531                     '
'0-376-0-541                     '
'0-376-0-550                     '
'0-376-0-560                     '
'0-376-0-563                     '
'0-376-0-567                     '
'0-376-0-568                     '
'0-376-0-582                     '
'0-376-0-588                     '
'0-376-0-594                     '
'0-376-0-606                     '
'0-376-0-608                     '
'0-376-0-617                     '
'0-376-0-619                     '
'0-376-0-620                     '
'0-376-0-621                     '
'0-376-0-628                     '
'0-376-0-642                     '
'0-376-0-644                     '
'0-376-0-653                     '
'0-376-0-654                     '
'0-376-0-656                     '
'0-376-0-658                     '
'0-376-0-661                     '
'0-376-0-662                     '
'0-376-0-672                     '
'0-376-0-674                     '
'0-376-0-678                     '
'0-376-0-687                     '
wigosid@hdr
EOF

# Check that the WIGOS ids in the files are all there, and complete (i.e. not truncated, or any extras)

odc sql 'select distinct wigosid' -i ../odb_463.odb | sed 's/ *$//g' | sort -u > reduced_sql

for f in out.*.odb; do odc sql 'select distinct wigosid' -i $f; done | sed 's/ *$//g' | sort -u > split_sql

diff reduced_sql expected_wigosid

diff split_sql expected_wigosid

# Clean up

cd ${wd}
rm -rf ${test_wd}
