list( APPEND odbtools_src_files
ECMLTool.cc
ECMLTool.h
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
odb_api_tools_c.cc
odb_api_tools_c.h
)

list( APPEND odbtest_src_files
Examples.cc
CAPIExamples.cc
UnitTests.cc
TestAggregateFunctions.cc
TestAggregateFunctions.sql
TestAggregateFunctions2.cc
TestAggregateFunctions3.cc
TestAtTableInTheOutput.cc
TestBitfields.cc
TestCodecOptimization.cc
TestCommandLineParsing.cc
TestDataJoin.cc
TestDataLink.cc
TestDataLoader.cc
TestDataPage.cc
TestDataRow.cc
TestDataSelect.cc
TestDataSet.cc
TestDataTable.cc
TestDispatchingWriter.cc
TestDistinct.cc
TestFastODA2Request.cc
TestFastODA2Request2.cc
TestFastODA2Request3.cc
TestFunctionCircle.cc
TestFunctionDateAndTime.cc
TestFunctionDistance.cc
TestFunctionDotp.cc
TestFunctionEqBox.cc
TestFunctionNorm.cc
TestFunctionRggBox.cc
TestFunctionTdiff.cc
TestFunctionThin.cc
TestFunctionTypeConversion.cc
TestFunctionsForAngleConversion.cc
TestFunctionsForTemperatureConversion.cc
TestInMemoryDataHandle.cc
TestInt16_MissingCodec.cc
TestIntegerValues.cc
TestMetaData.cc
TestMetaDataReader.cc
TestMetaDataReader.ksh
TestMissingValue.cc
TestOdaCAPI.cc
TestOdaCAPI.h
TestOrderBy.cc
TestSQLFunctionsInfo.cc
TestSelectDataHandle.cc
TestSelectStarAt.cc
TestSelectTwoFiles.cc
TestSetvbuffer.cc
TestStar.cc
TestTextSelect.cc
TestTextSelect.txt
TestTextSelect2.cc
TestTextSelect2.txt
TestTextSelect3.cc
MockReader.h
MockReader.cc
TestODBModule.cc
)

include_directories( ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../odb_api )

ecbuild_add_library( TARGET     odbtools
                     INSTALL_HEADERS LISTED
                     HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/odb_api/tools
                     SOURCES    ${odbtools_src_files}
                     TEMPLATES  ${odbtools_templates}
                     LIBS       Odb )

ecbuild_add_library( TARGET     odbtest
                     #INSTALL_HEADERS LISTED
                     #HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/odb_api/tools
                     SOURCES    ${odbtest_src_files}
                     TEMPLATES  ${odbtest_templates}
                     LIBS       Odb odbtools )

ecbuild_add_executable( TARGET    odb
                        SOURCES   odb.cc
                        LIBS      Odb odbtools odbtest )
