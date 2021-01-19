list( APPEND odctools_src_files
TestCase.h
TestCase.cc
TestRunner.cc
TestRunner.h
TestRunnerApplication.cc
TestRunnerApplication.h
TestRunnerApplication.cfg
CompactTool.cc
CompactTool.h
CompareTool.cc
CompareTool.h
CountTool.cc
CountTool.h
IndexTool.cc
IndexTool.h
FixedSizeRowTool.cc
FixedSizeRowTool.h
ImportTool.cc
ImportTool.h
LSTool.cc
LSTool.h
MDSetTool.cc
MDSetTool.h
MergeTool.cc
MergeTool.h
#ODA2RequestTool.cc
#ODA2RequestTool.h
ODAHeaderTool.cc
ODAHeaderTool.h
SQLTool.cc
SQLTool.h
SetTool.cc
SetTool.h
SplitTool.cc
SplitTool.h
Tool.cc
Tool.h
ToolFactory.cc
ToolFactory.h
ToolRunnerApplication.cc
ToolRunnerApplication.h
XYVTool.cc
XYVTool.h
)

list( APPEND odctest_src_files
Examples.cc
CAPIExamples.cc
UnitTests.cc
TestAggregateFunctions.cc
TestAggregateFunctions.sql
TestAggregateFunctions2.cc
TestAggregateFunctions3.cc
TestAtTableInTheOutput.cc
TestBitfields.cc
TestCommandLineParsing.cc
TestDispatchingWriter.cc
TestDistinct.cc
TestFunctionDateAndTime.cc
TestFunctionTdiff.cc
TestFunctionThin.cc
TestFunctionTypeConversion.cc
TestFunctionsForAngleConversion.cc
TestFunctionsForTemperatureConversion.cc
TestInt16_MissingCodec.cc
TestIntegerValues.cc
TestMetaData.cc
TestMetaDataReader.ksh
TestMissingValue.cc
TestOdaCAPI.cc
TestOdaCAPI.h
TestOrderBy.cc
TestSelectDataHandle.cc
TestSelectStarAt.cc
TestSelectTwoFiles.cc
TestSetvbuffer.cc
TestStar.cc
MockReader.h
MockReader.cc
)

ecbuild_add_library( TARGET     odctools
                     INSTALL_HEADERS LISTED
                     HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/odc/tools
                     SOURCES    ${odctools_src_files}
                     TEMPLATES  ${odctools_templates}
                     PUBLIC_LIBS odccore )

ecbuild_add_library( TARGET     odctest
                     #INSTALL_HEADERS LISTED
                     #HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/odc/tools
                     SOURCES    ${odctest_src_files}
                     TEMPLATES  ${odctest_templates}
                     PUBLIC_LIBS  odccore odctools )

## TODO: Re-enable odctest

ecbuild_add_executable( TARGET    odc
                        SOURCES   odc.cc
                        LIBS      odccore odctools odctest )
