%module pyodbapi
%include "std_string.i"
%include "std_vector.i"
%{
#define SWIG_FILE_WITH_INIT

#include "odblib/odb_api.h"

%}


%exception {
	using namespace ::odb;
	using namespace ::odb::sql;
    try {
        $action
    } catch (const ODBStopIteration& e) {
		PyErr_SetString(PyExc_StopIteration, "no more data");
		return NULL;
    } catch (const ODBIndexError& e) {
		PyErr_SetString(PyExc_IndexError, "column index out of range");
		return NULL;
	} catch (const eckit::FileError& e) {
		PyErr_SetString(PyExc_IOError, e.what());
		return NULL;
	} catch (const ::odb::sql::SyntaxError& e) {
		PyErr_SetString(PyExc_SyntaxError, e.what());
		return NULL;
	}  catch (const eckit::Exception& e) {
		PyErr_SetString(PyExc_RuntimeError, e.what());
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

#include "eclib/PathName.h"
#include "odblib/MemoryBlock.h"
#include "eclib/DataHandle.h"
#include "eclib/FileHandle.h"
#include "eclib/Timer.h"
#include "eclib/Resource.h"


%include "odblib/ODBSelect.h"
%include "odblib/SQLType.h"

%include "odblib/ColumnType.h"
%include "odblib/Types.h"
%include "odblib/SQLBitfield.h"
#include "odblib/StringTool.h"
#include "odblib/DataStream.h"
%include "odblib/HashTable.h"
%include "odblib/Codec.h"
%include "odblib/Column.h"
#include "odblib/HashTable.h"
#include "odblib/SQLIteratorSession.h"

using namespace odb;

%template(MetaDataBase) std::vector<Column*>;
%include "MetaData.h"

%include "RowsIterator.h"
#include "odblib/Header.h"

%include "exception.i"

%include "IteratorProxy.h"
%template(ReaderIteratorProxy) odb::IteratorProxy<odb::ReaderIterator,odb::Reader,const double>;
%template(ReaderIteratorRow) odb::Row_<odb::ReaderIterator,odb::Reader,const double,odb::IteratorProxy<odb::ReaderIterator,odb::Reader,const double> >;

%template(SelectIteratorProxy) odb::IteratorProxy<odb::SelectIterator,odb::Select,const double>;
%template(SelectIteratorRow) odb::Row_<odb::SelectIterator,odb::Select,const double,odb::IteratorProxy<odb::SelectIterator,odb::Select,const double> >;

#include "odblib/TemplateParameters.h"
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
#include "odblib/RowsIterator.h"
%include "SelectIterator.h"
#include "odblib/FixedSizeWriterIterator.h"

#include "odblib/SQLType.h"
#include "odblib/SQLInteractiveSession.h"
#include "odblib/SQLIteratorSession.h"
#include "odblib/SQLTable.h"
#include "odblib/SQLSelect.h"
#include "odblib/SQLParser.h"
#include "odblib/SQLExpression.h"

#include "odbcapi.h"
%init %{
	void python_api_start();
	python_api_start();
%}
