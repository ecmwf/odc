ecbuild_add_test(  TARGET TestAAAImportODB
                   SOURCES TestAAAImportODB.h TestAAAImportODB.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestAAAImportODBDispatching
                   SOURCES TestAAAImportODBDispatching.h TestAAAImportODBDispatching.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestAggregateFunctions2
                   SOURCES TestAggregateFunctions2.h TestAggregateFunctions2.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestAggregateFunctions3
                   SOURCES TestAggregateFunctions3.h TestAggregateFunctions3.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestAggregateFunctions
                   SOURCES TestAggregateFunctions.h TestAggregateFunctions.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestAtTableInTheOutput
                   SOURCES TestAtTableInTheOutput.h TestAtTableInTheOutput.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestBitfields
                   SOURCES TestBitfields.h TestBitfields.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestCatFiles
                   SOURCES TestCatFiles.h TestCatFiles.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestCodec
                   SOURCES TestCodec.h TestCodec.cc MockReader.h
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestCodecOptimization
                   SOURCES TestCodecOptimization.h TestCodecOptimization.cc MockReader.h
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestCommandLineParsing
                   SOURCES TestCommandLineParsing.h TestCommandLineParsing.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestConstCodec
                   SOURCES TestConstCodec.h TestConstCodec.cc MockReader.h
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestConstIntegerCodec
                   SOURCES TestConstIntegerCodec.h TestConstIntegerCodec.cc MockReader.h
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestDataJoin
                   SOURCES TestDataJoin.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestDataLink
                   SOURCES TestDataLink.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestDataLoader
                   SOURCES TestDataLoader.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestDataPage
                   SOURCES TestDataPage.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestDataRow
                   SOURCES TestDataRow.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestDataSelect
                   SOURCES TestDataSelect.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestDataSet
                   SOURCES TestDataSet.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestDataTable
                   SOURCES TestDataTable.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestDecoding
                   SOURCES TestDecoding.h TestDecoding.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestDispatchingWriter
                   SOURCES TestDispatchingWriter.h TestDispatchingWriter.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestDistinct
                   SOURCES TestDistinct.h TestDistinct.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestFastODA2Request2
                   SOURCES TestFastODA2Request2.h TestFastODA2Request2.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestFastODA2Request3
                   SOURCES TestFastODA2Request3.h TestFastODA2Request3.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestFastODA2Request
                   SOURCES TestFastODA2Request.h TestFastODA2Request.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestFunctionCircle
                   SOURCES TestFunctionCircle.h TestFunctionCircle.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestFunctionDateAndTime
                   SOURCES TestFunctionDateAndTime.h TestFunctionDateAndTime.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestFunctionDistance
                   SOURCES TestFunctionDistance.h TestFunctionDistance.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestFunctionDotp
                   SOURCES TestFunctionDotp.h TestFunctionDotp.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestFunctionEqBox
                   SOURCES TestFunctionEqBox.h TestFunctionEqBox.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestFunctionNorm
                   SOURCES TestFunctionNorm.h TestFunctionNorm.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestFunctionRggBox
                   SOURCES TestFunctionRggBox.h TestFunctionRggBox.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestFunctionsForAngleConversion
                   SOURCES TestFunctionsForAngleConversion.h TestFunctionsForAngleConversion.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestFunctionsForTemperatureConversion
                   SOURCES TestFunctionsForTemperatureConversion.h TestFunctionsForTemperatureConversion.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestFunctionTdiff
                   SOURCES TestFunctionTdiff.h TestFunctionTdiff.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestFunctionThin
                   SOURCES TestFunctionThin.h TestFunctionThin.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestFunctionTypeConversion
                   SOURCES TestFunctionTypeConversion.h TestFunctionTypeConversion.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestInMemoryDataHandle
                   SOURCES TestInMemoryDataHandle.h TestInMemoryDataHandle.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestInt16_MissingCodec
                   SOURCES TestInt16_MissingCodec.h TestInt16_MissingCodec.cc MockReader.h
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestIntegerValues
                   SOURCES TestIntegerValues.h TestIntegerValues.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestMetaData
                   SOURCES TestMetaData.h TestMetaData.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestMetaDataReader
                   SOURCES TestMetaDataReader.h TestMetaDataReader.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestMinMax
                   SOURCES TestMinMax.h TestMinMax.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestMissingValue
                   SOURCES TestMissingValue.h TestMissingValue.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestOdaCAPI
                   SOURCES TestOdaCAPI.h TestOdaCAPI.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestOrderBy
                   SOURCES TestOrderBy.h TestOrderBy.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestSelectDataHandle
                   SOURCES TestSelectDataHandle.h TestSelectDataHandle.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestSelectIterator2
                   SOURCES TestSelectIterator2.h TestSelectIterator2.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestSelectIterator3
                   SOURCES TestSelectIterator3.h TestSelectIterator3.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestSelectIterator
                   SOURCES TestSelectIterator.h TestSelectIterator.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestSelectStarAt
                   SOURCES TestSelectStarAt.h TestSelectStarAt.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestSelectTwoFiles
                   SOURCES TestSelectTwoFiles.h TestSelectTwoFiles.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestSetvbuffer
                   SOURCES TestSetvbuffer.h TestSetvbuffer.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestSimpleFilterIterator2
                   SOURCES TestSimpleFilterIterator2.h TestSimpleFilterIterator2.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestSimpleFilterIterator
                   SOURCES TestSimpleFilterIterator.h TestSimpleFilterIterator.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestSQLFunctionsInfo
                   SOURCES TestSQLFunctionsInfo.h TestSQLFunctionsInfo.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestStar
                   SOURCES TestStar.h TestStar.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestTEMPLATE
                   SOURCES TestTEMPLATE.h TestTEMPLATE.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestTextSelect2
                   SOURCES TestTextSelect2.h TestTextSelect2.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestTextSelect3
                   SOURCES TestTextSelect3.h TestTextSelect3.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestTextSelect
                   SOURCES TestTextSelect.h TestTextSelect.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET TestWriteCatFiles
                   SOURCES TestWriteCatFiles.h TestWriteCatFiles.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )

ecbuild_add_test(  TARGET UnitTests
                   SOURCES UnitTests.cc
                   LIBS Odb ${ECKIT_LIBRARIES} )
