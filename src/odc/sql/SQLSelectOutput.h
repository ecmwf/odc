/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Simon Smart
/// @date Aug 2018

/// @note This output is designed to generalise and standardise the output mechanism for the
/// odc::Select class. Prior to this, the class had two problems:
///
/// i) It invasively used the internals of the SQLSelect and TableIterators, accessing their
///    internal data buffers, and the results objects directly. This is nasty and results in
///    significant duplicate code.
///
/// ii) As a result of the original IFS origins of this code, integers are returned as DOUBLES.
///     this is reasonable, but a bit odd, and it is worth having in code the support to output
///     integers as integers for future/other use - without having to reimplement and duplicate
///     everything.
///
/// This output mechanism allows output to be made to a specified buffer (which may be changed
/// on a per-step basis) with a semantically correct behaviour depending on the column type.
///
/// This has been put in odc, not in eckit, on the understanding that the handling of
/// integers as doubles is esoteric and should probably not be replicated anywhere else.


#ifndef odc_SQLSelectOutput_H
#define odc_SQLSelectOutput_H

#include <vector>
#include "eckit/sql/SQLOutput.h"

#include "odc/core/MetaData.h"


namespace odc {
namespace sql {

//----------------------------------------------------------------------------------------------------------------------

class SQLSelectOutput : public eckit::sql::SQLOutput {

public:  // methods

    /// out/count specify an output buffer and its size in doubles.
    SQLSelectOutput(bool manageOwnBuffer = true);
    virtual ~SQLSelectOutput();

    void resetBuffer(double* out, size_t count);

    // Enable access to the aggregated data from the SelectIterator

    const double* data() const { return out_; }
    double& data(size_t i) { return out_[offsets_[i]]; }
    size_t rowDataSizeDoubles() const { return requiredBufferSize_; }
    const core::MetaData& metadata() const { return metaData_; }
    size_t dataOffset(size_t i) const { return offsets_[i]; }
    bool isNewDataset() const { return isNewDataset_; }

private:  // utility

    void outputNumber(double val, bool missing);

private:  // methods (overrides)

    virtual void print(std::ostream&) const;

    virtual void reset();
    virtual void flush();
    virtual bool output(const eckit::sql::expression::Expressions&);
    virtual void prepare(eckit::sql::SQLSelect&);
    virtual void updateTypes(eckit::sql::SQLSelect&);
    virtual void cleanup(eckit::sql::SQLSelect&);
    virtual unsigned long long count();

    virtual void outputReal(double, bool);
    virtual void outputDouble(double, bool);
    virtual void outputInt(double, bool);
    virtual void outputUnsignedInt(double, bool);
    virtual void outputString(const char*, size_t, bool);
    virtual void outputBitfield(double, bool);

private:  // members

    /// Only used if managing own buffer.
    std::vector<double> data_;

    /// Where are we writing data to (and how many elements can we write)
    double* out_;
    double* pos_;
    double* end_;
    size_t bufferElements_;
    size_t requiredBufferSize_;

    /// How are writes carried out
    std::vector<size_t> columnSizesDoubles_;
    std::vector<size_t> offsets_;
    std::vector<double> missingValues_;

    core::MetaData metaData_;

    /// How much output have we done
    unsigned long long count_;
    size_t currentColumn_;

    bool manageOwnBuffer_;
    bool isNewDataset_;
    bool newDatasetOutputted_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace sql
}  // namespace odc

#endif
