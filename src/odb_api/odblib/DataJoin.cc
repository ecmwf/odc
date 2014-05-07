/// @file   DataJoin.cc
/// @author Tomas Kral

#include "odb_api/odblib/DataJoin.h"
#include "odb_api/odblib/DataLink.h"

using namespace std;

namespace odb {

DataJoin::DataJoin(const DataTable& left, const DataTable& right,
        const std::string& primaryKey, const std::string& foreignKey, Type type)
  : leftTable_(left),
    rightTable_(right),
    joinedColumns_(left.columns() + right.columns()),
    primaryKey_(primaryKey),
    foreignKey_(foreignKey),
    type_(type)
{}

DataJoin::DataJoin(const DataLink& link)
  : leftTable_(link.parent()),
    rightTable_(link.child()),
    joinedColumns_(leftTable_.columns() + rightTable_.columns()),
    primaryKey_(link.primaryKey()),
    foreignKey_(link.foreignKey()),
    type_(LINKED)
{}

DataJoin::iterator DataJoin::begin() const
{
    return iterator(new internal::DataJoinIterator(*this, true));
}

DataJoin::iterator DataJoin::end() const
{
    return iterator(new internal::DataJoinIterator(*this, false));
}

} // namespace odb
