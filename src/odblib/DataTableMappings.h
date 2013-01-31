/// @file   DataTableMappings.h
/// @author Tomas Kral

#ifndef DATATABLEMAPPINGS_H_
#define DATATABLEMAPPINGS_H_

#include <map>
#include <string>

namespace odb {

class DataTableMappings : private std::map<std::string, std::string>
{
    typedef std::map<std::string, std::string> Map;
public:
    using Map::iterator;
    using Map::const_iterator;
    using Map::begin;
    using Map::end;
    using Map::find;

    /// Creates a new table mapping.
    DataTableMappings();

    /// Maps the data source table to the given dataset table.
    void add(const std::string& source, const std::string& target);
};

} // namespace odb

#endif // DATATABLEMAPPINGS_H_
