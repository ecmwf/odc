/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

///
/// \file WriterDispatchingIterator.h
///
/// @author Piotr Kuchta, June 2009

#ifndef odc_WriterDispatchingIterator_H
#define odc_WriterDispatchingIterator_H

#include <cstdint>
#include <map>

#include "eckit/sql/SQLTypedefs.h"

#include "odc/TemplateParameters.h"
#include "odc/Writer.h"
#include "odc/api/ColumnType.h"

namespace eckit {
class PathName;
}
namespace eckit {
class DataHandle;
}

namespace odc {

class TemplateParameters;

template <typename WRITE_ITERATOR, typename OWNER>
class WriterDispatchingIterator {
    typedef std::vector<double> Values;
    typedef std::map<Values, int> Values2IteratorIndex;
    typedef std::vector<WRITE_ITERATOR*> Iterators;

public:

    WriterDispatchingIterator(OWNER& owner, int maxOpenFiles, bool append = false);
    ~WriterDispatchingIterator();

    int open();

    double* data();
    double& data(size_t i);

    void setNumberOfColumns(size_t n);
    int setColumn(size_t index, std::string name, api::ColumnType type);
    int setBitfieldColumn(size_t index, std::string name, api::ColumnType type, eckit::sql::BitfieldDef b);

    void missingValue(size_t i, double);

    template <typename T>
    unsigned long pass1(T&, const T&);
    template <typename T>
    void verify(T&, const T&);
    unsigned long gatherStats(const double* values, unsigned long count);

    int close();

    api::ColumnType columnType(size_t index);
    const std::string& columnName(size_t index) const;
    const std::string& codecName(size_t index) const;
    double columnMissingValue(size_t index);

    const core::MetaData& columns() const { return columns_; }
    const core::MetaData& columns(const core::MetaData& md);

    OWNER& owner() { return owner_; }

    void property(std::string key, std::string value);
    std::string property(std::string);

    // std::vector<eckit::PathName> getFiles();
    std::vector<eckit::PathName> outputFiles();
    TemplateParameters& templateParameters() { return templateParameters_; }

    size_t rowDataSizeDoubles() const;

    /// The offset of a given column in the doubles[] data array
    size_t dataOffset(size_t i) const {
        ASSERT(columnOffsets_);
        return columnOffsets_[i];
    }

    // protected:
    void writeHeader();

    int writeRow(const double* values, unsigned long count);

    // If we are encoding strings, and the relevant string column size changes, we need
    // to restart the encoding process
    void flushAndResetColumnSizes(const std::map<std::string, size_t>& resetColumnSizeDoubles);

protected:

    bool next();

    /// Find iterator data should be dispatched to.
    WRITE_ITERATOR& dispatch(const double* values, unsigned long count);
    int dispatchIndex(const double* values, unsigned long count);
    int createIterator(const Values& dispatchedValues, const std::string& fileName);

    std::string generateFileName(const double* values, unsigned long count);

    OWNER& owner_;
    Writer<WRITE_ITERATOR> iteratorsOwner_;
    core::MetaData columns_;
    double* lastValues_;
    double* nextRow_;
    size_t* columnOffsets_;
    unsigned long long nrows_;
    std::string outputFileTemplate_;

    core::Properties properties_;

    std::vector<int> dispatchedIndexes_;
    Values2IteratorIndex values2iteratorIndex_;
    std::vector<unsigned long long> lastDispatch_;
    std::vector<std::string> iteratorIndex2fileName_;

    Values lastDispatchedValues_;
    int lastIndex_;
    bool initialized_;
    bool append_;

private:

    // No copy allowed.
    WriterDispatchingIterator(const WriterDispatchingIterator&);
    WriterDispatchingIterator& operator=(const WriterDispatchingIterator&);

    void parseTemplateParameters();

    int refCount_;

    Iterators iterators_;
    std::vector<eckit::PathName> files_;

    TemplateParameters templateParameters_;
    int maxOpenFiles_;

    std::map<std::string, int> filesCreated_;

    friend class IteratorProxy<WriterDispatchingIterator<WRITE_ITERATOR, DispatchingWriter>, DispatchingWriter>;
};

}  // namespace odc

#include "odc/WriterDispatchingIterator.cc"

#endif
