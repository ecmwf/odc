ecbuild_add_test(  TARGET TestAAAImportODB
                   SOURCES TestCase.cc TestCase.h TestAAAImportODB.h TestAAAImportODB.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestAAAImportODBDispatching
                   SOURCES TestCase.cc TestCase.h TestAAAImportODBDispatching.h TestAAAImportODBDispatching.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestAggregateFunctions2
                   SOURCES TestCase.cc TestCase.h TestAggregateFunctions2.h TestAggregateFunctions2.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestAggregateFunctions3
                   SOURCES TestCase.cc TestCase.h TestAggregateFunctions3.h TestAggregateFunctions3.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestAggregateFunctions
                   SOURCES TestCase.cc TestCase.h TestAggregateFunctions.h TestAggregateFunctions.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestAtTableInTheOutput
                   SOURCES TestCase.cc TestCase.h TestAtTableInTheOutput.h TestAtTableInTheOutput.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestBitfields
                   SOURCES TestCase.cc TestCase.h TestBitfields.h TestBitfields.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestCatFiles
                   SOURCES TestCase.cc TestCase.h TestCatFiles.h TestCatFiles.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestCodec
                   SOURCES TestCase.cc TestCase.h TestCodec.h TestCodec.cc MockReader.h
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestCodecOptimization
                   SOURCES TestCase.cc TestCase.h TestCodecOptimization.h TestCodecOptimization.cc MockReader.h
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestCommandLineParsing
                   SOURCES TestCase.cc TestCase.h TestCommandLineParsing.h TestCommandLineParsing.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestConstCodec
                   SOURCES TestCase.cc TestCase.h TestConstCodec.h TestConstCodec.cc MockReader.h
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestConstIntegerCodec
                   SOURCES TestCase.cc TestCase.h TestConstIntegerCodec.h TestConstIntegerCodec.cc MockReader.h
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestDataJoin
                   SOURCES TestCase.cc TestCase.h TestDataJoin.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestDataLink
                   SOURCES TestCase.cc TestCase.h TestDataLink.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestDataLoader
                   SOURCES TestCase.cc TestCase.h TestDataLoader.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestDataPage
                   SOURCES TestCase.cc TestCase.h TestDataPage.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestDataRow
                   SOURCES TestCase.cc TestCase.h TestDataRow.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestDataSelect
                   SOURCES TestCase.cc TestCase.h TestDataSelect.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestDataSet
                   SOURCES TestCase.cc TestCase.h TestDataSet.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestDataTable
                   SOURCES TestCase.cc TestCase.h TestDataTable.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestDecoding
                   SOURCES TestCase.cc TestCase.h TestDecoding.h TestDecoding.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestDispatchingWriter
                   SOURCES TestCase.cc TestCase.h TestDispatchingWriter.h TestDispatchingWriter.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestDistinct
                   SOURCES TestCase.cc TestCase.h TestDistinct.h TestDistinct.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestFastODA2Request2
                   SOURCES TestCase.cc TestCase.h TestFastODA2Request2.h TestFastODA2Request2.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestFastODA2Request3
                   SOURCES TestCase.cc TestCase.h TestFastODA2Request3.h TestFastODA2Request3.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestFastODA2Request
                   SOURCES TestCase.cc TestCase.h TestFastODA2Request.h TestFastODA2Request.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestFunctionCircle
                   SOURCES TestCase.cc TestCase.h TestFunctionCircle.h TestFunctionCircle.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestFunctionDateAndTime
                   SOURCES TestCase.cc TestCase.h TestFunctionDateAndTime.h TestFunctionDateAndTime.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestFunctionDistance
                   SOURCES TestCase.cc TestCase.h TestFunctionDistance.h TestFunctionDistance.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestFunctionDotp
                   SOURCES TestCase.cc TestCase.h TestFunctionDotp.h TestFunctionDotp.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestFunctionEqBox
                   SOURCES TestCase.cc TestCase.h TestFunctionEqBox.h TestFunctionEqBox.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestFunctionNorm
                   SOURCES TestCase.cc TestCase.h TestFunctionNorm.h TestFunctionNorm.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestFunctionRggBox
                   SOURCES TestCase.cc TestCase.h TestFunctionRggBox.h TestFunctionRggBox.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestFunctionsForAngleConversion
                   SOURCES TestCase.cc TestCase.h TestFunctionsForAngleConversion.h TestFunctionsForAngleConversion.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestFunctionsForTemperatureConversion
                   SOURCES TestCase.cc TestCase.h TestFunctionsForTemperatureConversion.h TestFunctionsForTemperatureConversion.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestFunctionTdiff
                   SOURCES TestCase.cc TestCase.h TestFunctionTdiff.h TestFunctionTdiff.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestFunctionThin
                   SOURCES TestCase.cc TestCase.h TestFunctionThin.h TestFunctionThin.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestFunctionTypeConversion
                   SOURCES TestCase.cc TestCase.h TestFunctionTypeConversion.h TestFunctionTypeConversion.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestInMemoryDataHandle
                   SOURCES TestCase.cc TestCase.h TestInMemoryDataHandle.h TestInMemoryDataHandle.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestInt16_MissingCodec
                   SOURCES TestCase.cc TestCase.h TestInt16_MissingCodec.h TestInt16_MissingCodec.cc MockReader.h
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestIntegerValues
                   SOURCES TestCase.cc TestCase.h TestIntegerValues.h TestIntegerValues.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestMetaData
                   SOURCES TestCase.cc TestCase.h TestMetaData.h TestMetaData.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestMetaDataReader
                   SOURCES TestCase.cc TestCase.h TestMetaDataReader.h TestMetaDataReader.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestMinMax
                   SOURCES TestCase.cc TestCase.h TestMinMax.h TestMinMax.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestMissingValue
                   SOURCES TestCase.cc TestCase.h TestMissingValue.h TestMissingValue.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestOdaCAPI
                   SOURCES TestCase.cc TestCase.h TestOdaCAPI.h TestOdaCAPI.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestOrderBy
                   SOURCES TestCase.cc TestCase.h TestOrderBy.h TestOrderBy.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestSelectDataHandle
                   SOURCES TestCase.cc TestCase.h TestSelectDataHandle.h TestSelectDataHandle.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestSelectIterator2
                   SOURCES TestCase.cc TestCase.h TestSelectIterator2.h TestSelectIterator2.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestSelectIterator3
                   SOURCES TestCase.cc TestCase.h TestSelectIterator3.h TestSelectIterator3.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestSelectIterator
                   SOURCES TestCase.cc TestCase.h TestSelectIterator.h TestSelectIterator.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestSelectStarAt
                   SOURCES TestCase.cc TestCase.h TestSelectStarAt.h TestSelectStarAt.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestSelectTwoFiles
                   SOURCES TestCase.cc TestCase.h TestSelectTwoFiles.h TestSelectTwoFiles.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestSetvbuffer
                   SOURCES TestCase.cc TestCase.h TestSetvbuffer.h TestSetvbuffer.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestSimpleFilterIterator2
                   SOURCES TestCase.cc TestCase.h TestSimpleFilterIterator2.h TestSimpleFilterIterator2.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestSimpleFilterIterator
                   SOURCES TestCase.cc TestCase.h TestSimpleFilterIterator.h TestSimpleFilterIterator.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestSQLFunctionsInfo
                   SOURCES TestCase.cc TestCase.h TestSQLFunctionsInfo.h TestSQLFunctionsInfo.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestStar
                   SOURCES TestCase.cc TestCase.h TestStar.h TestStar.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestTEMPLATE
                   SOURCES TestCase.cc TestCase.h TestTEMPLATE.h TestTEMPLATE.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestTextSelect2
                   SOURCES TestCase.cc TestCase.h TestTextSelect2.h TestTextSelect2.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestTextSelect3
                   SOURCES TestCase.cc TestCase.h TestTextSelect3.h TestTextSelect3.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestTextSelect
                   SOURCES TestCase.cc TestCase.h TestTextSelect.h TestTextSelect.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestWriteCatFiles
                   SOURCES TestCase.cc TestCase.h TestWriteCatFiles.h TestWriteCatFiles.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET UnitTests
                   SOURCES TestCase.cc TestCase.h UnitTests.cc
                   LIBS odbtools Odb ${ECKIT_LIBRARIES} )
