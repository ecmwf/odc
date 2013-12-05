/// @file   DataTables.cc
/// @author Tomas Kral

#include "odblib/DataTables.h"
#include "odblib/DataTable.h"
#include "eckit/exception/Exceptions.h"

using namespace std;

namespace odb {

DataTables::DataTables(DataSet& owner)
  : owner_(owner)
{}

void DataTables::insert(DataTable* table)
{
    ASSERT(!table->name().empty() && "DataTable with empty name not allowed.");
    ASSERT(!tableMap_.count(table->name()) && "Duplicate DataTable names not allowed.");
    ASSERT(!table->dataset() && "DataTable owned by other DataSet.");

    table->dataset(&owner_);
    tableMap_[table->name()] = table;
    tableSet_.insert(table);
}

DataTables::iterator DataTables::find(const std::string& name)
{
    std::set<DataTable*>::iterator it = tableSet_.begin();

    for (; it != tableSet_.end(); ++it)
    {
        if ((*it)->name() == name)
            return it;
    }

    return it;
}

DataTable* const DataTables::operator[](const std::string& name)
{
    std::map<std::string, DataTable*>::iterator it = tableMap_.find(name);

    if (it != tableMap_.end())
        return it->second;

    return 0;
}

const DataTable* const DataTables::operator[](const std::string& name) const
{
    return const_cast<const DataTables&>(*this)[name];
}

} // namespace odb
