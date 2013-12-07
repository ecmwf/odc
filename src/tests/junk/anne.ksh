set -e

function log
{
	if [ x$MARS_SMS_LABEL != x ];
	then
		smslabel $MARS_SMS_LABEL "$*";
	else
		echo ++++++++++++++++++++++++++++++++++++++++++++
		echo + $*
		echo ++++++++++++++++++++++++++++++++++++++++++++
	fi 
}

#set -v
set -x

log +++ Test: odb import

cat >TestIntegerValues.csv <<@@
date:INTEGER
20101130
20101201
20101202
@@
./odb import TestIntegerValues.csv TestIntegerValues.odb
./odb sql select \* from \"TestIntegerValues.odb\"

./odb TestIntegerValues

log +++ All tests completed OK
