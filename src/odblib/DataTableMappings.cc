/// @file   DataTableMappings.cc
/// @author Tomas Kral

//#include "odblib/DataTableMappings.h"

using namespace std;

namespace odb {

DataTableMappings::DataTableMappings()
  : Map()
{}

void DataTableMappings::add(const std::string& source, const std::string& target)
{
    insert(pair<std::string, std::string>(source, target));
}

} // namespace odb
