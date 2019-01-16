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
ODA2RequestTool.cc
ODA2RequestTool.h
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
odc_tools_c.cc
odc_tools_c.h
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
TestFastODA2Request.cc
TestFastODA2Request2.cc
TestFastODA2Request3.cc
TestFunctionDateAndTime.cc
TestFunctionDotp.cc
TestFunctionTdiff.cc
TestFunctionThin.cc
TestFunctionTypeConversion.cc
TestFunctionsForAngleConversion.cc
TestFunctionsForTemperatureConversion.cc
TestInMemoryDataHandle.cc
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

include_directories( ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../odc )

ecbuild_add_library( TARGET     odctools
                     INSTALL_HEADERS LISTED
                     HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/odc/tools
                     SOURCES    ${odctools_src_files}
                     TEMPLATES  ${odctools_templates}
                     LIBS       odccore )

ecbuild_add_library( TARGET     odctest
                     #INSTALL_HEADERS LISTED
                     #HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/odc/tools
                     SOURCES    ${odctest_src_files}
                     TEMPLATES  ${odctest_templates}
                     LIBS       odccore odctools )

## TODO: Re-enable odctest

ecbuild_add_executable( TARGET    odc
                        SOURCES   odc.cc
                        LIBS      odccore odctools ) ### odctest )
