/// @file   DataLinkIterator.cc
/// @author Tomas Kral

#include "odblib/DataLink.h"
#include "odblib/DataLinkIterator.h"

namespace odb {
namespace internal {

template <typename Row, typename Traits>
DataLinkIterator<Row, Traits>::DataLinkIterator()
  : entry_(),
    begin_()
{}

template <typename Row, typename Traits>
DataLinkIterator<Row, Traits>::DataLinkIterator(link_type& link, bool begin)
  : entry_(begin ? link.entries_.begin() : link.entries_.end()),
    begin_(begin ? link.childTable_.begin() : link.childTable_.end())
{}

template <typename Row, typename Traits>
typename DataLinkIterator<Row, Traits>::table_iterator_type
DataLinkIterator<Row, Traits>::end() const
{
    table_iterator_type end(begin_);
    std::advance(end, entry_->length);
    return end;
}

// Explicit template instantiations.

template struct DataLinkIteratorTraits<DataRow>;
template struct DataLinkIteratorTraits<const DataRow>;

template class DataLinkIterator<DataRow>;
template class DataLinkIterator<const DataRow>;

} // namespace internal
} // namespace odb
