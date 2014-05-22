/// @file   DataRecord.cc
/// @author Tomas Kral

#include "odb_api/DataRecord.h"

namespace odb {

DataRecord::DataRecord(DataRow& row, const DataColumns& columns)
  : row_(row),
    columns_(columns)
{}

DataField DataRecord::operator[](const std::string& name)
{
    size_t n = columns_.indexOf(name);
    return DataField(row_, columns_[n], n);
}

std::ostream& operator<<(std::ostream& stream, const DataRecord& record)
{
    stream << "(";

    for (size_t i = 0; i < record.size(); i++)
    {
        const DataField field = record[i];
        stream << field << ", ";
    }

    return stream << "\b\b)";
}

} // namespace odb
