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
/// \file TextReaderIterator.h
///
/// @author Piotr Kuchta, Oct 2010

#ifndef TextReaderIterator_H
#define TextReaderIterator_H

#include "odc/IteratorProxy.h"
#include "odc/core/MetaData.h"

namespace eckit {
class PathName;
}
namespace eckit {
class DataHandle;
}
namespace odc {
namespace sql {
class ODATableIterator;
}
}  // namespace odc

namespace odc {

//----------------------------------------------------------------------------------------------------------------------

class TextReader;

class TextReaderIterator : private eckit::NonCopyable {
public:

    TextReaderIterator(TextReader& owner);
    TextReaderIterator(TextReader& owner, const eckit::PathName&);
    ~TextReaderIterator();

    bool isNewDataset();
    const double* data() const { return lastValues_; }
    double* data() { return lastValues_; }
    double& data(size_t);
    // long integer(int i);

    bool operator!=(const TextReaderIterator& other);

    core::MetaData& columns() { return columns_; }

    static eckit::sql::BitfieldDef parseBitfields(const std::string&);
    // protected:

    int close();

    // next() is public cause it needs to be used by the C API functions - normally client code should not use it
    bool next();

    /// The offset of a given column in the doubles[] data array
    size_t dataOffset(size_t i) const {
        ASSERT(columnOffsets_);
        return columnOffsets_[i];
    }

    // Get the number of doubles per row.
    size_t rowDataSizeDoubles() const { return rowDataSizeDoubles_; }

private:

    // No copy allowed.
    TextReaderIterator(const TextReaderIterator&);
    TextReaderIterator& operator=(const TextReaderIterator&);

    void initRowBuffer();
    void parseHeader();

    core::MetaData columns_;
    double* lastValues_;
    size_t* columnOffsets_;
    size_t rowDataSizeDoubles_;
    unsigned long long nrows_;
    std::string delimiter_;

    std::istream* in_;
    // eckit::DataHandle *f;
    // Properties properties_;

    bool newDataset_;

public:

    bool noMore_;

    bool ownsF_;
    int refCount_;

protected:

    // FIXME:
    TextReaderIterator() : columns_(0) {}

    friend class odc::TextReader;
    friend class odc::IteratorProxy<odc::TextReaderIterator, odc::TextReader, const double>;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace odc

#endif
