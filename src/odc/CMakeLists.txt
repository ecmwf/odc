configure_file( ODBAPIVersionSHA1.cc.in ODBAPIVersionSHA1.cc @ONLY )

if ( ODB_SERVER_TIME_FORMAT_FOUR_DIGITS )
    add_definitions ( -DODB_SERVER_TIME_FORMAT_FOUR_DIGITS )
endif()

if ( odc_SCHEMA_PATH )
    add_definitions ( -Dodc_SCHEMA_PATH=${odc_SCHEMA_PATH} )
endif()


list( APPEND odblib_src_files
####odbql.cc
####odbql.h
pyodbapi.h
ODBAPIVersionSHA1.cc
StringTool.cc
StringTool.h
Array.h
###>>>#ShiftedBitColumnExpression.cc
###>>>#ShiftedBitColumnExpression.h
Codec.cc
Codec.h
CodecFactory.h
CodecFactory.cc
CodecOptimizer.cc
CodecOptimizer.h
Column.cc
Column.h
ColumnType.h
CommandLineParser.cc
CommandLineParser.h
Comparator.cc
Comparator.h
ConstantSetter.cc
ConstantSetter.h
####DataColumn.cc
####DataColumn.h
####DataColumns.cc
####DataColumns.h
####DataField.cc
####DataField.h
####DataJoin.cc
####DataJoin.h
####DataJoinIterator.cc
####DataJoinIterator.h
####DataLink.cc
####DataLink.h
####DataLinkFiller.cc
####DataLinkFiller.h
####DataLinkIterator.cc
####DataLinkIterator.h
####DataLinks.cc
####DataLinks.h
####DataLoader.cc
####DataLoader.h
####DataPage.cc
####DataPage.h
####DataRecord.cc
####DataRecord.h
####DataRecordIterator.h
####DataRow.cc
####DataRow.h
####DataSaver.cc
####DataSaver.h
####DataSelect.cc
####DataSelect.h
####DataSelectIterator.cc
####DataSelectIterator.h
####DataSelectOutput.cc
####DataSelectOutput.h
####DataSelectSession.cc
####DataSelectSession.h
####DataSet.cc
####DataSet.h
####DataSetBuilder.cc
####DataSetBuilder.h
####DataSetFiller.cc
####DataSetFiller.h
####DataStream.cc
####DataStream.h
####DataTable.cc
####DataTable.h
####DataTableFiller.cc
####DataTableFiller.h
####DataTableIterator.cc
####DataTableIterator.h
####DataTableMappings.cc
####DataTableMappings.h
####DataTables.cc
####DataTables.h
####DataView.cc
####DataView.h
DateTime.cc
DateTime.h
####Dictionary.cc
####Dictionary.h
DispatchingWriter.cc
DispatchingWriter.h
####DirectAccess.cc
####DirectAccess.h
####DirectAccessIterator.cc
####DirectAccessIterator.h
Duration.cc
Duration.h
Endian.h
FastODA2Request.cc
FastODA2Request.h
FixedSizeWriterIterator.cc
FixedSizeWriterIterator.h
MDI.cc
MDI.h
GribCodes.cc
GribCodes.h
Header.cc
Header.h
InMemoryDataHandle.cc
InMemoryDataHandle.h
IteratorProxy.h
LibOdc.cc
LibOdc.h
MD5.cc
MD5.h
MDSetter.cc
MDSetter.h
MDUpdatingIterator.cc
MDUpdatingIterator.h
MemoryBlock.cc
MemoryBlock.h
MetaData.cc
MetaData.h
MetaDataReader.cc
MetaDataReader.h
MetaDataReaderIterator.cc
MetaDataReaderIterator.h
####ODAColumn.cc
####ODAColumn.h
####NullColumn.cc
####NullColumn.h
####ODADatabase.cc
####ODADatabase.h
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
###>>>#SchemaAnalyzer.cc
###>>>#SchemaAnalyzer.h
SelectIterator.cc
SelectIterator.h
TODATable.cc
TODATable.h
TODATableIterator.cc
TODATableIterator.h
TReadOnlyMemoryDataHandle.cc
TReadOnlyMemoryDataHandle.h
TemplateParameters.cc
TemplateParameters.h
Tracer.cc
Tracer.h
UnsafeInMemoryDataHandle.h
Writer.cc
Writer.h
WriterBufferingIterator.cc
WriterBufferingIterator.h
WriterDispatchingIterator.cc
WriterDispatchingIterator.h
md5_hash.h
md5_hash.c
odc.h
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
Odb2Hub.h
Odb2Hub.cc
RequestUtils.h
RequestUtils.cc
Archiver.h
Archiver.cc 
Stager.h
Stager.cc
ODBTarget.cc
ODBTarget.h

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
sql/Types.cc
sql/Types.h

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
# requires  metkit
data/MarsHandleFactory.cc
data/MarsHandleFactory.h
)

# templates

list( APPEND odblib_templates
ConstantSetter.cc
DataStream.cc
FastODA2Request.cc
Header.cc
ODAUpdatingIterator.cc
WriterDispatchingIterator.cc
MDUpdatingIterator.cc
MetaDataReader.cc
)

# define Odb library

ecbuild_add_library( TARGET             Odb
                     INSTALL_HEADERS    LISTED
                     HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/odc
                     PRIVATE_INCLUDES   ${METKIT_INCLUDE_DIRS}         # Only for MarsRequestHandle
                     COMPONENT          server
                     SOURCES            ${odblib_src_files}
                     GENERATED          ODBAPIVersionSHA1.cc
                     TEMPLATES          ${odblib_templates}
                     LINKER_LANGUAGE    CXX
                     LIBS               metkit )


