/// @file   DataTableMappings.cc
/// @author Tomas Kral

#include "odb_api/DataTableMappings.h"

using namespace std;

namespace odc {

DataTableMappings::DataTableMappings()
  : Map()
{}

void DataTableMappings::add(const std::string& source, const std::string& target)
{
    insert(pair<std::string, std::string>(source, target));
}

} // namespace odc
