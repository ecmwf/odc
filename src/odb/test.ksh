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

log +++ Test: odb set

export INPUT=saved.disp.4.0.oda
export OUTPUT=disp.4.0.modified.oda


log Check the input data is there and has the expected values
ls -l $INPUT
./odb header $INPUT | grep expver | grep 0018
./odb header $INPUT | grep andate | grep 20000101
./odb ls -s expver,andate $INPUT | grep 0018 | grep 20000101 >/dev/null

log Create new file with changed values of expver and andate
./odb set "expver@desc='0019    ',andate=20100101" $INPUT $OUTPUT

log Check new files have the correct values
ls -l $OUTPUT
./odb ls -s expver,andate $OUTPUT | head -n 1

./odb sql select expver from \"$OUTPUT\" |uniq #|grep 0019 | head -n 1
./odb sql select andate from \"$OUTPUT\" |uniq #|grep 20100101 | head -n 1

./odb header $OUTPUT | grep expver | grep 0019
./odb header $OUTPUT | grep andate | grep 20100101

log +++ odb set Completed OK


log +++ Test: odb import

cat >test_odb_text_input.csv <<@@
stream:STRING,expver:INTEGER,value:REAL
'001',1,0.1
'001',2,0.2
@@
./odb import test_odb_text_input.csv test_import.odb
./odb sql select \* from \"test_import.odb\"


#grep "VARCHAR(30)\|INT" ../obsdb/obsdb.ddl|xargs|sed 's/ VARCHAR(30)/:STRING/g'|sed 's/ INT/:INTEGER/g'|sed 's/ //g'|sed 's/,/\&/g' >rtt.csv
#python ../obsdb/convertRTT.py >>rtt.csv
#./odb import -d \& rtt.csv rtt.odb
#./odb sql select \* from \"rtt.csv\"

log +++ odb import Completed OK

log +++ Test: SQL Functions
#./odb test functions
log +++ Completed. SQL Functions OK

log +++ Test: Input file command line option: -i. Also, test printing of column names by default
./odb sql select \* -i test_import.odb |grep stream |grep expver|grep value
log +++ odb Completed OK


log +++ Test: Input file command line option: -i. Also, test option -T: do not print column names
[ `./odb sql select \* -i test_import.odb -T|grep stream|wc|awk '{print $1}'` == 0 ]
log +++ odb Completed OK

./odb sql select time,lat,lon,obsvalue  order by time,lat,lon -i 2000010106.1.0.oda.odb -f odb -o order_by_out.odb
./odb header order_by_out.odb

log +++ Test SELECT with mixed aggregated and non-aggregated functions

cat >test_group_by.csv <<@@
stream:STRING,expver:INTEGER,value:REAL
'001',1,0.1
'001',2,0.2
NULL,4,0.4
NULL,5,0.5
@@

./odb import test_group_by.csv test_group_by.odb
./odb sql select stream,min\(expver\) from \"test_group_by.odb\"

log +++ Test portability of the split tool

rm -rf split.*.*.odb
./odb split split_crash_on_andate_and_antime.odb split.{andate}.{antime}.odb
ls -l split.*.*.odb

log +++ odb Completed OK


log +++ All tests completed OK
