/// @file   DataSetFiller.h
/// @author Tomas Kral

#ifndef DATASETFILLER_H_
#define DATASETFILLER_H_

#include "odb_api/DataLinkFiller.h"
#include "odb_api/DataTableFiller.h"

namespace odc { class MetaData; }

namespace odc {

class DataSet;

namespace internal {

class DataSetFillerIterator;

/*! Fills DataSet with data from a source.
 *
 *  @see DataTableFiller, DataLinkFiller
 *  @ingroup data
 */
class DataSetFiller
{
    typedef std::vector<DataTableFiller*> DataTableFillers;
    typedef std::vector<DataLinkFiller*> DataLinkFillers;
public:
    typedef DataSetFillerIterator iterator;

    DataSetFiller(DataSet& dataset, const odc::MetaData& metadata);
    ~DataSetFiller();

    iterator begin();

private:
    void buildFillers(const odc::MetaData& metadata);

private:
    DataSetFiller(const DataSetFiller&);
    DataSetFiller& operator=(const DataSetFiller&);

    DataSet& dataset_;
    DataTableFillers tableFillers_;
    DataLinkFillers linkFillers_;

    friend class DataSetFillerIterator;
};

class DataSetFillerIterator
  : public std::iterator<std::output_iterator_tag, DataSetFillerIterator>
{
    typedef std::vector<DataTableFiller*> DataTableFillers;
    typedef std::vector<DataLinkFiller*> DataLinkFillers;
    typedef std::vector<DataTableFiller::iterator> DataTableFillerIterators;
    typedef std::vector<DataLinkFiller::iterator> DataLinkFillerIterators;
public:
    explicit DataSetFillerIterator(DataSetFiller& filler);
    DataSetFillerIterator& operator*() { return *this; }
    DataSetFillerIterator& operator=(const double* data);
    DataSetFillerIterator& operator++();

private:
    DataSetFiller* owner_;
    DataTableFillerIterators tableFillers_;
    DataLinkFillerIterators linkFillers_;
};

} // namespace internal
} // namespace odc

#endif // DATASETFILLER_H_
