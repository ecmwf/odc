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
/// \file DirectAccess.h
///
/// @author Baudouin Raoult, Dec 2013


#ifndef DirectAccess_H
#define DirectAccess_H

#ifdef SWIGPYTHON
#include <Python.h>
#endif

#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"
#include "eckit/io/HandleHolder.h"

#include "odblib/DirectAccessIterator.h"
#include "odblib/IteratorProxy.h"


namespace eckit { class PathName; }
namespace eckit { class DataHandle; }

namespace odb {

class MetaData;


class DirectAccessBlock {
    size_t n_;
    size_t rows_;
    eckit::Offset offset_;
    eckit::Length length_;
    eckit::DataHandle* handle_;
    double* data_;
    MetaData* metaData_;
    unsigned long long last_;
    unsigned long long unloads_;
unsigned long long loads_;

public:
    DirectAccessBlock(size_t n, size_t rows, const eckit::Offset& offset, const eckit::Length& length):
        n_(n), rows_(rows), offset_(offset),
        length_(length), handle_(0), data_(0), metaData_(0), loads_(0), unloads_(0) {}

    ~DirectAccessBlock();

    size_t n() const { return n_; }
    size_t rows() const { return rows_; }
    eckit::Offset offset() const { return offset_; }
    eckit::Length length() const { return length_; }
    eckit::DataHandle* handle() const { return handle_; }

    void handle(eckit::DataHandle *h) {
        ASSERT(h); ASSERT(!handle_);
        handle_ = h;
        loads_++;
    }

    void unload();

    double* data() { return data_; }

    void data(double *h) {
        ASSERT(h); ASSERT(!data_);
        data_ = h;
    }

    MetaData* metaData() { return metaData_; }

    void metaData(MetaData *h) {
        ASSERT(h); ASSERT(!metaData_);
        metaData_ = h;
    }

    unsigned long long last() const { return last_; }

    void last(unsigned long long h) {
        last_ = h;
    }

    unsigned long long loads() const { return loads_; }
    unsigned long long unloads() const { return unloads_; }

};


class DirectAccess : public eckit::HandleHolder
{
public:

    typedef IteratorProxy<DirectAccessIterator, DirectAccess, double> iterator;
    typedef iterator::Row row;

    DirectAccess(eckit::DataHandle &, size_t maxBlocks = 256);
    DirectAccess(eckit::DataHandle *, size_t maxBlocks);
    DirectAccess(const std::string& path, size_t maxBlocks);

    virtual ~DirectAccess();


    row* operator[](size_t);

    eckit::DataHandle* dataHandle() { return &handle(); }
    // For C API
    DirectAccessIterator* createReadIterator(const eckit::PathName&);
    DirectAccessIterator* createReadIterator();

#ifdef SWIGPYTHON
    iterator __iter__() { return iterator(createReadIterator()); }
#endif

    size_t size() const { return index_.size(); }
    size_t count() const { return blocks_.size(); }

private:
    // No copy allowed
    DirectAccess(const DirectAccess&);
    DirectAccess& operator=(const DirectAccess&);

    void initBlocks();

    std::deque<DirectAccessBlock> blocks_;
    std::vector<std::pair<DirectAccessBlock*,size_t> > index_;


    //const eckit::PathName path_;
    const std::string path_;

    iterator current_;
    row row_;

    DirectAccessBlock* block_;
    size_t idx_;

    size_t maxBlocks_;
    size_t usedBlocks_;



    friend class DirectAccessIterator;
};

} // namespace odb

#endif
