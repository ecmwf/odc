#!/bin/ksh

set -ex

which odb
which odbsql
which odb2_to_odb1.x
which create_ioassign

if [[ ! -d odb2_to_odb1/ECMA.conv ]]; then
    gzip -d < odb2_to_odb1.ECMA.conv.tar.gz | tar xf -
fi

cd odb2_to_odb1/ECMA.conv || exit 1


rm -fr [0-9]* ECMA.* IOASSIGN{,.ECMA}

create_ioassign -l ECMA -d . -n 160

odb2_to_odb1.x -i conv -t groupid\=17.tables -o ECMA -npools 160

typeset -F count1 count2

count1=$(odbsql -T -q "SELECT count(*) FROM body")
count2=$(odb sql -T "SELECT count(*)" -i <(cat conv.[0-9]*.odb))

if [[ $count1 != $count2 ]]; then
    echo "Number of rows differs: count1=$count1, count2=$count2"
    exit 1
fi
