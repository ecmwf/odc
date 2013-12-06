/// @file   DataColumns.cc
/// @author Tomas Kral

#include "eckit/eckit.h"
#include "odblib/DataColumns.h"
#include "odblib/MetaData.h"

using namespace std;
using namespace eckit;

namespace odb {

DataColumns::DataColumns()
  : std::vector<DataColumn>()
{}

DataColumns::DataColumns(const odb::MetaData& metadata)
  : std::vector<DataColumn>()
{
    reserve(metadata.size());

    for (size_t i = 0; i < metadata.size(); i++)
    {
        odb::Column& column = *metadata[i];
        push_back(DataColumn(column));
    }
}

DataColumns::~DataColumns()
{}

DataColumns DataColumns::operator+(const DataColumns& other) const
{
    DataColumns columns(*this);
    columns.insert(columns.end(), other.begin(), other.end());
    return columns;
}

DataColumns& DataColumns::operator+=(const DataColumns& other)
{
    insert(end(), other.begin(), other.end());
    return *this;
}

size_t DataColumns::indexOf(const std::string& name) const
{
    std::vector<size_t> indices;

    for (size_t i = 0; i < size(); i++)
        if (at(i).name() == name || at(i).name().find(name + "@") == 0)
                indices.push_back(i);

    if (indices.size() > 1)
        throw eckit::UserError(std::string("Ambiguous column name: '") + name + "'");

    if (indices.size() == 0)
        throw eckit::UserError(std::string("Column '") + name + "' not found.");

    return indices[0];
}

void DataColumns::add(const std::string& name, const std::string& typeName)
{
    ColumnType type;

    if      (typeName == "INTEGER")  type = INTEGER;
    else if (typeName == "REAL")     type = REAL;
    else if (typeName == "DOUBLE")   type = DOUBLE;
    else if (typeName == "STRING")   type = STRING;
    else if (typeName == "BITFIELD") type = BITFIELD;
    else
        throw eckit::UserError("Unsupported column type: " + typeName);

    push_back(DataColumn(name, type));
}

DataColumns& DataColumns::operator=(const odb::MetaData& metadata)
{
    clear();
    reserve(metadata.size());

    for (size_t i = 0; i < metadata.size(); i++)
    {
        odb::Column& column = *metadata[i];
        push_back(DataColumn(column));
    }

    return *this;
}

bool DataColumns::operator==(const DataColumns& other) const
{
    if (this == &other)
        return true;

    if (size() != other.size())
        return false;

    for (size_t i = 0; i < size(); ++i)
    {
        if (!(at(i) == other.at(i)))
            return false;
    }

    return true;
}

} // namespace odb
