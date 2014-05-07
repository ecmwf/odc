/// @file   DataRecord.h
/// @author Tomas Kral

#ifndef ODBLIB_DATARECORD_H_
#define ODBLIB_DATARECORD_H_

#include "eckit/exception/Exceptions.h"
#include "odb_api/odblib/DataColumns.h"
#include "odb_api/odblib/DataField.h"
#include "odb_api/odblib/DataRecordIterator.h"

namespace odb {

class DataRow;

/** Represents a single table record.
 *
 *  Unlike DataRow, which only provides low-level access to the DataTable
 *  values, the DataRecord class provides a higher-level interface which
 *  combines both the DataRow access methods and the DataColumn meta-data. Each
 *  DataRecord consists of a collection of DataField objects (one field per
 *  column) which can be used to access and manipulate both values and
 *  meta-data of a single table cell.
 *
 *  @code
 *  const DataRow& row = table[0];
 *  const DataRecord record(row, table.columns());
 *
 *  for (DataRecord::const_iterator it = record.begin();
 *          it != record.end(); ++it)
 *  {
 *      const DataField& field = *it;
 *
 *      cout << "name=" << field.name() << std::endl;
 *      cout << "type=" << field.type() << std::endl;
 *      cout << "value=" << field.as<double>() << std::endl;
 *  }
 *  @endcode
 *
 *  @see DataField, DataRow, DataColumn
 *  @ingroup data
 */
class DataRecord
{
    typedef internal::DataRecordIterator<DataRecord, DataField>
            DataRecordIterator;

    typedef internal::DataRecordIterator<const DataRecord, const DataField>
            ConstDataRecordIterator;
public:
    /// Foreward iterator on record's fields.
    typedef DataRecordIterator iterator;

    /// Foreward iterator on record's fields (const version).
    typedef ConstDataRecordIterator const_iterator;

    /// Creates new DataRecord instance.
    DataRecord(DataRow& row, const DataColumns& columns);

    /// Returns the given field.
    DataField operator[](unsigned int n)
    { return DataField(row_, columns_[n], n); }

    /// Returns the given field (const overload).
    const DataField operator[](unsigned int n) const
    { return const_cast<DataRecord&>(*this)[n]; }

    /// Returns the field given its name.
    DataField operator[](const std::string& name);

    /// Returns the field given its name (const overload).
    const DataField operator[](const std::string& name) const
    { return const_cast<DataRecord&>(*this)[name]; }

    /// Returns the given field.
    DataField at(unsigned int n)
    { ASSERT(n < size()); return (*this)[n]; }

    /// Returns the given field (const overload).
    const DataField at(unsigned int n) const
    { return const_cast<DataRecord&>(*this).at(n); }

    /// Returns number of fields in the record.
    unsigned int size() const { return columns_.size(); }

    /// Returns iterator to the first field.
    iterator begin() { return iterator(*this, true); }

    /// Returns iterator to the first field (const version).
    const_iterator begin() const { return const_iterator(*this, true); };

    /// Returs iterator past the last field.
    iterator end() { return iterator(*this, false); }

    /// Returs iterator past the last field (const version.)
    const_iterator end() const { return const_iterator(*this, false); }

private:
    DataRecord(const DataRecord&);
    DataRecord& operator=(const DataRecord&);

    DataRow& row_;
    const DataColumns& columns_;

    template <typename, typename> friend class internal::DataRecordIterator;
};

std::ostream& operator<<(std::ostream& stream, const DataRecord& record);

} // namespace odb

#endif // ODBLIB_DATARECORD_H_
