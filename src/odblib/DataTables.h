/// @file   DataTables.h
/// @author Tomas Kral

#ifndef DATATABLES_H_
#define DATATABLES_H_

//#include <map>
//#include <set>
//#include <string>

namespace odb {

class DataSet;
class DataTable;

/*! Represets a collection of dataset tables.
 *
 *  The DataTables collection can be accessed trough the
 *  @ref DataSet::tables "tables()" method of the DataSet class.
 *
 *  To iterate through the collection of DataTable objects in a DataSet one
 *  can use DataTables::iterator class.
 *
 *  @code
 *  for (DataTables::iterator it = dataset.tables().begin();
 *          it != dataset.tables().end(); ++it)
 *  {
 *      DataTable* table = *it;
 *      cout << table->name() << std::endl;
 *  }
 *  @endcode
 *
 *  @ingroup data
 */
class DataTables
{
public:
    /// Iterates through the collection.
    typedef std::set<DataTable*>::iterator iterator;

    /// Iterates through the collection (const version)
    typedef std::set<DataTable*>::const_iterator const_iterator;

    /// Adds a table to the collection.
    void insert(DataTable* table);

    /// Searches the collection for the table with given name.
    iterator find(const std::string& name);

    /// Returns pointer to table with the given name.
    DataTable* const operator[](const std::string& name);

    /// Returns pointer to table with the given name (const overload).
    const DataTable* const operator[](const std::string& name) const;

    /// Returns the number of tables in the collection.
    size_t size() const { return tableSet_.size(); }

    /// Returns true if there are no tables in the collection.
    bool empty() const { return tableSet_.empty(); }

    /// Returns 1 if a table with the given @p name is found, otherwise returns 0.
    /// This method searches for a table with the given @p name. Since the DataSet
    /// cannot contain two duplicates of the same table, the method returns 1 if
    /// the table was found, and 0 otherwise.
    size_t count(const std::string& name) { return tableMap_.count(name); }

    /// Returns iterator pointing to the beginning of the collection.
    iterator begin() { return tableSet_.begin(); }

    /// Returns iterator pointing to the beginning of the collection (const version).
    const_iterator begin() const { return tableSet_.begin(); }

    /// Returns iterator pointing past the end of the collection.
    iterator end() { return tableSet_.end(); }

    /// Returns iterator pointing past the end of the collection (const version).
    const_iterator end() const { return tableSet_.end(); }

private:
    /// Creates a new collection.
    explicit DataTables(DataSet& owner);

private:
    DataTables(const DataTables&);
    DataTables& operator=(const DataTables&);

    DataSet& owner_;
    std::map<std::string, DataTable*> tableMap_;
    std::set<DataTable*> tableSet_;

    friend class DataSet;
};

} // namespace odb

#endif // DATATABLES_H_
