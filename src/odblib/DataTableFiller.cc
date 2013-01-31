/// @file   DataTableFiller.cc
/// @author Tomas Kral

#include "odblib/DataTableFiller.h"

#include <cmath>

#include "eclib/Exceptions.h"
#include "odblib/DataTable.h"

using namespace std;

namespace odb {
namespace internal {

DataTableFiller::DataTableFiller(DataTable& table)
  : targetTable_(table),
    childFiller_(0),
    sourceColumnIndexes_(0),
    linkLenIndexes_(0),
    rowBuffer_(0),
    rowBufferSize_(0)
{}

DataTableFiller::~DataTableFiller()
{
    if (rowBuffer_)
        delete [] rowBuffer_;
}

void DataTableFiller::addColumn(size_t sourceColumnIndex)
{
    sourceColumnIndexes_.push_back(sourceColumnIndex);
}

void DataTableFiller::addLink(size_t linkLenIndex)
{
    linkLenIndexes_.push_back(linkLenIndex);
}

DataTableFiller::iterator DataTableFiller::begin()
{
    if (!rowBuffer_ || sourceColumnIndexes_.size() > rowBufferSize_)
    {
        if (rowBuffer_)
            delete [] rowBuffer_;

        rowBufferSize_ = sourceColumnIndexes_.size();
        rowBuffer_ = new double [rowBufferSize_];
    }

    return DataTableFiller::iterator(*this);
}

DataTableFillerIterator::DataTableFillerIterator(DataTableFiller& owner)
  : owner_(&owner),
    targetTable_(&owner.targetTable_),
    sourceColumnIndexes_(owner.sourceColumnIndexes_),
    linkLenIndexes_(owner.linkLenIndexes_),
    rowBuffer_(owner.rowBuffer_),
    rowsToSkip_(0),
    sourceColumnsAligned_(true)
{
    ASSERT(sourceColumnIndexes_.size() > 0);

    for (size_t i = 0; i < sourceColumnIndexes_.size() - 1; i++)
    {
        if (sourceColumnIndexes_[i+1] != (sourceColumnIndexes_[i] + 1))
        {
            sourceColumnsAligned_ = false;
            break;
        }
    }
}

DataTableFillerIterator::~DataTableFillerIterator()
{}

DataTableFillerIterator& DataTableFillerIterator::operator=(const double* data)
{
    if (!rowsToSkip_)
    {
        fillRow(data);
        updateRowsToSkip(data);
    }

    return *this;
}

DataTableFillerIterator& DataTableFillerIterator::operator++()
{
    rowsToSkip_ = rowsToSkip_ ? --rowsToSkip_ : 0u;
    return *this;
}

void DataTableFillerIterator::updateRowsToSkip(const double* data)
{
    // NOTE: This algorithm only accounts for one level deep table
    // hierarchies.
    rowsToSkip_ = 0;
    for (size_t i = 0; i < linkLenIndexes_.size(); i++)
        rowsToSkip_ = ::max(rowsToSkip_, (size_t)data[linkLenIndexes_[i]]);
}

void DataTableFillerIterator::fillRow(const double* const data)
{
    if (sourceColumnsAligned_)
    {
        size_t offset = sourceColumnIndexes_[0];
        targetTable_->push_back(data + offset);
    }
    else
    {
        for (size_t i = 0; i < sourceColumnIndexes_.size(); i++)
            rowBuffer_[i] = data[sourceColumnIndexes_[i]];

        targetTable_->push_back(rowBuffer_);
    }
}

} // namespace internal
} // namespace odb
