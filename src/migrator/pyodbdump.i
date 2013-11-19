%module pyodbdump
%{

#define SWIG_FILE_WITH_INIT

#include <iostream>
#include <sstream>
#include <errno.h>
#include <math.h>
#include <values.h>
#include <stdint.h>
#include <vector>

using namespace std;

#include "odblib/MetaData.h"
#include "odblib/RowsIterator.h"
#include "odblib/Header.h"
#include "odblib/IteratorProxy.h"
#include "odblib/ODBAPIVersion.h"
#include "odblib/ODBAPISettings.h"
#include "odblib/SQLSession.h"
#include "odblib/SQLInteractiveSession.h"
#include "odblib/SQLParser.h"
#include "odblib/Column.h"

#include "migrator_api.h"

#include "OldODBReader.h"
#include "TSQLReader.h"
#include "ODBIterator.h"

using namespace odb;
using namespace odb::tool;

%}

%include "typemaps.i"
%include "std_string.i"
%include "exception.i"
%include "std_except.i"
%include "std_vector.i"


%exception {
	using namespace ::eclib;
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
	} catch (const FileError& e) {
		PyErr_SetString(PyExc_IOError, e.what());
		return NULL;
	} catch (const ::odb::sql::SyntaxError& e) {
		PyErr_SetString(PyExc_SyntaxError, e.what());
		return NULL;
	}  catch (const Exception& e) {
		PyErr_SetString(PyExc_RuntimeError, e.what());
		return NULL;
	}
	
}

%include "odblib/ODBAPIVersion.h"
%include "odblib/ODBAPISettings.h"
%include "odblib/Column.h"
%include "odblib/IteratorProxy.h"
%include "TSQLReader.h"
%template(MetaDataBase) std::vector<odb::Column*>;
%template(OldODBReader) odb::tool::TSQLReader<odb::tool::ODBIterator>;
%template(ODBReader_iterator) odb::IteratorProxy<odb::tool::ODBIterator,odb::tool::TSQLReader<odb::tool::ODBIterator>, const double>;
%include "odblib/MetaData.h"
%include "odblib/RowsIterator.h"
#include "odblib/Header.h"

%include "migrator_api.h"
%include "ODBIterator.h"

%template(ODBReader) odb::tool::TSQLReader<ODBIterator>;

#include "odbcapi.h"


%init %{
	void python_api_start();
	python_api_start();
%}
