/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odb_api/ColumnType.h"
#include "odb_api/DispatchingWriter.h"
#include "eckit/sql/expression/SQLExpressions.h"
#include "odb_api/SQLODAOutput.h"
#include "eckit/sql/SQLSelect.h"
#include "odb_api/Writer.h"

namespace odb {
namespace sql {

template<typename WRITER>
SQLODAOutput<WRITER>::SQLODAOutput(WRITER* writer, const MetaData& columns)
: writer_(writer), it_(writer->begin()), count_(0), metaData_(0)
{
    metaData_ = columns;
}

template<typename WRITER>
SQLODAOutput<WRITER>::SQLODAOutput(WRITER* writer)
: writer_(writer), it_(writer->begin()), count_(0), metaData_(0)
{}

template<typename WRITER>
SQLODAOutput<WRITER>::~SQLODAOutput()
{
    delete writer_;
}

template<typename WRITER>
void SQLODAOutput<WRITER>::print(std::ostream& s) const
{ 
    s << "SQLODAOutput: iterator: " << it_ << " metaData_: " <<  metaData_ << std::endl;;
}

template<typename WRITER>
void SQLODAOutput<WRITER>::size(int) {}

template<typename WRITER>
unsigned long long SQLODAOutput<WRITER>::count() { return count_; }

template<typename WRITER>
void SQLODAOutput<WRITER>::reset() { count_ = 0; }

template<typename WRITER>
void SQLODAOutput<WRITER>::flush() {}

template<typename WRITER>
void SQLODAOutput<WRITER>::cleanup(SQLSelect& sql) { sql.outputFiles((**it_).outputFiles()); }

template<typename WRITER>
bool SQLODAOutput<WRITER>::output(const expression::Expressions& results)
{
    size_t nCols (results.size());
    for(size_t i (0); i < nCols; ++i)
    {
        bool missing = false;
        // TODO: pass the context to it_
        (*it_)[i] = results[i]->eval(missing);
    }

    ++it_;
    ++count_;
    return true;
}

template<typename WRITER>
void SQLODAOutput<WRITER>::prepare(SQLSelect& sql) 
{
    const expression::Expressions& columns (sql.output());
    size_t n (columns.size());

    std::ostream& L(eckit::Log::debug());

    if (metaData_.size()) {
        L << "SQLODAOutput: Using meta of INTO table" << std::endl;
        ASSERT(metaData_.size() == n);
        const_cast<MetaData&>(it_->columns()) = metaData_;
    }
    else
    {
        const_cast<MetaData&>(it_->columns()).setSize(n);
        for(size_t i (0); i < n; i++)
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
                (**it_).setColumn(i, name, typ);
            else
                (**it_).setBitfieldColumn(i, name, typ, c.bitfieldDef());

            (**it_).missingValue(i, c.missingValue());
        }
    }
    (**it_).writeHeader();
    L << " => SQLODAOutput: " << std::endl << (**it_).columns() << std::endl;
}


// Explicit template instantiations.

template class SQLODAOutput<Writer<> >;
template class SQLODAOutput<DispatchingWriter>;

} // namespace sql
} // namespace odb
