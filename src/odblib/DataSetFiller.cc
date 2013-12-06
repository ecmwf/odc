/// @file   DataSetFiller.cc
/// @author Tomas Kral

#include "odblib/DataSetFiller.h"

//#include "odblib/MetaData.h"

//#include "odblib/DataSet.h"
//#include "odblib/DataTable.h"
//#include "odblib/DataLink.h"

using namespace std;

namespace odb {
namespace internal {

DataSetFiller::DataSetFiller(DataSet& dataset, const odb::MetaData& metadata)
  : dataset_(dataset),
    tableFillers_(),
    linkFillers_()
{
    buildFillers(metadata);
}

DataSetFiller::~DataSetFiller()
{
    for (DataTableFillers::iterator it = tableFillers_.begin();
            it != tableFillers_.end(); ++it)
    {
        DataTableFiller* filler = *it;
        delete filler;
    }

    for (DataLinkFillers::iterator it = linkFillers_.begin();
            it != linkFillers_.end(); ++it)
    {
        DataLinkFiller* filler = *it;
        delete filler;
    }
}

void DataSetFiller::buildFillers(const odb::MetaData& metadata)
{
    for (DataTables::iterator it = dataset_.tables().begin();
            it != dataset_.tables().end(); ++it)
    {
        DataTable& table = (**it);

        // Add source columns for the filler.
        DataTableFiller* filler = new DataTableFiller(table);

        for (size_t i = 0; i < table.columns().size(); i++)
        {
            const DataColumn& column = table.columns()[i];
            filler->addColumn(metadata.columnIndex(column.name()));
        }

        // Add link columns for the filler (needed to figure how many rows
        // belong to parent table and how many to child table).
        for (DataLinks::iterator it = dataset_.links().begin(),
                end = dataset_.links().end(); it != end; ++it)
        {
            DataLink& link = (**it);

            if (link.parent().name() == table.name())
            {
                size_t linkLenIndex = metadata.columnIndex(link.lengthName());
                filler->addLink(linkLenIndex);
            }
        }

        tableFillers_.push_back(filler); 
    }

    for (DataLinks::iterator it = dataset_.links().begin(),
            end = dataset_.links().end(); it != end; ++it)
    {
        DataLink& link = (**it);

        size_t offsetColumnIndex = metadata.columnIndex(link.offsetName());
        size_t lenColumnIndex = metadata.columnIndex(link.lengthName());

        DataLinkFiller* filler = new DataLinkFiller(link);

        filler->offsetColumnIndex(offsetColumnIndex);
        filler->lenColumnIndex(lenColumnIndex);

        linkFillers_.push_back(filler);
    }
}

DataSetFiller::iterator DataSetFiller::begin()
{
    return DataSetFiller::iterator(*this);
}

DataSetFillerIterator::DataSetFillerIterator(DataSetFiller& owner)
  : owner_(&owner),
    tableFillers_(),
    linkFillers_()
{
    tableFillers_.reserve(owner.tableFillers_.size());
    for (DataTableFillers::iterator it = owner.tableFillers_.begin(),
            end = owner.tableFillers_.end(); it != end; ++it)
        tableFillers_.push_back((*it)->begin());

    linkFillers_.reserve(owner.linkFillers_.size());
    for (DataLinkFillers::iterator it = owner.linkFillers_.begin(),
            end = owner.linkFillers_.end(); it != end; ++it)
        linkFillers_.push_back((*it)->begin());
}

DataSetFillerIterator& DataSetFillerIterator::operator=(const double* data)
{
    for (DataTableFillerIterators::iterator it = tableFillers_.begin(),
            end = tableFillers_.end(); it != end; ++it)
        (**it) = data;

    for (DataLinkFillerIterators::iterator it = linkFillers_.begin(),
            end = linkFillers_.end(); it != end; ++it)
        (**it) = data;

    return *this;
}

DataSetFillerIterator& DataSetFillerIterator::operator++()
{
    for (DataTableFillerIterators::iterator it = tableFillers_.begin(),
            end = tableFillers_.end(); it != end; ++it)
        ++(*it);

    for (DataLinkFillerIterators::iterator it = linkFillers_.begin(),
            end = linkFillers_.end(); it != end; ++it)
        ++(*it);

    return *this;
}

} // namespace internal
} // namespace odb
