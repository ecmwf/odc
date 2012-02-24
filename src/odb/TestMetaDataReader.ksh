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

cat >TestMetaDataReader1.csv <<@@
x:INTEGER
1
@@
./odb import TestMetaDataReader1.csv TestMetaDataReader1.odb

cat >TestMetaDataReader2.csv <<@@
x:INTEGER
2
@@
./odb import TestMetaDataReader2.csv TestMetaDataReader2.odb

cat >TestMetaDataReader3.csv <<@@
x:INTEGER
3
@@
./odb import TestMetaDataReader3.csv TestMetaDataReader3.odb
cat TestMetaDataReader1.odb TestMetaDataReader2.odb TestMetaDataReader3.odb >TestMetaDataReader.odb

./odb sql select \* from \"TestMetaDataReader.odb\"

log +++ All tests completed OK
