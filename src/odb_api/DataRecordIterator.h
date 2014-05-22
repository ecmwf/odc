/// @file   DataRecordIterator.h
/// @author Tomas Kral

#ifndef ODBLIB_DATARECORDITERATOR_H_
#define ODBLIB_DATARECORDITERATOR_H_

#include <stddef.h>
#include "odb_api/IteratorFacade.h"

namespace odb {

class DataRecord;

namespace internal {

/*! @internal
 *  @brief Foreward iterator on record's fields.
 *  @ingroup data
 */
template <typename R, typename F>
class DataRecordIterator
  : public ForwardIteratorFacade<DataRecordIterator<R, F>, F, F>
{
public:
    DataRecordIterator() {}

    template <typename T, typename U>
    DataRecordIterator(const DataRecordIterator<T, U>& other)
      : record_(other.record_),
        index_(other.index_)
    {}

    template <typename T, typename U>
    DataRecordIterator& operator=(const DataRecordIterator<T, U>& other)
    {
        record_ = other.record_;
        index_ = other.index_;
    }

private:
    DataRecordIterator(R& record, bool begin)
      : record_(&record),
        index_(begin ? 0 : record.size())
    {}

    F dereference() const { return (*record_)[index_]; }
    void increment() { ++index_; }

    template <typename T, typename U>
    bool equal(const DataRecordIterator<T, U>& other) const
    { return index_ == other.index_; }

    template <typename T, typename U>
    ptrdiff_t distance(const DataRecordIterator<T, U>& other) const
    { return (other.index_ - index_); }

    R* record_;
    unsigned int index_;

    friend class odb::DataRecord;
    friend class odb::IteratorFacadeAccess;
    template <typename T, typename U> friend class DataRecordIterator;
};

} // namespace internal
} // namespace odb

#endif // ODBLIB_DATARECORDITERATOR_H_
