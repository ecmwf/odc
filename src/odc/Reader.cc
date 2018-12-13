/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "odc/data/DataHandleFactory.h"
#include "odc/Reader.h"

using namespace std;
using namespace eckit;

namespace odc {

Reader::Reader(DataHandle &dh)
: dataHandle_(&dh),
  deleteDataHandle_(false) {}

Reader::Reader()
: dataHandle_(0),
  deleteDataHandle_(true),
  path_("")
{}

Reader::Reader(Reader&& rhs) :
    dataHandle_(rhs.dataHandle_),
    deleteDataHandle_(rhs.deleteDataHandle_),
    path_(std::move(rhs.path_)) {

    rhs.dataHandle_ = 0;
    rhs.deleteDataHandle_ = false;
}

Reader& Reader::operator=(Reader&& rhs) {
    std::swap(dataHandle_, rhs.dataHandle_);
    std::swap(deleteDataHandle_, rhs.deleteDataHandle_);
    std::swap(path_, rhs.path_);
    return *this;
}

Reader::Reader(const std::string& path)
: dataHandle_(DataHandleFactory::openForRead(path)),
  deleteDataHandle_(true),
  path_(path)
{}

Reader::~Reader()
{
    if (dataHandle_ && deleteDataHandle_)
    {
        dataHandle_->close();
        delete dataHandle_;
    }
}

ReaderIterator* Reader::createReadIterator(const PathName& pathName)
{
    return new ReaderIterator(*this, pathName);
}

ReaderIterator* Reader::createReadIterator()
{
return createReadIterator(path_);
}

Reader::iterator Reader::begin()
{
    ReaderIterator * it = new ReaderIterator(*this);
    it->next();
    return iterator(it);
}

const Reader::iterator Reader::end() const {
    return iterator(0);
}

void Reader::noMoreData()
{
    if (dataHandle_ && deleteDataHandle_)
    {
        dataHandle_->close();
        delete dataHandle_;
    }
    dataHandle_ = 0;
}

eckit::DataHandle* Reader::dataHandle()
{
    // Assume the Reader was constructed with a path, and not a DataHandle*
    if (! dataHandle_)
        dataHandle_ = odc::DataHandleFactory::openForRead(path_);
    return dataHandle_; 
}

} // namespace odc
