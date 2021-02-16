#!/bin/bash

set -uex

# A unique working directory

wd=$(pwd)
test_wd=$(pwd)/test_sd_45479

mkdir -p ${test_wd}
cd ${test_wd}

odc sql "select statid, datum_status.active@body, datum_status.passive@body, datum_status.blacklisted@body" -i ../sd_45479_source.odb -o temporary.odb -f odb

# And check that we made a change, and reverted it

odc compare ../sd_45479_compare.odb temporary.odb

# Clean up

cd ${wd}
rm -rf ${test_wd}
