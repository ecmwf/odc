/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/sql/expression/SQLExpressions.h"
#include "eckit/sql/SQLSelect.h"

#include "odc/api/ColumnType.h"
#include "odc/DispatchingWriter.h"
#include "odc/LibOdc.h"
#include "odc/sql/ODAOutput.h"
#include "odc/sql/Types.h"
#include "odc/Writer.h"

using namespace eckit;
using namespace eckit::sql;
using namespace odc::api;

namespace odc {
namespace sql {

//----------------------------------------------------------------------------------------------------------------------

/*template<typename WRITER>
ODAOutput<WRITER>::ODAOutput(WRITER* writer, const MetaData& columns)
: writer_(writer), it_(writer->begin()), count_(0), metaData_(0)
{
    metaData_ = columns;
}*/

template<typename WRITER>
ODAOutput<WRITER>::ODAOutput(WRITER* writer) :
    writer_(writer),
    it_(writer->begin()),
    count_(0)
{}

template<typename WRITER>
ODAOutput<WRITER>::~ODAOutput() {}

template<typename WRITER>
void ODAOutput<WRITER>::print(std::ostream& s) const
{ 
    s << "ODAOutput: iterator: " << it_ << std::endl; // " metaData_: " <<  metaData_ << std::endl;;
}

template<typename WRITER>
unsigned long long ODAOutput<WRITER>::count() { return count_; }

template<typename WRITER>
void ODAOutput<WRITER>::reset() { count_ = 0; }

template<typename WRITER>
void ODAOutput<WRITER>::flush() {}

template<typename WRITER>
void ODAOutput<WRITER>::cleanup(SQLSelect& sql) { sql.outputFiles((**it_).outputFiles()); }

template<typename WRITER>
bool ODAOutput<WRITER>::output(const expression::Expressions& results)
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
void ODAOutput<WRITER>::preprepare(SQLSelect& sql) {}

template<typename WRITER>
void ODAOutput<WRITER>::prepare(SQLSelect& sql)
{
    const expression::Expressions& columns (sql.output());

    //if (metaData_.size()) {
    //    Log::debug<LibOdc>() << "ODAOutput: Using meta of INTO table" << std::endl;
    //    ASSERT(metaData_.size() == n);
    //    const_cast<MetaData&>(it_->columns()) = metaData_;
    //}
    //else
    //{

        const_cast<MetaData&>(it_->columns()).setSize(columns.size());

        for (size_t i = 0; i < columns.size(); i++) {

            SQLExpression& c(*columns[i]);
            api::ColumnType typ = sqlToOdbType(*c.type());

            if (typ == BITFIELD) {
                (**it_).setBitfieldColumn(i, c.title(), typ, c.bitfieldDef());
            } else {
                (**it_).setColumn(i, c.title(), typ);
            }

            (**it_).missingValue(i, c.missingValue());
        }
    //}
    (**it_).writeHeader();
    Log::debug<LibOdc>() << " => ODAOutput: " << std::endl << (**it_).columns() << std::endl;
}

// Direct output functions removed in order output

template <typename WRITER> void ODAOutput<WRITER>::outputReal(double, bool) { NOTIMP; }
template <typename WRITER> void ODAOutput<WRITER>::outputDouble(double, bool) { NOTIMP; }
template <typename WRITER> void ODAOutput<WRITER>::outputInt(double, bool) { NOTIMP; }
template <typename WRITER> void ODAOutput<WRITER>::outputUnsignedInt(double, bool) { NOTIMP; }
template <typename WRITER> void ODAOutput<WRITER>::outputString(const char*, size_t, bool) { NOTIMP; }
template <typename WRITER> void ODAOutput<WRITER>::outputBitfield(double, bool) { NOTIMP; }


// Explicit template instantiations.

template class ODAOutput<Writer<>>;
template class ODAOutput<DispatchingWriter>;

//----------------------------------------------------------------------------------------------------------------------

} // namespace sql
} // namespace odc
