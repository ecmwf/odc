/// @file   DataField.h
/// @author Tomas Kral

#ifndef ODBLIB_DATAFIELD_H_
#define ODBLIB_DATAFIELD_H_

#include "eckit/eckit.h"
#include "odb_api/ColumnType.h"
#include "odb_api/DataColumn.h"
#include "odb_api/DataRow.h"

namespace odc {

class DataRecord;

/** @brief Represents a single field of a data table.
 *
 *  @see DataRecord
 *  @ingroup data
 */
class DataField
{
public:
    /// Returns the field's name.
    const std::string& name() const
    { return column_->name(); }

    /// Returns the field's data type.
    ColumnType type() const
    { return column_->type(); }

    /// Returns the field's value casted to the given type.
    template <typename T>
    T as() const { return row_->get<T>(index_); }

    /// Returns the field's missing value.
    double missingValue() const
    { return column_->missingValue(); }

    /// Returns the field's default value.
    double defaultValue() const
    { return column_->defaultValue(); }

    /// Returns @c true if the field's value is @c NULL (i.e. missing value),
    /// otherwise returns @c false.
    bool isNull() const
    { return (*row_)[index_] == column_->missingValue(); }

    /// Sets the field's value to the @em value.
    template <typename T>
    DataField& operator=(const T& value)
    { row_->DataRow::set<T>(index_, value); return *this; }

private:
    /// Creates new DataField instance.
    DataField(DataRow& row, const DataColumn& column, size_t index);

    DataField& operator=(const DataField&);

    DataRow* const row_;
    const DataColumn* const column_;
    size_t index_;

    friend class DataRecord;
};

std::ostream& operator<<(std::ostream& stream, const DataField& field);

} // namespace odc

#endif // ODBLIB_DATAFIELD_H_
