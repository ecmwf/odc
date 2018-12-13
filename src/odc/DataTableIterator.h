/// @file   DataTableIterator.h
/// @author Tomas Kral

#ifndef DATATABLEITERATOR_H_
#define DATATABLEITERATOR_H_

#include <stddef.h>

#include "odc/DataPage.h"
#include "odc/IteratorFacade.h"

namespace odc {

class DataTable;
class DataRow;

namespace internal {

template <typename T>
struct DataTableIteratorTraits
{
    typedef typename T::block_type block_type;
    typedef typename T::row_proxy_type row_proxy_type;
};

template <>
struct DataTableIteratorTraits<DataRow>
{
    typedef DataPage block_type;
    typedef DataRowProxy row_proxy_type;
};

template <>
struct DataTableIteratorTraits<const DataRow>
{
    typedef const DataPage block_type;
    typedef const DataRowProxy row_proxy_type;
};

/*! An iterator on data table rows.
 *
 *  The DataTableIterator template implements a random-access iterator on the
 *  rows of a DataTable.
 *
 *  @ingroup data
 *  @internal
 */
template <typename Row, typename Traits = DataTableIteratorTraits<Row> >
class DataTableIterator
  : public RandomIteratorFacade<DataTableIterator<Row, Traits>, Row>
{
    typedef typename Traits::block_type block_type;
    typedef typename Traits::row_proxy_type row_proxy_type;
public:
    DataTableIterator()
     : block_(0), row_(0) {}

    template <typename R, typename T>
    DataTableIterator(const DataTableIterator<R, T>& other)
      : block_(other.block_), row_(other.row_) {}

private:
    DataTableIterator(block_type* block, row_proxy_type* row)
      : block_(block), row_(row) {}

    Row& dereference() const
    { return reinterpret_cast<Row&>(*row_); }

    void increment();
    void decrement();
    void advance(ptrdiff_t n);

    template <typename R, typename T>
    ptrdiff_t distance(const DataTableIterator<R, T>& other) const;

    template <typename R, typename T>
    bool equal(const DataTableIterator<R, T>& other) const
    { return (row_ == other.row_); }

    block_type* block_;
    row_proxy_type* row_;

    friend class DataPage;
    friend class odc::DataTable;
    friend class odc::IteratorFacadeAccess;
    template <typename R, typename T> friend class DataTableIterator;
};

} // namespace internal
} // namespace odc

#endif // DATATABLEITERATOR_H_
