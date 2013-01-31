/// @file   DataLinkFiller.cc
/// @author Tomas Kral

#include "odblib/DataLinkFiller.h"
#include "odblib/DataLink.h"
#include "eclib/Exceptions.h"

using namespace std;

namespace odb {
namespace internal {

DataLinkFiller::DataLinkFiller(DataLink& link)
  : link_(link),
    offsetColumnIndex_(0),
    lenColumnIndex_(0)
{}

DataLinkFiller::iterator DataLinkFiller::begin()
{
    return DataLinkFiller::iterator(*this);
}

DataLinkFillerIterator::DataLinkFillerIterator(const DataLinkFiller& filler)
  : link_(&filler.link_),
    offsetColumnIndex_(filler.offsetColumnIndex_),
    lenColumnIndex_(filler.lenColumnIndex_),
    rowsToSkip_(0)
{}

DataLinkFillerIterator& DataLinkFillerIterator::operator=(const double* data)
{
    if (!rowsToSkip_)
    {
        fillEntry(data);
        updateRowsToSkip(data);
    }

    return *this;
}

DataLinkFillerIterator& DataLinkFillerIterator::operator++()
{
    --rowsToSkip_;
    return *this;
}

void DataLinkFillerIterator::fillEntry(const double* data)
{
    DataLink::Entry entry(data[offsetColumnIndex_], data[lenColumnIndex_]);
    link_->push_back(entry);
}

void DataLinkFillerIterator::updateRowsToSkip(const double* data)
{
    rowsToSkip_ = data[lenColumnIndex_];
    ASSERT(rowsToSkip_ > 0);
}

} // namespace internal
} // namespace odb
