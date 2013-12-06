/// @file   DataLink.h
/// @author Tomas Kral

#ifndef DATALINK_H_
#define DATALINK_H_

#include "odblib/DataLinkIterator.h"

namespace odb {

class DataSet;
class DataLinks;
class DataTable;

namespace internal { class DataLinkFillerIterator; }

/*! Represents a link between two tables.
 *
 *  The DataLink class is used to navigate through the rows of related
 *  DataTable objects. The relationship between the two tables is defined by
 *  matching pairs of the primary and foreign keys. The type of both columns
 *  must be identical.
 *
 *  DataLink represents a special case of one-to-many relationship where all
 *  foreign keys in the child table appear in the same order as primary keys
 *  in the parent table and there is at least one matching foreign key per
 *  each primary key of the parent table.
 *
 *  The @ref DataLink::iterator "iterator" of the DataLink class provides
 *  sequential access to the parent rows and at the same time allows to
 *  navigate through all related rows in the child table.
 *
 *  The following code example demonstrates how to first create a link between
 *  two tables related via "parent_id" and "child_id" columns, and how to use
 *  the @ref DataLink::iterator "iterator" of the DataLink class to navigate
 *  through the related rows.
 *
 *  @code
 *  DataLink link(parentTable, childTable, "parent_id", "child_id");
 *
 *  for (DataLink::iterator range = link.begin();
 *          range != link.end(); ++range)
 *  {
 *      for (DataTable::iterator childRow = range->begin();
 *              childRow != range->end(); ++childRow)
 *      {
 *          DataRecord record(*childRow, childTable.columns());
 *          cout << record << std::endl;
 *      }
 *  }
 *  @endcode
 *  
 *  @see DataTable, DataJoin
 *
 *  @ingroup data
 */
class DataLink
{
    typedef internal::DataLinkEntry Entry;
    typedef std::vector<Entry> Entries;
public:
    /// Iterates through DataLink entries.
    typedef internal::DataLinkIterator<DataRow> iterator;

    /// Iterates through DataLink entries (const version).
    typedef internal::DataLinkIterator<const DataRow> const_iterator;

    /// Creates and populates a new link given the @em parent and @em child
    /// table.
    DataLink(DataTable& parent, DataTable& child, const std::string& primaryKey,
            const std::string& foreignKey);

    /// Destroys link object.
   ~DataLink();

    /// Returns pointer to the owner of the link.
    DataSet* dataset() { return owner_; }

    /// Returns reference to the parent table.
    DataTable& parent() { return parentTable_; }

    /// Returns const reference to the parent table.
    const DataTable& parent() const { return parentTable_; }

    /// Returns reference to the child table.
    DataTable& child() { return childTable_; }

    /// Returns const reference to the child table.
    const DataTable& child() const { return childTable_; }

    /// Returns primary key of the parent table.
    const std::string& primaryKey() const { return primaryKey_; }

    /// Returns foreign key of the child table.
    const std::string& foreignKey() const { return foreignKey_; }

    /// Returns iterator to the first link entry.
    iterator begin();

    /// Returns iterator to the first link entry (const version).
    const_iterator begin() const;

    /// Returns iterator past the last link entry.
    iterator end();

    /// Returns iterator past the last link entry (const version).
    const_iterator end() const;

    /// Returns the number of link entries.
    size_t size() const { return entries_.size(); }

    /// Clears the link by removing all entries.
    /// @note Calling this method invalidates all exisisting @ref
    /// DataLink::iterator iterators.
    void clear() { entries_.clear(); }

    /// Returns @c true if there are no link entries.
    bool empty() const { return entries_.empty(); }

    /// Inserts the @em row at the given @em position in the child table.
    DataTable::iterator insert(DataLink::iterator& range,
            const DataTable::iterator& position, const DataRow& row);

    void offsetName(const std::string name) { offsetName_ = name; }
    const std::string& offsetName() const { return offsetName_; }

    const std::string& lengthName() const { return lengthName_; }
    void lengthName(const std::string name) { lengthName_ = name; }

private:
    /// Creates new link between the @em parent and the @em child table.
    DataLink(DataTable& parent, DataTable& child);

    void dataset(DataSet* dataset) { owner_ = dataset; }
    void build(const std::string&, const std::string&);
    void push_back(const Entry entry) { entries_.push_back(entry); }

private:
    DataLink(const DataLink&);
    DataLink& operator=(const DataLink&);

    DataSet* owner_;
    DataTable& parentTable_;
    DataTable& childTable_;
    Entries entries_;
    std::string primaryKey_;
    std::string foreignKey_;
    std::string offsetName_;
    std::string lengthName_;
    bool dirty_;

    friend class DataSetBuilder;
    friend class DataLinks;
    friend class internal::DataLinkFillerIterator;
    template <typename R> friend class internal::DataLinkIteratorTraits;
    template <typename R, typename T> friend class internal::DataLinkIterator;
};

} // namespace odb

#endif // DATALINK_H_
