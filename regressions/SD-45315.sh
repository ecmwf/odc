#!/bin/bash

set -uex

# A unique working directory

wd=$(pwd)
test_wd=$(pwd)/test_sd_45315

mkdir -p ${test_wd}
cd ${test_wd}

# Test running an SQL command that
# i) Selects fields
# ii) Implicitly and explicitly uses tables

odc sql "select anflag.final, anflag.varqc, anflag,status.active@hdr, status.passive@hdr, status.active@body, status.passive@body where statid=' 00247'" -i ../2000010106.odb -o temporary.odb

# And check that we made a change, and reverted it

odc compare ../odb_45315.sh temporary.odb

# Clean up

cd ${wd}
rm -rf ${test_wd}
