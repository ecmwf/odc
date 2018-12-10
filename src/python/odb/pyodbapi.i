%module pyodbapi
%warnfilter(503) operator<<;
%warnfilter(389) operator[];
%warnfilter(383) operator++;
%warnfilter(362) operator=;
%warnfilter(508) operator->;
%rename(_print) print;

#include <cstddef>
%include "std_string.i"
%include "std_vector.i"
%{
#define SWIG_FILE_WITH_INIT
#include "odb_api/pyodbapi.h"

%}


%exception {
	using namespace ::odb;
	using namespace ::odc::sql;
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
	} catch (const ::odc::sql::SyntaxError& e) {
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

%include "odb_api/ColumnType.h"
%include "eckit/sql/SQLTypedefs.h"
#include "odb_api/StringTool.h"
#include "odb_api/DataStream.h"
%include "odb_api/Codec.h"
%include "odb_api/Column.h"
#include "eckit/sql/SQLIteratorSession.h"

using namespace odc;

%template(MetaDataBase) std::vector<Column*>;
%include "odb_api/MetaData.h"

#include "odb_api/Header.h"

%include "exception.i"

%include "odb_api/IteratorProxy.h"
%template(ReaderIteratorProxy) odc::IteratorProxy<odc::ReaderIterator,odc::Reader,const double>;
%template(ReaderIteratorRow) odc::Row_<odc::ReaderIterator,odc::Reader,const double,odc::IteratorProxy<odc::ReaderIterator,odc::Reader,const double> >;

%template(SelectIteratorProxy) odc::IteratorProxy<odc::SelectIterator,odc::Select,const double>;
%template(SelectIteratorRow) odc::Row_<odc::SelectIterator,odc::Select,const double,odc::IteratorProxy<odc::SelectIterator,odc::Select,const double> >;

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
%include "odb_api/SelectIterator.h"
#include "odb_api/FixedSizeWriterIterator.h"

#include "eckit/sql/SQLInteractiveSession.h"
#include "eckit/sql/SQLIteratorSession.h"
#include "eckit/sql/SQLTable.h"
#include "eckit/sql/SQLSelect.h"
#include "eckit/sql/SQLParser.h"
#include "eckit/sql/expression/SQLExpression.h"

#include "odb_api/odbcapi.h"
%init %{
	void python_api_start();
	python_api_start();
%}
