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
/// \file Writer.cc
///
/// @author Piotr Kuchta, Feb 2009

#include "odc/Writer.h"

#include <errno.h>
#include <math.h>
#include <algorithm>
#include <iostream>
#include <sstream>
// #include <values.h>

using namespace std;

#include "eckit/config/Resource.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/DataHandle.h"
#include "eckit/io/FileDescHandle.h"

#include "odc/ODBAPISettings.h"
#include "odc/WriterBufferingIterator.h"
#include "odc/codec/CodecOptimizer.h"
#include "odc/core/Codec.h"
#include "odc/core/Column.h"
#include "odc/core/DataStream.h"
#include "odc/core/MetaData.h"

namespace odc {


template <typename ITERATOR>
Writer<ITERATOR>::Writer() :
    path_(""),
    dataHandle_(0),
    rowsBufferSize_(
        eckit::Resource<long>("$ODB_ROWS_BUFFER_SIZE;-rowsBufferSize;rowsBufferSize", DEFAULT_ROWS_BUFFER_SIZE)),
    openDataHandle_(true),
    deleteDataHandle_(true) {}

template <typename ITERATOR>
Writer<ITERATOR>::Writer(const eckit::PathName& path) :
    path_(path),
    dataHandle_(0),
    rowsBufferSize_(
        eckit::Resource<long>("$ODB_ROWS_BUFFER_SIZE;-rowsBufferSize;rowsBufferSize", DEFAULT_ROWS_BUFFER_SIZE)),
    openDataHandle_(true),
    deleteDataHandle_(true) {
    if (path_ == "/dev/stdout" || path_ == "stdout") {
        eckit::Log::info() << "Writing to stdout" << std::endl;
        dataHandle_     = new eckit::FileDescHandle(1);
        openDataHandle_ = false;
    }
}

template <typename ITERATOR>
Writer<ITERATOR>::Writer(eckit::DataHandle* dh, bool openDataHandle, bool deleteDataHandle) :
    path_(""),
    dataHandle_(dh),
    rowsBufferSize_(
        eckit::Resource<long>("$ODB_ROWS_BUFFER_SIZE;-rowsBufferSize;rowsBufferSize", DEFAULT_ROWS_BUFFER_SIZE)),
    openDataHandle_(openDataHandle),
    deleteDataHandle_(deleteDataHandle) {}

template <typename ITERATOR>
Writer<ITERATOR>::Writer(eckit::DataHandle& dh, bool openDataHandle) :
    path_(""),
    dataHandle_(&dh),
    rowsBufferSize_(
        eckit::Resource<long>("$ODB_ROWS_BUFFER_SIZE;-rowsBufferSize;rowsBufferSize", DEFAULT_ROWS_BUFFER_SIZE)),
    openDataHandle_(openDataHandle),
    deleteDataHandle_(false) {}

template <typename ITERATOR>
Writer<ITERATOR>::~Writer() {
    if (deleteDataHandle_)
        delete dataHandle_;
}

template <typename ITERATOR>
typename Writer<ITERATOR>::iterator Writer<ITERATOR>::begin(bool openDataHandle) {
    eckit::DataHandle* dh = 0;
    if (dataHandle_ == 0) {
        dh = ODBAPISettings::instance().writeToFile(path_, eckit::Length(0), false);
        return typename Writer::iterator(new ITERATOR(*this, dh, openDataHandle));
    }
    else {
        ASSERT(dataHandle_);
        dh = dataHandle_;
        return typename Writer::iterator(new ITERATOR(*this, *dh, openDataHandle));
    }
}

template <typename ITERATOR>
ITERATOR* Writer<ITERATOR>::createWriteIterator(eckit::PathName pathName, bool append) {
    eckit::Length estimatedLength = 10 * 1024 * 1024;
    eckit::DataHandle* h          = append ? ODBAPISettings::instance().appendToFile(pathName, estimatedLength)
                                           : ODBAPISettings::instance().writeToFile(pathName, estimatedLength);
    return new ITERATOR(*this, h, false);
}

// Explicit templates' instantiations.

template Writer<WriterBufferingIterator>::Writer();
template Writer<WriterBufferingIterator>::Writer(const eckit::PathName&);
template Writer<WriterBufferingIterator>::Writer(eckit::DataHandle&, bool);
template Writer<WriterBufferingIterator>::Writer(eckit::DataHandle*, bool, bool);

template Writer<WriterBufferingIterator>::~Writer();
template Writer<WriterBufferingIterator>::iterator Writer<WriterBufferingIterator>::begin(bool);
template WriterBufferingIterator* Writer<WriterBufferingIterator>::createWriteIterator(eckit::PathName, bool);

}  // namespace odc
