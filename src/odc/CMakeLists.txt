configure_file( ODBAPIVersionSHA1.cc.in ODBAPIVersionSHA1.cc @ONLY )

if ( ODB_SERVER_TIME_FORMAT_FOUR_DIGITS )
    add_definitions ( -DODB_SERVER_TIME_FORMAT_FOUR_DIGITS )
endif()

if ( odc_SCHEMA_PATH )
    add_definitions ( -Dodc_SCHEMA_PATH=${odc_SCHEMA_PATH} )
endif()

configure_file( odc_config.f90.in odc_config.f90 )


list( APPEND odclib_src_files

api/odc.h
api/odc_c.h
api/odc.cc
api/Odb.h
api/Odb.cc
api/ColumnType.h
api/ColumnInfo.h
api/StridedData.h

pyodbapi.h
ODBAPIVersionSHA1.cc
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
Partition.h
Partition.cc
Partitions.h
Partitions.cc
Indexer.h
Indexer.cc
Partitioner.h
Partitioner.cc
FileCollector.h
FileCollector.cc
FileMapper.h
FileMapper.cc
RequestUtils.h
RequestUtils.cc
Stager.h
Stager.cc
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
sql/function/FunctionEQ_BOXLAT.cc
sql/function/FunctionEQ_BOXLAT.h
sql/function/FunctionEQ_BOXLON.cc
sql/function/FunctionEQ_BOXLON.h
sql/function/FunctionRGG_BOXLAT.cc
sql/function/FunctionRGG_BOXLAT.h
sql/function/FunctionRGG_BOXLON.cc
sql/function/FunctionRGG_BOXLON.h

utility/Tracer.cc
utility/Tracer.h

# Data types' support
data/DataHandleFactory.h
data/DataHandleFactory.cc
data/FileHandleFactory.h
data/FileHandleFactory.cc
data/PartFileHandleFactory.h
data/PartFileHandleFactory.cc
data/HttpHandle.h
data/HttpHandle.cc
data/HttpHandleFactory.h
data/HttpHandleFactory.cc

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

ecbuild_add_library( TARGET             odccore
                     INSTALL_HEADERS    LISTED
                     HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/odc
                     SOURCES            ${odclib_src_files}
                     GENERATED          ODBAPIVersionSHA1.cc
                     TEMPLATES          ${odclib_templates}
                     LINKER_LANGUAGE    CXX
                     LIBS               eckit_sql eckit )

ecbuild_add_library(
    TARGET         fodc
    CONDITION      HAVE_FORTRAN
    SOURCES        api/odc.f90
                   odc_config.f90
    GENERATED      odc_config.f90
    LIBS           odccore
)


