CC=$(CFLAGS)
#YFLAGS=-t

INSTALLLIBDIR=../lib

DEPLIB= $(INSTALLLIBDIR)/libEc.a $(INSTALLLIBDIR)/libOdb.a
INCLUDE=-I../odalib -I../eclib
BINLIB=-L$(INSTALLLIBDIR) -lEc -lOdb 

PROGRAM.TARGET=odb

SRC.cc= \
TestTEMPLATE.cc \
TestMetaDataReader.cc \
TestFunctionNorm.cc \
TestFunctionDotp.cc \
TestFunctionTypeConversion.cc \
TestFunctionDateAndTime.cc \
TestFunctionTdiff.cc \
TestFunctionThin.cc \
TestFunctionCircle.cc \
TestFunctionRggBox.cc \
TestFunctionEqBox.cc \
TestFunctionDistance.cc \
TestFunctionsForTemperatureConversion.cc \
TestFunctionsForAngleConversion.cc \
TestAAAImportODB.cc \
TestAAAImportODBDispatching.cc \
TestInMemoryDataHandle.cc \
TestCommandLineParsing.cc \
TestSelectIterator.cc \
TestSelectIterator2.cc \
TestSelectIterator3.cc \
TestBitfields.cc \
TestSelectStarAt.cc \
TestStar.cc \
TestCatFiles.cc \
TestWriteCatFiles.cc \
TestMinMax.cc \
TestConstCodec.cc \
TestCodec.cc \
TestCodecOptimization.cc \
TestMetaData.cc \
TestConstIntegerCodec.cc \
TestInt16_MissingCodec.cc \
TestDispatchingWriter.cc \
TestAggregateFunctions.cc \
TestAggregateFunctions2.cc \
TestAggregateFunctions3.cc \
TestDistinct.cc \
TestOrderBy.cc \
TestSimpleFilterIterator.cc \
TestDecoding.cc \
TestSelectDataHandle.cc \
TestTextSelect.cc \
TestTextSelect2.cc \
TestTextSelect3.cc \
TestSimpleFilterIterator2.cc \
TestOdaCAPI.cc \
TestSQLFunctionsInfo.cc \
TestIntegerValues.cc \
TestFastODA2Request.cc \
TestFastODA2Request2.cc \
TestFastODA2Request3.cc \
TestAtTableInTheOutput.cc \
TestMissingValue.cc \
TestSelectTwoFiles.cc \
TestSetvbuffer.cc \
UnitTests.cc


SRC.c=
PERSIST=
LINK=


all :: program

include ../make.mars

install: all
	if [ -e $(INSTALLBINDIR) ]; then echo Directory $(INSTALLBINDIR) already exists. You have probably forgot to change version number. && exit 1; fi
	mkdir -p -m 755 $(INSTALLBINDIR) 
	cp -p -f $(PROGRAM.TARGET) $(INSTALLBINDIR)
	chmod -R a+rx $(INSTALLBINDIR)
	find  $(INSTALLBINDIR) -type d -exec chmod a+rx {} \;
	find  $(INSTALLBINDIR) -type f -exec chmod a+r {} \;

clean::
	rm -f sqll.c sqly.c

%.cc : junk/%.cc
	rsh leda "echo 'cd /ccvobs/mars/mars/src/sql;cleartool mv junk/$@ .' | cleartool setview mars_server_linux"
%.h : junk/%.h
	rsh leda "echo 'cd /ccvobs/mars/mars/src/sql;cleartool mv junk/$@ .' | cleartool setview mars_server_linux"
