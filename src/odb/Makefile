CC=$(CFLAGS)
#YFLAGS=-t

INSTALLLIBDIR=../lib

DEPLIB= $(INSTALLLIBDIR)/libEc.a $(INSTALLLIBDIR)/libOdb.a
INCLUDE=-I../odalib -I../eclib
BINLIB=-L$(INSTALLLIBDIR) -lEc -lOdb 

PROGRAM.TARGET=odb

SRC.cc= \
Tool.cc \
CommandLineParser.cc \
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
TestCase.cc \
TestAAAImportODB.cc \
TestAAAImportODBDispatching.cc \
TestRunnerApplication.cc \
TestRunner.cc \
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
TestAtTableInTheOutput.cc \
TestMissingValue.cc \
TestSelectTwoFiles.cc \
ToolFactory.cc \
ToolRunnerApplication.cc \
CompactTool.cc \
SetTool.cc \
ImportTool.cc \
SplitTool.cc \
FixedSizeRowTool.cc \
CompareTool.cc \
MergeTool.cc \
ODAHeaderTool.cc \
CountTool.cc \
LSTool.cc \
SQLTool.cc \
XYVTool.cc \
ODA2RequestTool.cc \
SQLServerTool.cc \
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
