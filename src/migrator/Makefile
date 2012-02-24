CC=$(ODB_CPLUSPLUS) $(CPPFLAGS)
FC=pgf

INSTALLLIBDIR=/var/tmp/tmpdir/stf/p4w/od/oda/src/lib
INSTALLLIBDIR=../lib

DEPLIB= $(INSTALLLIBDIR)/libEc.a $(INSTALLLIBDIR)/libOdb.a
INCLUDE=-I../eclib -I../odalib -I../oda $(ODB_INCLUDE)

# I put '-lifsaux'  in front of other libs required by ODB cause it helps to link on Suse91
# $ uname -a
# Linux drn05 2.6.5-7.308-smp #1 SMP Mon Dec 10 11:36:40 UTC 2007 x86_64 x86_64 x86_64 GNU/Linux
# 
# g++ (GCC) 4.2.1
# 
BINLIB=-L$(INSTALLLIBDIR) -lEc -lOdb  -lifsaux $(ODB_LIBS)
LIBLIB=-L$(INSTALLLIBDIR) -lEc -lOdb  $(ODB_LIBS)

PROGRAM.TARGET=odb2oda

SRC.cc= \
ODB2ODATool.cc \
ODBIterator.cc \
FakeODBIterator.cc \
ReptypeGenIterator.cc \
ksh.cc \
../oda/Tool.cc \
../oda/CommandLineParser.cc \
../oda/ToolFactory.cc \
../oda/ToolRunnerApplication.cc \
../oda/TestCase.cc \
../oda/TestRunnerApplication.cc \
../oda/TestRunner.cc \
../oda/ODA2RequestTool.cc \
../oda/CountTool.cc \
TestImportODB.cc \


SRC.c=odbdump_sami.c
PERSIST=
LINK=


all :: program
#all :: sharedlib 


include ../make.mars

install:
	cp -p -f $(PROGRAM.TARGET) $(INSTALLBINDIR)
	chmod -R a+rx $(INSTALLBINDIR)

clean::
	rm -f sqll.c sqly.c

%.cc : junk/%.cc
	rsh leda "echo 'cd /ccvobs/mars/mars/src/sql;cleartool mv junk/$@ .' | cleartool setview mars_server_linux"
%.h : junk/%.h
	rsh leda "echo 'cd /ccvobs/mars/mars/src/sql;cleartool mv junk/$@ .' | cleartool setview mars_server_linux"

test::
	time echo ./odb2oda \
		-addcolumns reptype=0,class="'od'",stream="'oper'",type="'oda'" \
		-genreptype sensor@hdr,satname_1@hdr,satname_2@hdr,satname_3@hdr,satname_4@hdr,bufrtype@hdr,subtype@hdr,obstype@hdr,codetype@hdr \
		/hugetmp/data/2009072800/ECMA.conv sql_conv /hugetmp/data/2009072800/ECMA.conv.{reptype}.oda
	#for f in /hugetmp/data/2009072800/ECMA.conv.*.oda; do echo oda oda2request $$f $$f.archive.request; oda oda2request $$f $$f.archive.request; done
	#cd /tmp/p4/mars/client/dev/odb_dump/src && 
	for f in /hugetmp/data/2009072800/ECMA.conv.*.oda.archive.request; do pwd;echo $$f;cat $$f|mars -t;echo; done
