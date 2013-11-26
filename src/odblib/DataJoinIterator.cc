/// @file   DataJoinIterator.cc
/// @author Tomas Kral

#include "odblib/DataJoinIterator.h"
#include "odblib/DataJoin.h"

#include <sstream>

using namespace std;
using namespace eckit;

namespace odb {
namespace internal {

DataJoinIterator::DataJoinIterator(const DataJoin& join, bool begin)
  : join_(join),
    columnsSize_(join.columns().size()),
    rowsSize_(10),
    cacheSize_(rowsSize_ * (columnsSize_ + 1)), // + 1 extra column for row metadata
    cache_(begin ? new double [cacheSize_] : 0),
    cacheIt_(cache_),
    cacheEnd_(cache_ + cacheSize_),
    rowProxy_(cacheIt_),
    leftIt_(begin ? join.leftTable().begin() : join.leftTable().end()),
    leftEnd_(join.leftTable().end()),
    leftSize_(join.leftTable().columns().size()),
    rightIt_(begin ? join.rightTable().begin() : join.rightTable().end()),
    rightEnd_(join.rightTable().end()),
    rightSize_(join.rightTable().columns().size()),
    primaryKeyIndex_(join.leftTable().columnIndex(join.primaryKey())),
    foreignKeyIndex_(join.rightTable().columnIndex(join.foreignKey())),
    type_(join.type_),
    done_(begin ? false : true)
{
    if (begin)
    {
        ASSERT(cache_);
        initialize();
        populate();
    }
}

DataJoinIterator::~DataJoinIterator()
{
    delete [] cache_;
}

void DataJoinIterator::increment()
{
    if (!done_)
    {
        cacheIt_ += columnsSize_ + 1;

        if (cacheIt_ == cacheEnd_)
        {
            if (leftIt_ == leftEnd_ && rightIt_ == rightEnd_)
            {
                done_ = true;
                return;
            }

            populate();
            cacheIt_ = cache_;
        }

        rowProxy_ = cacheIt_;
    }
}

void DataJoinIterator::populate()
{
    switch (type_) {

    case DataJoin::LINKED:
    {
        double primaryKey = (*leftIt_)[primaryKeyIndex_];
        copy(leftIt_->data(), leftIt_->data() + leftSize_, cache_ + 1);

        double* cacheIt = cache_;
        for (; cacheIt != cacheEnd_ && rightIt_ != rightEnd_;
                cacheIt += columnsSize_ + 1, ++rightIt_)
        {
            double foreignKey = (*rightIt_)[foreignKeyIndex_];

            if (foreignKey != primaryKey)
            {
                ++leftIt_; 
                copy(leftIt_->data(), leftIt_->data() + leftSize_, cacheIt + 1);
                primaryKey = (*leftIt_)[primaryKeyIndex_];

                if (foreignKey != primaryKey)
                {
                    std::stringstream message("DataJoinIterator: ");
                    message << "Mismatch of primary and foreign keys in linked tables.";
                    message << std::endl;
                    throw eckit::UserError(message.str());
                }
            }
            else if (cacheIt > cache_)
            {
                double* previous = cacheIt - columnsSize_ - 1;
                copy(previous + 1, previous + 1 + leftSize_, cacheIt + 1);
            }

            copy(rightIt_->data(), rightIt_->data() + rightSize_,
                    cacheIt + 1 + leftSize_);
        }

        // If reached the end of the right table, increment left table iterator
        // to point to the end.
        
        if (rightIt_ == rightEnd_)
            ++leftIt_;

        cacheEnd_ = cacheIt;
    }
    break;

    case DataJoin::INNER:
    {
        double primaryKey = (*leftIt_)[primaryKeyIndex_];
        bool primaryKeyChanged = true;

        double* cacheIt = cache_;

        // Loop until the cache is full or we reached end of the left table.
        while (cacheIt != cacheEnd_ && leftIt_ != leftEnd_)
        {
            // Find matching foreign key in the right table.
            for (; rightIt_ != rightEnd_ &&
                    (*rightIt_)[foreignKeyIndex_] != primaryKey; ++rightIt_);

            if (rightIt_ != rightEnd_)
            {
                // Found matching foreign key in the right table.
                
                if (primaryKeyChanged)
                {
                    // Copy new row from the left table.
                    copy(leftIt_->data(), leftIt_->data() + leftSize_, cacheIt + 1);
                    primaryKeyChanged = false;
                }
                else
                {
                    // The same row is already in the cache so copy it from
                    // the previous cache row.
                    DataRowProxy prevRow = cacheIt - columnsSize_ - 1;
                    copy(prevRow.data(), prevRow.data() + leftSize_, cacheIt + 1);
                }

                // Copy content of the right table row.
                copy(rightIt_->data(), rightIt_->data() + rightSize_,
                        cacheIt + 1 + leftSize_);

                ++rightIt_;
                cacheIt += columnsSize_ + 1;
            }
            else if (++leftIt_ != leftEnd_)
            {
                double key = (*leftIt_)[primaryKeyIndex_];
                ASSERT(key != primaryKey); // TODO: throw exception rather than assert
                primaryKey = key;
                primaryKeyChanged = true;
                rightIt_ = join_.rightTable().begin();
            }
        }
        
        cacheEnd_ = cacheIt;
    }
    break;

    default:
        ASSERT(!"DataJoinIterator: Unexpected join type.");
    }
}

void DataJoinIterator::initialize()
{
    for (double* data = cache_; data != cache_ + cacheSize_;
            data += columnsSize_ + 1)
    {
        DataRowProxy row(data);
        row.size(columnsSize_);
        row.flag(0);
    }
}

} // namespace internal
} // namespace odb
