list( APPEND odblib_src_files
Array.h
BitColumnExpression.cc
BitColumnExpression.h
Codec.cc
Codec.h
CodecOptimizer.cc
CodecOptimizer.h
Column.cc
Column.h
ColumnExpression.cc
ColumnExpression.h
Comparator.cc
Comparator.h
ConstantExpression.cc
ConstantExpression.h
ConstantSetter.cc
ConstantSetter.h
DataStream.cc
DataStream.h
DateTime.cc
DateTime.h
Decoder.cc
Decoder.h
Dictionary.cc
Dictionary.h
DispatchingWriter.cc
DispatchingWriter.h
Duration.cc
Duration.h
Endian.h
Environment.cc
Environment.h
EqRegionCache.cc
EqRegionCache.h
Expressions.cc
Expressions.h
FastODA2Request.cc
FastODA2Request.h
FixedSizeWriterIterator.cc
FixedSizeWriterIterator.h
FunctionAND.cc
FunctionAND.h
FunctionAVG.cc
FunctionAVG.h
FunctionCOUNT.cc
FunctionCOUNT.h
FunctionDOTP.cc
FunctionDOTP.h
FunctionEQ.cc
FunctionEQ.h
FunctionEQ_BOXLAT.cc
FunctionEQ_BOXLAT.h
FunctionEQ_BOXLON.cc
FunctionEQ_BOXLON.h
FunctionExpression.cc
FunctionExpression.h
FunctionIN.cc
FunctionIN.h
FunctionIntegerExpression.cc
FunctionIntegerExpression.h
FunctionJOIN.cc
FunctionJOIN.h
FunctionJULIAN.cc
FunctionJULIAN.h
FunctionMAX.cc
FunctionMAX.h
FunctionMIN.cc
FunctionMIN.h
FunctionNORM.cc
FunctionNORM.h
FunctionNOT_IN.cc
FunctionNOT_IN.h
FunctionNOT_NULL.cc
FunctionNOT_NULL.h
FunctionNULL.cc
FunctionNULL.h
FunctionNVL.cc
FunctionNVL.h
FunctionOR.cc
FunctionOR.h
FunctionRGG_BOXLAT.cc
FunctionRGG_BOXLAT.h
FunctionRGG_BOXLON.cc
FunctionRGG_BOXLON.h
FunctionRMS.cc
FunctionRMS.h
FunctionROWNUMBER.cc
FunctionROWNUMBER.h
FunctionSTDEV.cc
FunctionSTDEV.h
FunctionSUM.cc
FunctionSUM.h
FunctionTDIFF.cc
FunctionTDIFF.h
FunctionTHIN.cc
FunctionTHIN.h
FunctionTIMESTAMP.cc
FunctionTIMESTAMP.h
FunctionVAR.cc
FunctionVAR.h
GribCodes.cc
GribCodes.h
HashTable.cc
HashTable.h
Header.cc
Header.h
InMemoryDataHandle.cc
InMemoryDataHandle.h
IteratorProxy.h
LimitValues.h
MD5.cc
MD5.h
MemoryBlock.cc
MemoryBlock.h
MetaData.cc
MetaData.h
MetaDataReader.cc
MetaDataReader.h
MetaDataReaderIterator.cc
MetaDataReaderIterator.h
NumberExpression.cc
NumberExpression.h
ODAColumn.cc
ODAColumn.h
ODADatabase.cc
ODADatabase.h
ODAHandle.cc
ODAHandle.h
ODATranslator.h
ODAUpdatingIterator.cc
ODAUpdatingIterator.h
ODBAPISettings.cc
ODBAPISettings.h
ODBAPIVersion.cc
ODBAPIVersion.h
ODBSelect.cc
ODBSelect.h
OrderByExpressions.cc
OrderByExpressions.h
ParameterExpression.cc
ParameterExpression.h
Reader.cc
Reader.h
ReaderIterator.cc
ReaderIterator.h
RegionCache.cc
RegionCache.h
RggRegionCache.cc
RggRegionCache.h
RowsIterator.h
SQLAST.h
SQLBit.cc
SQLBit.h
SQLBitColumn.cc
SQLBitColumn.h
SQLBitfield.cc
SQLBitfield.h
SQLColumn.cc
SQLColumn.h
SQLCountStar.cc
SQLCountStar.h
SQLCreateTable.cc
SQLCreateTable.h
SQLDatabase.cc
SQLDatabase.h
SQLDistinctOutput.cc
SQLDistinctOutput.h
SQLDouble.cc
SQLDouble.h
SQLExpression.cc
SQLExpression.h
SQLExpressionEvaluated.cc
SQLExpressionEvaluated.h
SQLIndex.cc
SQLIndex.h
SQLInt.cc
SQLInt.h
SQLInteractiveSession.cc
SQLInteractiveSession.h
SQLIterator.h
SQLIteratorOutput.cc
SQLIteratorOutput.h
SQLIteratorSession.cc
SQLIteratorSession.h
SQLODAOutput.cc
SQLODAOutput.h
SQLOrderOutput.cc
SQLOrderOutput.h
SQLOutput.cc
SQLOutput.h
SQLOutputConfig.cc
SQLOutputConfig.h
SQLParser.cc
SQLParser.h
SQLReal.cc
SQLReal.h
SQLSelect.cc
SQLSelect.h
SQLSelectFactory.cc
SQLSelectFactory.h
SQLServer.cc
SQLServer.h
SQLServerOutput.cc
SQLServerOutput.h
SQLServerSession.cc
SQLServerSession.h
SQLService.cc
SQLService.h
SQLSession.cc
SQLSession.h
SQLSimpleOutput.cc
SQLSimpleOutput.h
SQLStatement.cc
SQLStatement.h
SQLString.cc
SQLString.h
SQLTable.cc
SQLTable.h
SQLType.cc
SQLType.h
SQLUser.cc
SQLUser.h
SchemaAnalyzer.cc
SchemaAnalyzer.h
SelectIterator.cc
SelectIterator.h
SelectOneTable.cc
SelectOneTable.h
SimpleFilter.cc
SimpleFilter.h
SimpleFilterIterator.cc
SimpleFilterIterator.h
Stack.cc
Stack.h
StringExpression.cc
StringExpression.h
StringTool.cc
StringTool.h
TODATable.cc
TODATable.h
TODATableIterator.cc
TODATableIterator.h
TReadOnlyMemoryDataHandle.cc
TReadOnlyMemoryDataHandle.h
TemplateParameters.cc
TemplateParameters.h
TextReader.cc
TextReader.h
TextReaderIterator.cc
TextReaderIterator.h
Tracer.cc
Tracer.h
UnsafeInMemoryDataHandle.h
VariableExpression.cc
VariableExpression.h
Writer.cc
Writer.h
WriterBufferingIterator.cc
WriterBufferingIterator.h
WriterDispatchingIterator.cc
WriterDispatchingIterator.h
md5.h
md5.c
oda.h
odbcapi.cc
odbcapi.h
piconst.h
)

# templates

list( APPEND odblib_templates
CodecOptimizer.cc
ConstantSetter.cc
DataStream.cc
FastODA2Request.cc
Header.cc
ODAUpdatingIterator.cc
SQLIteratorOutput.cc
SQLIteratorSession.cc
SimpleFilter.cc
SimpleFilterIterator.cc
TODATable.cc
TODATableIterator.cc
WriterDispatchingIterator.cc
)

# define Odb library

ecmwf_add_library( TARGET     Odb
                  COMPONENT  server
                  SOURCES    ${odblib_src_files}
                  TEMPLATES  ${odblib_templates}
                  LIBS       Ec )

ecmwf_generate_yy( YYPREFIX odblib_
                  YACC sqly
                  LEX  sqll
                  DEPENDANT SQLParser.cc )

###############################################################################
# swig python interface

if( SWIG_FOUND AND PYTHONLIBS_FOUND )

    set(CMAKE_SWIG_FLAGS "")

    include_directories( ${PYTHON_INCLUDE_PATH} )

    set_source_files_properties( pyodbapi.swig PROPERTIES CPLUSPLUS ON )
    set_source_files_properties( pyodbapi.swig PROPERTIES SWIG_FLAGS "-includeall")

    swig_add_module( pyodbapi python pyodbapi.swig )

    swig_link_libraries( pyodbapi Odb ${PYTHON_LIBRARIES} )

    set_target_properties( _pyodbapi PROPERTIES LINKER_LANGUAGE CXX )

#    set(PYTHON_DEST "lib64/python${PYTHON_VERSION}/site-packages" )
#    install(TARGETS _partio DESTINATION ${PYTHON_DEST})
#    install(FILES ${CMAKE_BINARY_DIR}/${outdir}/src/py/partio.py  DESTINATION ${PYTHON_DEST})

endif()
