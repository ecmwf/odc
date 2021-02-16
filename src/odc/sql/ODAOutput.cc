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
using eckit::sql::expression::SQLExpression;

namespace odc {
namespace sql {

//----------------------------------------------------------------------------------------------------------------------

template<typename WRITER>
ODAOutput<WRITER>::ODAOutput(WRITER* writer) :
    writer_(writer),
    it_(nullptr),
    col_(0),
    count_(0),
    initted_(false)
{}

template<typename WRITER>
ODAOutput<WRITER>::~ODAOutput() {}

template<typename WRITER>
void ODAOutput<WRITER>::print(std::ostream& s) const {
    s << "ODAOutput: iterator: " << it_ << std::endl;
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
    for(col_ = 0; col_ < nCols; ++col_) {
        results[col_]->output(*this);
    }

    ++it_;
    ++count_;
    return true;
}

template<typename WRITER>
void ODAOutput<WRITER>::preprepare(SQLSelect& sql) {}

template<typename WRITER>
void ODAOutput<WRITER>::prepare(SQLSelect& sql) { initUpdateTypes(sql); }

template<typename WRITER>
void ODAOutput<WRITER>::updateTypes(eckit::sql::SQLSelect& sql) { initUpdateTypes(sql); }

template<typename WRITER>
void ODAOutput<WRITER>::initUpdateTypes(eckit::sql::SQLSelect& sql) {
    const expression::Expressions& columns(sql.output());

    if (!initted_) {
        initted_ = true;
        it_ = writer_->begin();
        it_->setNumberOfColumns(columns.size());
        columnSizes_.resize(columns.size());
        missingValues_.resize(columns.size());

        for (size_t i = 0; i < columns.size(); ++i) {

            SQLExpression& c(*columns[i]);
            api::ColumnType typ = sqlToOdbType(*c.type());

            columnSizes_[i] = sizeof(double);
            if (typ == BITFIELD) {
                it_->setBitfieldColumn(i, c.title(), typ, c.bitfieldDef());
            } else {
                it_->setColumn(i, c.title(), typ);
                if (typ == STRING) {
                    size_t maxlen = c.type()->size();
                    columnSizes_[i] = maxlen;
                    ASSERT(maxlen % sizeof(double) == 0);
                    it_->columns()[i]->dataSizeDoubles(maxlen / sizeof(double));
                }
            }
            missingValues_[i] = it_->missingValue(i);
        }

        it_->writeHeader();

        Log::debug<LibOdc>() << " => ODAOutput: " << std::endl << it_->columns() << std::endl;

    } else {

        std::map<std::string, size_t> resetColumnSizeDoubles;

        for (size_t i = 0; i < columns.size(); ++i) {

            SQLExpression& c(*columns[i]);
            api::ColumnType typ = sqlToOdbType(*c.type());

            ASSERT(typ == it_->columns()[i]->type());
            ASSERT(c.title() == it_->columns()[i]->name());

            if (typ == STRING) {
                size_t maxlen = c.type()->size();
                ASSERT(maxlen % sizeof(double) == 0);
                size_t sizeDoubles = maxlen / sizeof(double);
                if (sizeDoubles > it_->columns()[i]->dataSizeDoubles()) {
                    columnSizes_[i] = maxlen;
                    resetColumnSizeDoubles[c.title()] = sizeDoubles;
                }
            } else {
                ASSERT(it_->columns()[i]->dataSizeDoubles() == 1);
            }
        }

        // And if we _do_ need to adjust the column sizes...

        if (!resetColumnSizeDoubles.empty()) {
            it_->flushAndResetColumnSizes(resetColumnSizeDoubles);
        }
    }
}

// Direct output functions removed in order output

template <typename WRITER>
void ODAOutput<WRITER>::outputNumber(double val, bool missing) {
    (*it_)[col_] = (missing ? missingValues_[col_] : val);
}

template <typename WRITER> void ODAOutput<WRITER>::outputReal(double val, bool missing) { outputNumber(val, missing); }
template <typename WRITER> void ODAOutput<WRITER>::outputDouble(double val, bool missing) { outputNumber(val, missing); }
template <typename WRITER> void ODAOutput<WRITER>::outputInt(double val, bool missing) { outputNumber(val, missing); }
template <typename WRITER> void ODAOutput<WRITER>::outputUnsignedInt(double val, bool missing) { outputNumber(val, missing); }
template <typename WRITER> void ODAOutput<WRITER>::outputBitfield(double val, bool missing) { outputNumber(val, missing); }
template <typename WRITER> void ODAOutput<WRITER>::outputString(const char* val, size_t len, bool missing) {
    ::memset(&(*it_)[col_], 0, columnSizes_[col_]);
    ASSERT(len <= columnSizes_[col_]);
    if (!missing) ::strncpy(reinterpret_cast<char*>(&(*it_)[col_]), val, len);
}

// Explicit template instantiations.

template class ODAOutput<Writer<>>;
template class ODAOutput<DispatchingWriter>;

//----------------------------------------------------------------------------------------------------------------------

} // namespace sql
} // namespace odc
