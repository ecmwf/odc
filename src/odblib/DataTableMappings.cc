/// @file   DataTableMappings.cc
/// @author Tomas Kral

#include "odblib/DataTableMappings.h"

using namespace std;

namespace odb {

DataTableMappings::DataTableMappings()
  : Map()
{}

void DataTableMappings::add(const string& source, const string& target)
{
    insert(pair<string, string>(source, target));
}

} // namespace odb
