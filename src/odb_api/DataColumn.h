/// @file   DataColumn.h
/// @author Tomas Kral

#ifndef odb_sql_DataColumn_H
#define odb_sql_DataColumn_H

#include "eckit/eckit.h"
#include "odb_api/ColumnType.h"
#include "eckit/sql/SQLTypedefs.h"

namespace odc {

class Column;

/*! Represents a single column of a table.
 *
 *  The DataColumn class is a basic building block for defining the schema
 *  (or structure) of a DataTable. The schema is created by adding one or
 *  more DataColumn objects into a DataColumns collection.
 *
 *  Each DataColumn has a name, type, missing and default values. Columns
 *  representing bitfields also provide a bitfield definition.
 *
 *  @ingroup data
 */
class DataColumn
{
public:
    /// Creates a new column given its @e name and @e type.
    DataColumn(const std::string& name, ColumnType type);

    /// Creates a new column given its @e name, @e type and missing value.
    DataColumn(const std::string& name, ColumnType type, double missingValue);

    /// Creates a new column given its @e name, @e type, missing and default values.
    DataColumn(const std::string& name, ColumnType type, double missingValue,
            double defaultValue);

    /// Creates a new column copying all the properties from an odc::Column.
    DataColumn(const odc::Column& column);

    /// Returns the column's name.
    const std::string& name() const { return name_; }

    /// Returns the column's data type.
    ColumnType type() const { return type_; }

    /// Returns the column's missing value.
    double missingValue() const { return missingValue_; }

    /// Sets the column's missing value.
    void missingValue(double value) { missingValue_ = value; }

    /// Returns the column's default value.
    double defaultValue() const { return defaultValue_; }

    /// Sets the column's default value.
    void defaultValue(double value) { defaultValue_ = value; }

    /// Returns the column's bitfield definition.
    const eckit::sql::BitfieldDef& bitfieldDef() const { return bitfieldDef_; }

    /// Sets the column's bitfield definition.
    void bitfieldDef(const eckit::sql::BitfieldDef& def) { bitfieldDef_ = def; }

    /// Compares two columns for equality.
    /// Returns @c true if the name, type, missing and default values of the
    /// two columns are equal, otherwise returns @c false.

    bool operator==(const DataColumn& other) const;

    /// Compares two columns for in-equality.
    /// Returns @c true if either the name, type, missing or default values of the
    /// two columns are not equal, otherwise returns @c false.

    bool operator!=(const DataColumn& other) const;

private:
    static double defaultMissingValue(ColumnType type);

    std::string name_;
    ColumnType type_;
    double missingValue_;
    double defaultValue_;
    eckit::sql::BitfieldDef bitfieldDef_;
};

} // namespace odc

#endif // DATACOLUMN_H_
