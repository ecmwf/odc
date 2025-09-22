/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Piotr Kuchta
/// @date Feb 2009
/// @author Simon Smart
/// @date Dec 2018

#ifndef Reader_H
#define Reader_H

#ifdef SWIGPYTHON
#include <Python.h>
#endif

#include "eckit/filesystem/PathName.h"
#include "odc/IteratorProxy.h"
#include "odc/ReaderIterator.h"

namespace eckit {
class DataHandle;
}

namespace odc {

class Reader : public eckit::NonCopyable {
public:

    typedef IteratorProxy<ReaderIterator, Reader, const double> iterator;
    typedef iterator::Row row;

    Reader(eckit::DataHandle&);
    Reader(const eckit::PathName& path);
    Reader();

    Reader(Reader&& rhs);
    Reader& operator=(Reader&& rhs);

    ~Reader();

    // TODO: begin() should be const.
    iterator begin();
    const iterator end() const;

    eckit::DataHandle* dataHandle();
    // For C API
    ReaderIterator* createReadIterator(const eckit::PathName&);
    ReaderIterator* createReadIterator();

#ifdef SWIGPYTHON
    iterator __iter__() { return iterator(createReadIterator()); }
#endif

    // For the iterator to signal all data has been slurped.
    void noMoreData();

private:

    eckit::DataHandle* dataHandle_;
    bool deleteDataHandle_;
    eckit::PathName path_;

    friend class IteratorProxy<ReaderIterator, Reader, const double>;
    friend class ReaderIterator;
};

}  // namespace odc

#endif
