/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <algorithm>
#include <limits>
#include <numeric>

#include "eckit/eckit.h"
#include "eckit/log/Number.h"

#include "eckit/sql/SQLSelect.h"
#include "eckit/sql/expression/SQLExpression.h"

#include "odc/sql/SQLSelectOutput.h"
#include "odc/sql/Types.h"


using namespace eckit;
using namespace eckit::sql;

namespace odc {
namespace sql {

//----------------------------------------------------------------------------------------------------------------------

// TODO: n.b. We can implement an optimised case if the output buffer matches the buffer in the
//            innards of the select, which just does a memcpy. It will be a bit messy, but this
//            is probably the place to do it (although it may rather belong in eckit).

SQLSelectOutput::SQLSelectOutput(bool manageOwnBuffer) :
    out_(0),
    pos_(0),
    end_(0),
    bufferElements_(0),
    count_(0),
    manageOwnBuffer_(manageOwnBuffer),
    isNewDataset_(true),
    newDatasetOutputted_(false) {}

SQLSelectOutput::~SQLSelectOutput() {}

void SQLSelectOutput::resetBuffer(double* out, size_t count) {

    // The fortran interface doesn't pass in the buffer size, so just assume it is correct
    // if the size is specified to be zero
    if (count == 0)
        count = requiredBufferSize_;

    ASSERT(!manageOwnBuffer_);
    out_ = pos_     = out;
    end_            = out_ + count;
    bufferElements_ = count;
    ASSERT(bufferElements_ >= requiredBufferSize_);
}

void SQLSelectOutput::print(std::ostream& s) const {
    s << "SQLSelectOutput";
}


void SQLSelectOutput::reset() {
    pos_ = out_;
}
void SQLSelectOutput::flush() {}

bool SQLSelectOutput::output(const expression::Expressions& results) {
    ASSERT(results.size() == columnSizesDoubles_.size());
    pos_ = out_;
    for (currentColumn_ = 0; currentColumn_ < columnSizesDoubles_.size(); currentColumn_++) {
        results[currentColumn_]->output(*this);
    }
    ASSERT(pos_ == end_);
    count_++;

    if (isNewDataset_) {
        if (newDatasetOutputted_) {
            isNewDataset_        = false;
            newDatasetOutputted_ = false;
        }
        else {
            newDatasetOutputted_ = true;
        }
    }

    return true;
}


void SQLSelectOutput::outputNumber(double x, bool missing) {
    ASSERT(pos_ >= out_ && pos_ < end_);
    *pos_++ = (missing ? missingValues_[currentColumn_] : x);
}

// TODO: We can add special missing-value behaviour here --- with user specified missing values!

void SQLSelectOutput::outputReal(double x, bool missing) {
    outputNumber(x, missing);
}
void SQLSelectOutput::outputDouble(double x, bool missing) {
    outputNumber(x, missing);
}
void SQLSelectOutput::outputInt(double x, bool missing) {
    outputNumber(x, missing);
}
void SQLSelectOutput::outputUnsignedInt(double x, bool missing) {
    outputNumber(x, missing);
}
void SQLSelectOutput::outputBitfield(double x, bool missing) {
    outputNumber(x, missing);
}

void SQLSelectOutput::outputString(const char* s, size_t len, bool missing) {

    ASSERT(pos_ >= out_ && (pos_ + columnSizesDoubles_[currentColumn_]) <= end_);

    size_t charSize = columnSizesDoubles_[currentColumn_] * sizeof(double);
    if (len > charSize) {
        std::ostringstream ss;
        ss << "String too long for configured output: " << len << " > " << charSize;
        throw SeriousBug(ss.str(), Here());
    }

    if (missing) {
        len = 0;
    }
    else {
        ::memcpy(reinterpret_cast<char*>(pos_), s, len);
    }

    if (len < charSize) {
        ::memset(&reinterpret_cast<char*>(pos_)[len], 0, charSize - len);
    }

    pos_ += columnSizesDoubles_[currentColumn_];
}


void SQLSelectOutput::prepare(SQLSelect& sql) {
    updateTypes(sql);
}

void SQLSelectOutput::updateTypes(SQLSelect& sql) {

    size_t offset = 0;
    expression::Expressions output(sql.output());
    metaData_.setSize(output.size());
    offsets_.clear();
    offsets_.reserve(output.size());
    columnSizesDoubles_.clear();
    columnSizesDoubles_.reserve(output.size());
    missingValues_.clear();
    missingValues_.reserve(output.size());

    // TODO: What happens here if the metadata/columns change during an odb?
    // --> We need to update this allocation as we go.
    isNewDataset_        = true;
    newDatasetOutputted_ = false;

    for (size_t i = 0; i < output.size(); i++) {

        // Column sizes for output

        const auto& column(output[i]);
        size_t colSizeBytes = column->type()->size();
        ASSERT(colSizeBytes % 8 == 0);

        columnSizesDoubles_.push_back(colSizeBytes / 8);
        offsets_.push_back(offset);
        offset += columnSizesDoubles_.back();

        // Update the metadata

        metaData_[i]->name(column->title());
        metaData_[i]->type<core::SameByteOrder>(sqlToOdbType(*column->type()));
        metaData_[i]->hasMissing(column->hasMissingValue());
        metaData_[i]->bitfieldDef(column->bitfieldDef());
        metaData_[i]->dataSizeDoubles(columnSizesDoubles_.back());

        // n.b. missing value in the target can be different than for the source. And it can change in the source
        //      between frames. So don't set the target missing value to the sources, use the default one associated
        //      with the default encoder.

        missingValues_.push_back(metaData_[i]->missingValue());
    }

    requiredBufferSize_ = std::accumulate(columnSizesDoubles_.begin(), columnSizesDoubles_.end(), 0);

    // Buffer allocation if necessary

    if (manageOwnBuffer_) {
        data_.resize(offset);
        pos_ = out_     = &data_[0];
        end_            = pos_ + offset;
        bufferElements_ = offset;

        // If a buffer is being provided at the time of doing each request, then we need to
        // test against the buffer size then, not now.
        ASSERT(bufferElements_ >= requiredBufferSize_);
    }
}

void SQLSelectOutput::cleanup(SQLSelect& sql) {}

unsigned long long SQLSelectOutput::count() {
    return count_;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace sql
}  // namespace odc
