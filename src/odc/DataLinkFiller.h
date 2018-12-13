/// @file   DataLinkFiller.h
/// @author Tomas Kral

#ifndef DATALINKFILLER_H_
#define DATALINKFILLER_H_

#include "eckit/eckit.h"

namespace odc {

class DataLink;

namespace internal {

class DataLinkFillerIterator;

/*! Fills DataLink with entries from data source.
 *
 *  The DataLinkFiller class populates a DataLink with entries from a data
 *  source (i.e. ODB file).
 *
 *  @ingroup data
 */
class DataLinkFiller
{
public:
    typedef DataLinkFillerIterator iterator;
    explicit DataLinkFiller(DataLink& link);

    void offsetColumnIndex(size_t n) { offsetColumnIndex_ = n; }
    void lenColumnIndex(size_t n) { lenColumnIndex_ = n; }

    iterator begin();

private:
    DataLinkFiller(const DataLinkFiller&);
    DataLinkFiller& operator=(const DataLinkFiller&);

    DataLink& link_;
    size_t offsetColumnIndex_;
    size_t lenColumnIndex_;

    friend class DataLinkFillerIterator;
};

class DataLinkFillerIterator
  : public std::iterator<std::output_iterator_tag, DataLinkFillerIterator>
{
public:
    explicit DataLinkFillerIterator(const DataLinkFiller& filler);
    DataLinkFillerIterator& operator*() { return *this; }
    DataLinkFillerIterator& operator=(const double* data);
    DataLinkFillerIterator& operator++();

private:
    void fillEntry(const double* data);
    void updateRowsToSkip(const double* data);

private:
    DataLink* link_;
    size_t offsetColumnIndex_;
    size_t lenColumnIndex_;
    size_t rowsToSkip_;
};

} // namespace internal
} // namespace odc

#endif // DATALINKFILLER_H_
