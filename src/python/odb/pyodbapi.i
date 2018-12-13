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
#include "odc/pyodbapi.h"

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

%include "odc/ODBAPIVersion.h"
%include "odc/ODBAPISettings.h"

#include "eckit/filesystem/PathName.h"
#include "odc/MemoryBlock.h"
#include "eckit/io/DataHandle.h"
#include "eckit/filesystem/FileHandle.h"
#include "eckit/utils/Timer.h"
#include "eckit/config/Resource.h"

using namespace eckit;

%include "odc/Select.h"

%include "odc/ColumnType.h"
%include "eckit/sql/SQLTypedefs.h"
#include "odc/StringTool.h"
#include "odc/DataStream.h"
%include "odc/Codec.h"
%include "odc/Column.h"
#include "eckit/sql/SQLIteratorSession.h"

using namespace odc;

%template(MetaDataBase) std::vector<Column*>;
%include "odc/MetaData.h"

#include "odc/Header.h"

%include "exception.i"

%include "odc/IteratorProxy.h"
%template(ReaderIteratorProxy) odc::IteratorProxy<odc::ReaderIterator,odc::Reader,const double>;
%template(ReaderIteratorRow) odc::Row_<odc::ReaderIterator,odc::Reader,const double,odc::IteratorProxy<odc::ReaderIterator,odc::Reader,const double> >;

%template(SelectIteratorProxy) odc::IteratorProxy<odc::SelectIterator,odc::Select,const double>;
%template(SelectIteratorRow) odc::Row_<odc::SelectIterator,odc::Select,const double,odc::IteratorProxy<odc::SelectIterator,odc::Select,const double> >;

#include "odc/TemplateParameters.h"
%include "odc/Reader.h"
%include "odc/TextReader.h"
%include "odc/Select.h"
%include "odc/Writer.h"
%include "odc/WriterBufferingIterator.h"
%include "odc/WriterDispatchingIterator.h"
%include "odc/DispatchingWriter.h"
%include "odc/DispatchingWriter.h"
%include "odc/ReaderIterator.h"
%include "odc/TextReaderIterator.h"
%include "odc/SelectIterator.h"
#include "odc/FixedSizeWriterIterator.h"

#include "eckit/sql/SQLInteractiveSession.h"
#include "eckit/sql/SQLIteratorSession.h"
#include "eckit/sql/SQLTable.h"
#include "eckit/sql/SQLSelect.h"
#include "eckit/sql/SQLParser.h"
#include "eckit/sql/expression/SQLExpression.h"

#include "odc/odbcapi.h"
%init %{
	void python_api_start();
	python_api_start();
%}
