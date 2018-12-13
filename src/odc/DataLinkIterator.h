/// @file   DataLinkIterator.h
/// @author Tomas Kral

#ifndef DATALINKITERATOR_H_
#define DATALINKITERATOR_H_

#include "eckit/eckit.h"
#include "odc/DataRow.h"
#include "odc/DataTable.h"

namespace odc {

class DataLink;

namespace internal {

struct DataLinkEntry
{
    DataLinkEntry(size_t offset, size_t length)
      : offset(offset), length(length) {}

    size_t offset;
    size_t length;
};

/// Provides member types required by DataLinkIterator class.
template <typename T>
struct DataLinkIteratorTraits
{
    typedef typename T::link_type link_type;
    typedef typename T::table_iterator_type table_iterator_type;
    typedef typename T::entry_iterator_type entry_iterator_type;
};

/// Template spacialization of DataLinkIteratorTraits for the DataRow class.
template <>
struct DataLinkIteratorTraits<DataRow>
{
    typedef DataLink link_type;
    typedef DataTable::iterator table_iterator_type;
    typedef std::vector<DataLinkEntry>::iterator entry_iterator_type;
};

/// Template spacialization of DataLinkIteratorTraits for the @c const DataRow class.
template <>
struct DataLinkIteratorTraits<const DataRow>
{
    typedef const DataLink link_type;
    typedef DataTable::const_iterator table_iterator_type;
    typedef std::vector<DataLinkEntry>::const_iterator entry_iterator_type;
};

/*! @internal
 *
 *  @brief Iterates on DataLink entries.
 *
 *  The DataLinkIterator represents a link between a single row of a parent
 *  table and a set of related rows from a child table. The respective child
 *  rows can be accessed through DataRange by dereferencing the iterator
 *  object.
 *
 *  @tparam Row    represents a row type (DataRow or const DataRow)
 *  @tparam Traits specialization of the DataLinkIteratorTraits class for the
 *                 given row type
 *
 *  @ingroup data
 */
template <typename Row, typename Traits = DataLinkIteratorTraits<Row> >
class DataLinkIterator
  : public BidirectionalIteratorFacade<DataLinkIterator<Row, Traits>,
    DataLinkIterator<Row, Traits> >
{
    typedef typename Traits::link_type link_type;
    typedef typename Traits::table_iterator_type table_iterator_type;
    typedef typename Traits::entry_iterator_type entry_iterator_type;
public:
    DataLinkIterator();

    template <typename R, typename T>
    DataLinkIterator(const DataLinkIterator<R, T>& other);

    /// Returns iterator to the beginning of the range.
    table_iterator_type begin() const { return begin_; }

    /// Returns iterator past the end of the range.
    table_iterator_type end() const;

    /// Returns the number of rows in the range.
    size_t size() const { return entry_->length; }

private:
    DataLinkIterator(link_type& link, bool begin);

    DataLinkIterator& dereference() const
    { return const_cast<DataLinkIterator&>(*this); }

    void increment()
    { std::advance(begin_, entry_->length); ++entry_; }

    void decrement()
    { std::advance(begin_, (--entry_)->length); }

    template <typename R, typename T>
    bool equal(const DataLinkIterator<R, T>& other) const
    { return (entry_ == other.entry_); }

    entry_iterator_type entry_;
    table_iterator_type begin_;

    friend class odc::DataLink;
    friend class odc::IteratorFacadeAccess;
    template <typename R, typename T> friend class DataLinkIterator;
};

template <typename Row, typename Traits>
template <typename R, typename T>
DataLinkIterator<Row, Traits>::DataLinkIterator(const DataLinkIterator<R, T>& other)
  : entry_(other.entry_),
    begin_(other.begin_)
{}

} // namespace internal
} // namespace odc

#endif // DATALINKITERATOR_H_
