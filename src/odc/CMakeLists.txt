configure_file( ODBAPIVersionSHA1.cc.in ODBAPIVersionSHA1.cc @ONLY )

configure_file( odc_config.f90.in odc_config.f90 )


list( APPEND odclib_src_files

${CMAKE_CURRENT_BINARY_DIR}/ODBAPIVersionSHA1.cc

api/odc.h
api/odc.cc
api/Odb.h
api/Odb.cc
api/ColumnType.h
api/ColumnInfo.h
api/StridedData.h

pyodbapi.h
StringTool.cc
StringTool.h
CommandLineParser.cc
CommandLineParser.h
Comparator.cc
Comparator.h
ConstantSetter.cc
ConstantSetter.h
DispatchingWriter.cc
DispatchingWriter.h
MDI.cc
MDI.h
IteratorProxy.h
LibOdc.cc
LibOdc.h
MDSetter.cc
MDSetter.h
MDUpdatingIterator.cc
MDUpdatingIterator.h
ODAHandle.cc
ODAHandle.h
ODATranslator.h
ODAUpdatingIterator.cc
ODAUpdatingIterator.h
ODBAPISettings.cc
ODBAPISettings.h
ODBAPIVersion.cc
ODBAPIVersion.h
ODBApplication.cc
ODBApplication.h
Select.cc
Select.h
Reader.cc
Reader.h
ReaderIterator.cc
ReaderIterator.h
SelectIterator.cc
SelectIterator.h
TemplateParameters.cc
TemplateParameters.h
Writer.cc
Writer.h
WriterBufferingIterator.cc
WriterBufferingIterator.h
WriterDispatchingIterator.cc
WriterDispatchingIterator.h
odccapi.cc
odccapi.h
RowsCounter.h
RowsCounter.cc
Block.h
Block.cc
Indexer.h
Indexer.cc
RequestUtils.h
RequestUtils.cc
ODBTarget.cc
ODBTarget.h

core/Column.cc
core/Column.h
core/DataStream.h
core/DecodeTarget.cc
core/DecodeTarget.h
core/Encoder.cc
core/Encoder.h
core/Exceptions.cc
core/Exceptions.h
core/Header.cc
core/Header.h
core/MetaData.cc
core/MetaData.h
core/Span.cc
core/Span.h
core/Table.cc
core/Table.h
core/TablesReader.cc
core/TablesReader.h
core/ThreadSharedDataHandle.cc
core/ThreadSharedDataHandle.h
core/Codec.cc
core/Codec.h
core/CodecFactory.h
core/CodecFactory.cc

codec/Constant.cc
codec/Constant.h
codec/Integer.cc
codec/Integer.h
codec/IntegerMissing.cc
codec/IntegerMissing.h
codec/String.cc
codec/String.h
codec/Real.cc
codec/Real.h
codec/CodecOptimizer.cc
codec/CodecOptimizer.h

csv/TextReader.cc
csv/TextReader.h
csv/TextReaderIterator.cc
csv/TextReaderIterator.h

sql/SQLOutputConfig.cc
sql/SQLOutputConfig.h
sql/SQLSelectOutput.cc
sql/SQLSelectOutput.h
sql/ODAOutput.cc
sql/ODAOutput.h
sql/TODATable.cc
sql/TODATable.h
sql/TODATableIterator.cc
sql/TODATableIterator.h
sql/Types.cc
sql/Types.h

utility/Tracer.cc
utility/Tracer.h

#iterator-api/BaseReader.cc
#iterator-api/BaseReader.h
)

# templates

list( APPEND odclib_templates
ConstantSetter.cc
ODAUpdatingIterator.cc
WriterDispatchingIterator.cc
MDUpdatingIterator.cc
)

# define odc library

ecbuild_add_library( TARGET odccore
                     INSTALL_HEADERS
                        LISTED
                     HEADER_DESTINATION
                        ${INSTALL_INCLUDE_DIR}/odc
                     SOURCES
                        ${odclib_src_files}
                     TEMPLATES
                        ${odclib_templates}
                     PUBLIC_INCLUDES
                        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/src>
                        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
                     PUBLIC_LIBS
                        eckit_sql eckit )

ecbuild_add_library(
    TARGET          fodc
    CONDITION       HAVE_FORTRAN
    SOURCES         api/odc.f90
                    ${CMAKE_CURRENT_BINARY_DIR}/odc_config.f90
    PUBLIC_INCLUDES $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/module>
                    $<INSTALL_INTERFACE:module/odc>
    PUBLIC_LIBS     odccore
)

if ( HAVE_FORTRAN AND ECBUILD_INSTALL_FORTRAN_MODULES )
    install( FILES ${CMAKE_Fortran_MODULE_DIRECTORY}/${CMAKE_CFG_INTDIR}/odc.mod
             DESTINATION module/odc
             COMPONENT modules )
endif()
