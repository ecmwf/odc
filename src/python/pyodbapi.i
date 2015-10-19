%module pyodbapi
#include <cstddef>
%include "std_string.i"
%include "std_vector.i"
%{
#define SWIG_FILE_WITH_INIT
#include "odb_api/pyodbapi.h"

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
#include <stdint.h>

using namespace std;

%include "odb_api/ODBAPIVersion.h"
%include "odb_api/ODBAPISettings.h"

#include "eckit/filesystem/PathName.h"
#include "odb_api/MemoryBlock.h"
#include "eckit/io/DataHandle.h"
#include "eckit/filesystem/FileHandle.h"
#include "eckit/utils/Timer.h"
#include "eckit/config/Resource.h"

using namespace eckit;

%include "odb_api/Select.h"
%include "odb_api/SQLType.h"

%include "odb_api/ColumnType.h"
%include "odb_api/Types.h"
%include "odb_api/SQLBitfield.h"
#include "odb_api/StringTool.h"
#include "odb_api/DataStream.h"
%include "odb_api/HashTable.h"
%include "odb_api/Codec.h"
%include "odb_api/Column.h"
#include "odb_api/HashTable.h"
#include "odb_api/SQLIteratorSession.h"

using namespace odb;

%template(MetaDataBase) std::vector<Column*>;
%include "odb_api/MetaData.h"

%include "odb_api/RowsIterator.h"
#include "odb_api/Header.h"

%include "exception.i"

%include "odb_api/IteratorProxy.h"
%template(ReaderIteratorProxy) odb::IteratorProxy<odb::ReaderIterator,odb::Reader,const double>;
%template(ReaderIteratorRow) odb::Row_<odb::ReaderIterator,odb::Reader,const double,odb::IteratorProxy<odb::ReaderIterator,odb::Reader,const double> >;

%template(SelectIteratorProxy) odb::IteratorProxy<odb::SelectIterator,odb::Select,const double>;
%template(SelectIteratorRow) odb::Row_<odb::SelectIterator,odb::Select,const double,odb::IteratorProxy<odb::SelectIterator,odb::Select,const double> >;

#include "odb_api/TemplateParameters.h"
%include "odb_api/Reader.h"
%include "odb_api/TextReader.h"
%include "odb_api/Select.h"
%include "odb_api/Writer.h"
%include "odb_api/WriterBufferingIterator.h"
%include "odb_api/WriterDispatchingIterator.h"
%include "odb_api/DispatchingWriter.h"
%include "odb_api/DispatchingWriter.h"
%include "odb_api/ReaderIterator.h"
%include "odb_api/TextReaderIterator.h"
#include "odb_api/RowsIterator.h"
%include "odb_api/SelectIterator.h"
#include "odb_api/FixedSizeWriterIterator.h"

#include "odb_api/SQLType.h"
#include "odb_api/SQLInteractiveSession.h"
#include "odb_api/SQLIteratorSession.h"
#include "odb_api/SQLTable.h"
#include "odb_api/SQLSelect.h"
#include "odb_api/SQLParser.h"
#include "odb_api/SQLExpression.h"

#include "odb_api/odbcapi.h"
%init %{
	void python_api_start();
	python_api_start();
%}
