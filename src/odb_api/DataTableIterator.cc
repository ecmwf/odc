/// @file   DataTableIterator.cc
/// @author Tomas Kral

#include "odb_api/DataPage.h"
#include "odb_api/DataTableIterator.h"

using namespace std;

namespace odc {
namespace internal {

template <typename Row, typename Traits>
void DataTableIterator<Row, Traits>::increment()
{
    ++row_;

    while (block_)
    {
        if (row_ != block_->end())
            return;

        block_ = block_->next();

        if (block_)
            row_ = block_->begin();
    }
}

template <typename Row, typename Traits>
void DataTableIterator<Row, Traits>::decrement()
{
    while (block_)
    {
        if (row_ != block_->begin())
            break;

        block_ = block_->previous();

        if (block_)
            row_ = block_->end();
    }

    --row_;
}

template <typename Row, typename Traits>
void DataTableIterator<Row, Traits>::advance(ptrdiff_t n)
{
    if (n > 0)
    {
        ptrdiff_t d = min(n, block_->end() - row_);

        if (d < n)
        {
            while (d < n)
            {
                if (!block_->next() || !block_->next()->size()) // Cannot advance past the last elemnt.
                    return;

                block_ = block_->next();

                n -= d;
                d = min(n, (ptrdiff_t) block_->size());
            }

            row_ = block_->begin();
        }

        row_ += n;

        if (row_ == block_->end() && block_->next() && block_->next()->size())
        {
            block_ = block_->next();
            row_ = block_->begin();
        }

    }
    else if (n < 0)
    {
        ptrdiff_t d = max(n, block_->begin() - row_);

        if (d > n)
        {
            while (d > n)
            {
                block_ = block_->previous();

                ASSERT(block_);

                n -= d;
                d = max(n, -(ptrdiff_t) block_->size());
            }

            row_ = block_->end();
        }

        row_ += d;
    }
}

template <typename Row, typename Traits>
template <typename R, typename T>
ptrdiff_t DataTableIterator<Row, Traits>::distance(const DataTableIterator<R, T>& other) const
{
    ptrdiff_t d = 0;

    if (block_ == other.block_)
    {
        d = (row_ - other.row_);
    }
    else if (block_->rank() < other.block_->rank())
    {
        d = (row_ - block_->end());
        block_type* block = block_->next();

        while (block && block != other.block_)
        {
            d -= block_->size();
            block = block_->next();
        }

        d -= (other.row_ - block->begin());
    }
    else if (block_->rank() > other.block_->rank())
    {
        d = (row_ - block_->begin());
        block_type* block = block_->previous();

        while (block && block != other.block_)
        {
            d += block_->size();
            block = block_->previous();
        }

        d += block->end() - other.row_;
    }

    return d;
}


// Explicit template instantiations.

template struct DataTableIteratorTraits<DataRow>;
template struct DataTableIteratorTraits<const DataRow>;

template class DataTableIterator<DataRow>;
template class DataTableIterator<const DataRow>;

template ptrdiff_t
DataTableIterator<DataRow>::distance(const DataTableIterator<DataRow>&) const;
template ptrdiff_t
DataTableIterator<DataRow>::distance(const DataTableIterator<const DataRow>&) const;
template ptrdiff_t
DataTableIterator<const DataRow>::distance(const DataTableIterator<DataRow>&) const;
template ptrdiff_t
DataTableIterator<const DataRow>::distance(const DataTableIterator<const DataRow>&) const;

} // namespace internal
} // namespace odc
