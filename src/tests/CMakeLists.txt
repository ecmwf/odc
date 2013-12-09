set(TESTS 
TestAAAImportODB
TestAAAImportODBDispatching
TestAggregateFunctions
TestAggregateFunctions2
TestAggregateFunctions3
TestAtTableInTheOutput
TestBitfields
TestCatFiles
TestCodec
TestCodecOptimization
TestCommandLineParsing
TestConstCodec
TestConstIntegerCodec
TestDataJoin
TestDataLink
TestDataLoader
TestDataPage
TestDataRow
TestDataSelect
TestDataSet
TestDataTable
TestDecoding
TestDispatchingWriter
TestDistinct
TestFastODA2Request
TestFastODA2Request2
TestFastODA2Request3
TestFunctionCircle
TestFunctionDateAndTime
TestFunctionDistance
TestFunctionDotp
TestFunctionEqBox
TestFunctionNorm
TestFunctionRggBox
TestFunctionTdiff
TestFunctionThin
TestFunctionTypeConversion
TestFunctionsForAngleConversion
TestFunctionsForTemperatureConversion
TestInMemoryDataHandle
TestInt16_MissingCodec
TestIntegerValues
TestMetaData
TestMetaDataReader
TestMinMax
TestMissingValue
TestOdaCAPI
TestOrderBy
TestSQLFunctionsInfo
TestSelectDataHandle
TestSelectIterator
TestSelectIterator2
TestSelectIterator3
TestSelectStarAt
TestSelectTwoFiles
TestSetvbuffer
TestSimpleFilterIterator
TestSimpleFilterIterator2
TestStar
TestTEMPLATE
TestTextSelect
TestTextSelect2
TestTextSelect3
TestWriteCatFiles
UnitTests
)

set(EXTRA_INCLUDES "")
set(EXTRA_LIBS "")

if( ENABLE_MIGRATOR )
	list(APPEND TESTS TestImportODB)
	set(EXTRA_INCLUDES ${ODB_INCLUDE_DIRS})
	set(EXTRA_LIBS Odbmigrator)
endif()

foreach(t ${TESTS})

ecbuild_add_test(  TARGET ${t}
                   SOURCES ${t}.cc MockReader.h
				   INCLUDES   ${EXTRA_INCLUDES}
				   LIBS ${EXTRA_LIBS} odbtools Odb ${ECKIT_LIBRARIES} )
endforeach()
