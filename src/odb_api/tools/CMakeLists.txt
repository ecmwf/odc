list( APPEND odbtools_src_files
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
LetHandler.cc
LetHandler.h
DefineFunctionHandler.cc
DefineFunctionHandler.h
VariableLookupHandler.cc
VariableLookupHandler.h
ArchiveHandler.cc
ArchiveHandler.h
ListHandler.cc
ListHandler.h
PrintHandler.cc
PrintHandler.h
RetrieveHandler.cc
RetrieveHandler.h
CompareHandler.cc
CompareHandler.h
SQLHandler.cc
SQLHandler.h
ODBModule.cc
ODBModule.h
)

list( APPEND odbtest_src_files
UnitTests.cc
TestAAAImportODB.cc
TestAAAImportODBDispatching.cc
TestAggregateFunctions.cc
TestAggregateFunctions.sql
TestAggregateFunctions2.cc
TestAggregateFunctions3.cc
TestAtTableInTheOutput.cc
TestBitfields.cc
TestCatFiles.cc
TestCodec.cc
TestCodecOptimization.cc
TestCommandLineParsing.cc
TestConstCodec.cc
TestConstIntegerCodec.cc
TestDataJoin.cc
TestDataLink.cc
TestDataLoader.cc
TestDataPage.cc
TestDataRow.cc
TestDataSelect.cc
TestDataSet.cc
TestDataTable.cc
TestDecoding.cc
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
TestMinMax.cc
TestMissingValue.cc
TestOdaCAPI.cc
TestOdaCAPI.h
TestOrderBy.cc
TestSQLFunctionsInfo.cc
TestSelectDataHandle.cc
TestSelectIterator.cc
TestSelectIterator2.cc
TestSelectIterator3.cc
TestSelectStarAt.cc
TestSelectTwoFiles.cc
TestSetvbuffer.cc
TestSimpleFilterIterator.cc
TestSimpleFilterIterator2.cc
TestStar.cc
TestTEMPLATE.cc
TestTextSelect.cc
TestTextSelect.txt
TestTextSelect2.cc
TestTextSelect2.txt
TestTextSelect3.cc
TestWriteCatFiles.cc
MockReader.h
MockReader.cc
)

include_directories( ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../odb_api )

ecbuild_add_library( TARGET     odbtools
                     INSTALL_HEADERS LISTED
                     HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/odb_api/tools
                     SOURCES    ${odbtools_src_files}
                     TEMPLATES  ${odbtools_templates}
                     LIBS       Odb eckit )

ecbuild_add_library( TARGET     odbtest
                     #INSTALL_HEADERS LISTED
                     #HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/odb_api/tools
                     SOURCES    ${odbtest_src_files}
                     TEMPLATES  ${odbtest_templates}
                     LIBS       Odb odbtools eckit )

ecbuild_add_executable( TARGET    odb
                        SOURCES   odb.cc
                        LIBS      Odb odbtools odbtest eckit )
