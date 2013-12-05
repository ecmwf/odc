/// @file   DataView.cc
/// @author Tomas Kral

#include "odblib/DataView.h"
#include "odblib/DataSet.h"
#include "eckit/exception/Exceptions.h"

using namespace std;
using namespace eckit;

namespace odb {

DataViewFiller::DataViewFiller(DataView& view, const DataLink& link)
  : view_(&view),
    fillLinks_(view.viewLinks_),
    linkIt_(link.begin()),
    linkEnd_(link.end()),
    parentIt_(link.parent().begin()),
    childIt_((*linkIt_).begin()),
    childEnd_((*linkIt_).end()),
    parentOffset_(0),
    parentSize_(link.parent().columns().size()),
    childOffset_(0),
    childSize_(link.child().columns().size()),
    linkOffset_(0),
    linkSize_(2),
    offset_(0),
    fillParent_(true)
{
    ASSERT(link.size() == link.parent().size());

    parentOffset_ = columnIndex(link.parent().columns()[0].name());
    childOffset_ = columnIndex(link.child().columns()[0].name());

    if (fillLinks_)
    {
        std::string parentName = link.parent().name();
        std::string childName = link.child().name();
        linkOffset_ = columnIndex(childName + ".offset@" + parentName);
    }
}

void DataViewFiller::fill(double* const data)
{
    if (fillParent_)
    {
        const DataRow& row = *parentIt_;
        copy(row.data(), row.data() + parentSize_, data + parentOffset_);

        if (fillLinks_)
        {
            data[linkOffset_] = offset_;
            data[linkOffset_ + 1] = linkIt_->size();

            offset_ += linkIt_->size();
        }

        ++parentIt_;
        ++linkIt_;

        fillParent_ = false;
    }

    copy((*childIt_).data(), (*childIt_).data() + childSize_,
            data + childOffset_);
    ++childIt_;

    if ((childIt_ == childEnd_) && (linkIt_ != linkEnd_))
    {
        childIt_ = (*linkIt_).begin();
        childEnd_ = (*linkIt_).end();
        fillParent_ = true;
    }
}

bool DataViewFiller::done() const
{
    return (linkIt_ == linkEnd_) && (childIt_ == childEnd_);
}

size_t DataViewFiller::columnIndex(const std::string& name) const
{
    std::vector<size_t> indices;

    for (size_t i = 0; i < view_->columns().size(); i++)
        if (view_->columns().at(i).name() == name
                || view_->columns().at(i).name().find(name + "@") == 0)
            indices.push_back(i);

    if (indices.size() > 1)
        throw eckit::UserError(std::string("Ambiguous column name: '") + name + "'");

    if (indices.size() == 0)
        throw eckit::UserError(std::string("Column '") + name + "' not found.");

    return indices[0];
}

DataView::DataView(const DataSet& dataset, const std::string& masterName,
        bool viewLinks)
  : columns_(),
    fillers_(),
    rowBuffer_(0),
    rowBufferSize_(0),
    viewLinks_(viewLinks)
{
    typedef std::vector<const DataLink*> Links;

    Links links;

    // Find all links belonging to the master table.
    for (DataLinks::const_iterator it = dataset.links().begin();
            it != dataset.links().end(); ++it)
    {
        const DataLink* link = *it;

        if (link->parent().name() == masterName)
            links.push_back(link);
    }

    // Compute the required size of the row buffer.
    for (Links::const_iterator it = links.begin();
            it != links.end(); ++it)
    {
        const DataLink& link = **it;

        rowBufferSize_ += link.parent().columns().size();
        rowBufferSize_ += link.child().columns().size();

        if (viewLinks_)
            rowBufferSize_ += 2; // offset and length column
    }

    // Allocate row buffer.
    rowBuffer_ = new double [rowBufferSize_];

    // Insert columns from all tables aligned with the master table.
    for (Links::const_iterator it = links.begin();
            it != links.end(); ++it)
    {
        const DataLink& link = **it;
        const DataTable& parent = link.parent();

        columns_.insert(columns_.end(), parent.columns().begin(),
                parent.columns().end());
    }

    // Insert columns from all links.
    if (viewLinks_)
    {
        for (Links::const_iterator it = links.begin();
                it != links.end(); ++it)
        {
        const DataLink& link = **it;
        const DataTable& parent = link.parent();
        const DataTable& child = link.child();

            columns_.push_back(DataColumn(child.name() + ".offset@"
                        + parent.name(), INTEGER));    
            columns_.push_back(DataColumn(child.name() + ".len@"
                        + parent.name(), INTEGER));    
        }
    }

    // Insert columns from all child tables.
    for (Links::const_iterator it = links.begin();
            it != links.end(); ++it)
    {
        const DataLink& link = **it;
        const DataTable& child = link.child();

        columns_.insert(columns_.end(), child.columns().begin(),
                child.columns().end());
    }

    // Initialize all fillers.
    for (Links::const_iterator it = links.begin();
            it != links.end(); ++it)
    {
        const DataLink& link = **it;
        DataViewFiller filler(*this, link);
        fillers_.push_back(filler);
    }
}

DataView::~DataView()
{
    if (rowBuffer_)
        delete [] rowBuffer_;
}

DataView::iterator DataView::begin()
{
    return DataView::iterator(*this, false);
}

DataView::iterator DataView::end()
{
    return DataView::iterator(*this, true);
}

namespace internal {

DataViewIterator::DataViewIterator(DataView& owner, bool end)
  : owner_(&owner),
    fillers_(end ? 0 : &owner.fillers_),
    rowBuffer_(end ? 0 : owner.rowBuffer_),
    rowBufferSize_(end ? 0 : owner.rowBufferSize_),
    done_(end ? true : false)
{
    if (!done_)
    {
        for (DataViewFillers::iterator it = fillers_->begin(),
                end = fillers_->end(); it != end; ++it)
        {
            DataViewFiller& filler = *it;
            filler.fill(rowBuffer_);
        }
    }
}

bool DataViewIterator::operator!=(const DataViewIterator& other)
{
    ASSERT(owner_ == other.owner_);

    if (done_)
        return false;
    else
        done_ = (*fillers_)[0].done();
    return true;
}

DataViewIterator& DataViewIterator::operator++()
{
    if (!done_)
    {
        for (DataViewFillers::iterator it = fillers_->begin(),
                end = fillers_->end(); it != end; ++it)
        {
            DataViewFiller& filler = *it;
            filler.fill(rowBuffer_);
        }
    }

    return *this;
}

} // namespace internal
} // namespace odb
