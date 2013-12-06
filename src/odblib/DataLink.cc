/// @file   DataLink.cc
/// @author Tomas Kral

#include "odblib/DataLink.h"

using namespace std;

namespace odb {

DataLink::DataLink(DataTable& parent, DataTable& child)
  : owner_(0),
    parentTable_(parent),
    childTable_(child),
    entries_(),
    primaryKey_(""),
    foreignKey_(""),
    offsetName_(""),
    lengthName_(""),
    dirty_(false)
{}

DataLink::DataLink(DataTable& parent, DataTable& child,
        const std::string& primaryKey, const std::string& foreignKey)
  : owner_(0),
    parentTable_(parent),
    childTable_(child),
    entries_(),
    primaryKey_(primaryKey),
    foreignKey_(foreignKey),
    offsetName_(""),
    lengthName_(""),
    dirty_(false)
{
    build(primaryKey, foreignKey);
}

DataLink::~DataLink()
{}

DataLink::iterator DataLink::begin()
{
    return DataLink::iterator(*this, true);
}

DataLink::const_iterator DataLink::begin() const
{
    return DataLink::const_iterator(*this, true);
}

DataLink::iterator DataLink::end()
{
    return DataLink::iterator(*this, false);
}

DataLink::const_iterator DataLink::end() const
{
    return DataLink::const_iterator(*this, false);
}

DataTable::iterator DataLink::insert(DataLink::iterator& range,
        const DataTable::iterator& position, const DataRow& row)
{
    ptrdiff_t n = (position - range.begin_);
    DataTable::iterator it(childTable_.insert(position, row));
    range.begin_ = it - n;

    // NOTE: If implementing direct access operator[] in the future,
    // don't forget to recompute offsets when they get dirty after
    // inserting new rows.

    ++(*range.entry_).length;
    dirty_ = true;

    return it;
}

void DataLink::build(const std::string& primaryKey, const std::string& foreignKey)
{
    entries_.reserve(parentTable_.size());

    primaryKey_ = primaryKey;
    foreignKey_ = foreignKey;

    size_t primaryIndex = parentTable_.columnIndex(primaryKey);
    size_t foreignIndex = childTable_.columnIndex(foreignKey);

    size_t offset = 0;
    size_t length = 0;

    for (DataTable::const_iterator parentRow = parentTable_.begin(),
            childRow = childTable_.begin(); parentRow != parentTable_.end()
            && childRow != childTable_.end(); ++parentRow)
    {
        const double key = (*parentRow)[primaryIndex];

        for (length = 0; childRow != childTable_.end()
                && (*childRow)[foreignIndex] == key; ++childRow, ++length);

        entries_.push_back(Entry(offset, length));
        offset += length;
    }
}

} // namespace odb
