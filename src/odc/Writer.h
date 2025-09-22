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
/// \file Writer.h
///
/// @author Piotr Kuchta, Feb 2009

#ifndef ODAWRITER_H
#define ODAWRITER_H

#include "odc/IteratorProxy.h"
#include "odc/WriterBufferingIterator.h"

namespace eckit {
class PathName;
}
namespace eckit {
class DataHandle;
}

namespace odc {

typedef WriterBufferingIterator DefaultWritingIterator;

template <typename ITERATOR = DefaultWritingIterator>
class Writer {
    enum {
        DEFAULT_ROWS_BUFFER_SIZE = 10000
    };

public:

    typedef ITERATOR iterator_class;
    typedef IteratorProxy<ITERATOR, Writer> iterator;

    Writer(const eckit::PathName& path);
    Writer(eckit::DataHandle&, bool openDataHandle = true);
    Writer(eckit::DataHandle*, bool openDataHandle = true, bool deleteDataHandle = false);
    Writer();
    virtual ~Writer();

    iterator begin(bool openDataHandle = true);

    eckit::DataHandle& dataHandle() { return *dataHandle_; }

    ITERATOR* createWriteIterator(eckit::PathName, bool append = false);

    unsigned long rowsBufferSize() { return rowsBufferSize_; }
    Writer& rowsBufferSize(unsigned long n) { rowsBufferSize_ = n; }

    const eckit::PathName path() { return path_; }

private:

    // No copy allowed
    Writer(const Writer&);
    Writer& operator=(const Writer&);

    const eckit::PathName path_;
    eckit::DataHandle* dataHandle_;
    unsigned long rowsBufferSize_;

    bool openDataHandle_;
    bool deleteDataHandle_;
};

}  // namespace odc

#endif
