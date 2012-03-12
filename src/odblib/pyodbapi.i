%module pyodbapi
%include "std_string.i"
%include "std_vector.i"
%{
#define SWIG_FILE_WITH_INIT

#include "oda.h"

%}


%exception {
	using namespace ::odb;
    try {
        $action
    } catch (const ODBStopIteration& e) {
		PyErr_SetString(PyExc_StopIteration, "no more data");
		return NULL;
    } catch (const ODBIndexError& e) {
		PyErr_SetString(PyExc_IndexError, "column index out of range");
		return NULL;
	}
}

#include <iostream>
#include <sstream>
#include <errno.h>
#include <math.h>
#include <values.h>
#include <stdint.h>

using namespace std;

%include "ODBAPIVersion.h"
%include "ODBAPISettings.h"

#include "PathName.h"
#include "MemoryBlock.h"
#include "DataHandle.h"
#include "FileHandle.h"
#include "Timer.h"
#include "Resource.h"


%include "ODBSelect.h"
%include "SQLType.h"

%include "SQLBitfield.h"
#include "StringTool.h"
#include "DataStream.h"
#include "HashTable.h"
#include "Codec.h"
%include "Column.h"
#include "HashTable.h"
#include "SQLIteratorSession.h"

using namespace odb;

%template(MetaDataBase) std::vector<Column*>;
%include "MetaData.h"

%include "RowsIterator.h"
#include "Header.h"

%include "exception.i"

%include "IteratorProxy.h"
%template(ReaderIteratorProxy) odb::IteratorProxy<odb::ReaderIterator,odb::Reader,const double>;
%template(ReaderIteratorRow) odb::Row_<odb::ReaderIterator,odb::Reader,const double,odb::IteratorProxy<odb::ReaderIterator,odb::Reader,const double> >;

%template(SelectIteratorProxy) odb::IteratorProxy<odb::SelectIterator,odb::Select,const double>;
%template(SelectIteratorRow) odb::Row_<odb::SelectIterator,odb::Select,const double,odb::IteratorProxy<odb::SelectIterator,odb::Select,const double> >;

#include "TemplateParameters.h"
%include "Reader.h"
%include "TextReader.h"
%include "ODBSelect.h"
%include "Writer.h"
%include "WriterBufferingIterator.h"
%include "WriterDispatchingIterator.h"
%include "DispatchingWriter.h"
%include "DispatchingWriter.h"
%include "ReaderIterator.h"
%include "TextReaderIterator.h"
#include "RowsIterator.h"
%include "SelectIterator.h"
#include "FixedSizeWriterIterator.h"

#include "SQLType.h"
#include "SQLInteractiveSession.h"
#include "SQLIteratorSession.h"
#include "SQLTable.h"
#include "SQLSelect.h"
#include "SQLParser.h"
#include "SQLExpression.h"

#include "odbcapi.h"
%init %{
	void python_api_start();
	python_api_start();
%}
