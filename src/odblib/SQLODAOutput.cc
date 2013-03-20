/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

//#include <values.h>

#include "odblib/oda.h"

#include "odblib/SQLIteratorSession.h"
#include "odblib/SQLSelect.h"
#include "odblib/SQLType.h"
#include "odblib/SQLODAOutput.h"




namespace odb {
namespace sql {

template<typename ITERATOR>
SQLODAOutput<ITERATOR>::SQLODAOutput(ITERATOR writer):
	writer_(writer),
	count_(0)
{}

template<typename ITERATOR>
SQLODAOutput<ITERATOR>::~SQLODAOutput() {}

template<typename ITERATOR>
void SQLODAOutput<ITERATOR>::print(ostream& s) const { s << "SQLODAOutput"; }

template<typename ITERATOR>
void SQLODAOutput<ITERATOR>::size(int) {}

template<typename ITERATOR>
unsigned long long SQLODAOutput<ITERATOR>::count() { return count_; }

template<typename ITERATOR>
void SQLODAOutput<ITERATOR>::reset() { count_ = 0; }

template<typename ITERATOR>
void SQLODAOutput<ITERATOR>::flush() {}

template<typename ITERATOR>
void SQLODAOutput<ITERATOR>::cleanup(SQLSelect& sql) {} // { writer_->close(); }

template<typename ITERATOR>
bool SQLODAOutput<ITERATOR>::output(const expression::Expressions& results)
{
	size_t nCols = results.size();
    for(size_t i = 0; i < nCols; i++)
	{
		bool missing = false;
		(*writer_)[i] = results[i]->eval(missing);
	}

	++writer_;
	++count_;
	return true;
}

template<typename ITERATOR>
void SQLODAOutput<ITERATOR>::prepare(SQLSelect& sql)
{
    using eckit::Log;
    
	const expression::Expressions& columns (sql.output());
	size_t n = columns.size();

	writer_->columns().setSize(n);
	for(size_t i = 0; i < n; i++)
	{
		SQLExpression& c = *columns[i];
		//results[i]->title(columnNames_[i]);

		string name = c.title();
		const type::SQLType& type = *c.type();
		string t = type.name();
		ColumnType typ =
			t == "integer" ? INTEGER
			: t == "string" ? STRING
			: t == "real" ? REAL
			: t == "double" ? DOUBLE
			: t.find("Bitfield") == 0 ? BITFIELD
			: IGNORE;

		Log::debug(Here()) << "SQLODAOutput::output: " << i << " " << name
			<< " hasMissingValue: " << (c.hasMissingValue() ? "true" : "false")
			<< ", missingValue: " << c.missingValue() << endl;

		if (! (typ == BITFIELD))
			(**writer_).setColumn(i, name, typ);
		else
			(**writer_).setBitfieldColumn(i, name, typ, c.bitfieldDef());

		(**writer_).missingValue(i, c.missingValue());
	}
	Log::debug(Here()) << "SQLODAOutput::output: write header" << endl;
	(**writer_).writeHeader();
}


// Explicit template instantiations.

template class SQLODAOutput<Writer<>::iterator>;
template class SQLODAOutput<DispatchingWriter::iterator>;

} // namespace sql
} // namespace odb
