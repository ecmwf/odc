/// @file   DataColumn.cc
/// @author Tomas Kral

#include "odb_api/Column.h"
#include "odb_api/DataColumn.h"

using namespace std;

namespace odc {

DataColumn::DataColumn(const std::string& name, ColumnType type)
  : name_(name),
    type_(type),
    missingValue_(defaultMissingValue(type)),
    defaultValue_(missingValue_),
    bitfieldDef_()
{}

DataColumn::DataColumn(const std::string& name, ColumnType type, double missingValue)
  : name_(name),
    type_(type),
    missingValue_(missingValue),
    defaultValue_(missingValue_),
    bitfieldDef_()
{}

DataColumn::DataColumn(const std::string& name, ColumnType type, double missingValue,
        double defaultValue)
  : name_(name),
    type_(type),
    missingValue_(missingValue),
    defaultValue_(defaultValue),
    bitfieldDef_()
{}

DataColumn::DataColumn(const odc::Column& column)
  : name_(column.name()),
    type_(column.type()),
    missingValue_(defaultMissingValue(type_)),
    defaultValue_(missingValue_),
    bitfieldDef_(column.bitfieldDef())
{}

double DataColumn::defaultMissingValue(ColumnType type)
{
    switch (type)
    {
        case INTEGER:  return odc::MDI::integerMDI();
        case REAL:     return odc::MDI::realMDI();
        case DOUBLE:   return odc::MDI::realMDI();
        case STRING:   return *reinterpret_cast<const double*>("        ");
        case BITFIELD: return 0;
        default: ASSERT(!"Unexpected odc::ColumnType.");
    };

    return 0;
}

bool DataColumn::operator==(const DataColumn& other) const
{
    return (name_ == other.name_)
        && (type_ == other.type_)
        && (missingValue_ == other.missingValue_)
        && (defaultValue_ == other.defaultValue_);
}

bool DataColumn::operator!=(const DataColumn& other) const
{
    return (name_ != other.name_)
        || (type_ != other.type_)
        || (missingValue_ != other.missingValue_)
        || (defaultValue_ != other.defaultValue_);
}

} // namespace odc
