/// @file   DataColumns.h
/// @author Tomas Kral

#ifndef ODBLIB_DATACOLUMNS_H_
#define ODBLIB_DATACOLUMNS_H_

#include "odblib/DataColumn.h"

namespace odb { class MetaData; }

namespace odb {

/*! Represents a collection of table columns.
 *
 *  The DataColumns class defines a schema of a DataTable. You can access
 *  the DataColumns through the @ref DataTable::columns "columns()"
 *  method of a DataTable class.
 *
 *  @ingroup data
 */
class DataColumns : private std::vector<DataColumn>
{
public:
    using std::vector<DataColumn>::push_back;
    using std::vector<DataColumn>::size;
    using std::vector<DataColumn>::empty;
    using std::vector<DataColumn>::begin;
    using std::vector<DataColumn>::end;
    using std::vector<DataColumn>::clear;
    using std::vector<DataColumn>::operator[];
    using std::vector<DataColumn>::operator=;

    /// Iterates through columns in the collection.
    typedef std::vector<DataColumn>::iterator iterator;

    /// Iterates through columns in the collection (const version).
    typedef std::vector<DataColumn>::const_iterator const_iterator;

    /// Creates a new empty collection of columns.
    DataColumns();

    /// Creates and populates a new collection of columns.
    DataColumns(const odb::MetaData& metadata);

    /// Destroys the collection.
    ~DataColumns();

    /// Returns column of the given name.
    DataColumn& operator[](const std::string& name)
    { return (*this)[indexOf(name)]; }

    /// Returns column of the given name (const overload).
    const DataColumn& operator[](const std::string& name) const
    { return (*this)[indexOf(name)]; }

    /// Returns joined collection of the current and the @p other collection.
    DataColumns operator+(const DataColumns& other) const;

    /// Adds columns from the @p other collection to the current one.
    DataColumns& operator+=(const DataColumns& other);

    /// Compares two column collections.
    /// Returns @c true if the two column collections contains the same columns.
    bool operator==(const DataColumns& other) const;

    /// Returns index of the column given its name.
    size_t indexOf(const std::string& name) const;

    /// Adds a new column of the given name and type into the collection.
    void add(const std::string& name, const std::string& type);

    /// Assigns columns from ODB metadata.
    /// This method is provided for inter-operability with ODB library.
    DataColumns& operator=(const odb::MetaData& metadata);
};

} // namespace odb

#endif // ODBLIB_DATACOLUMNS_H_
