/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odblib/ColumnType.h"
#include "odblib/DispatchingWriter.h"
#include "odblib/Expressions.h"
#include "odblib/SQLODAOutput.h"
#include "odblib/SQLSelect.h"
#include "odblib/Writer.h"

namespace odb {
namespace sql {

template<typename ITERATOR>
SQLODAOutput<ITERATOR>::SQLODAOutput(ITERATOR writer, const MetaData& columns)
: writer_(writer), count_(0), metaData_(0)
{
    eckit::Log::info() << " => SQLODAOutput::SQLODAOutput: columns = " << columns << std::endl;
    metaData_ = columns;
    eckit::Log::info() << " <= SQLODAOutput::SQLODAOutput" << std::endl;
}

template<typename ITERATOR>
SQLODAOutput<ITERATOR>::SQLODAOutput(ITERATOR writer)
: writer_(writer), count_(0), metaData_(0)
{}

template<typename ITERATOR>
SQLODAOutput<ITERATOR>::~SQLODAOutput() {}

template<typename ITERATOR>
void SQLODAOutput<ITERATOR>::print(std::ostream& s) const
{ 
    s << "SQLODAOutput: writer: " << writer_ << " metaData_: " <<  metaData_ << std::endl;;
}

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
void SQLODAOutput<ITERATOR>::prepare(SQLSelect& sql) {
	const expression::Expressions& columns (sql.output());
    size_t n = columns.size();

    std::ostream& L(eckit::Log::info());

    if (metaData_.size()) {
        L << "SQLODAOutput: Using meta of INTO table" << std::endl;
        ASSERT(metaData_.size() == n);
        const_cast<MetaData&>(writer_->columns()) = metaData_;
    }
    else
    {
        const_cast<MetaData&>(writer_->columns()).setSize(n);
        for(size_t i = 0; i < n; i++)
        {
            SQLExpression& c(*columns[i]);
            std::string name(c.title());
            const type::SQLType& type(*c.type());
            std::string t(type.name());
            ColumnType typ =
                t == "integer" ? INTEGER
                : t == "string" ? STRING
                : t == "real" ? REAL
                : t == "double" ? DOUBLE
                : t.find("Bitfield") == 0 ? BITFIELD
                : IGNORE;

            if (! (typ == BITFIELD))
                (**writer_).setColumn(i, name, typ);
            else
                (**writer_).setBitfieldColumn(i, name, typ, c.bitfieldDef());

            (**writer_).missingValue(i, c.missingValue());
        }
    }
    (**writer_).writeHeader();
    L << " => SQLODAOutput: " << std::endl << (**writer_).columns() << std::endl;
}


// Explicit template instantiations.

template class SQLODAOutput<Writer<>::iterator>;
template class SQLODAOutput<DispatchingWriter::iterator>;

} // namespace sql
} // namespace odb
