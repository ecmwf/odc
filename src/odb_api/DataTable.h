/// @file   DataTable.h
/// @author Tomas Kral

#ifndef odb_sql_DataTable_H
#define odb_sql_DataTable_H

#include "eckit/exception/Exceptions.h"
#include "odb_api/DataRow.h"
#include "odb_api/DataPage.h"
#include "odb_api/DataColumns.h"
#include "odb_api/DataTableIterator.h"

namespace odb {

class DataSet;
class DataTables;

namespace internal
{
    class DataPage;
}

/*! Represents properties of a table.
 *
 *  The DataTableProperties class represents a set of properties that can be
 *  used to fine tune some of the internal memory management options available
 *  in the DataTable class.
 *
 *  The @e blockSizeInKb property can be used to define the size of memory
 *  blocks (in kB) that are used to hold the actual table data. Theoretically,
 *  the best performance should be achieved by setting this value to the size
 *  of the processor cache. The default value is 1024 Kb. Note that the actual
 *  size of the blocks can differ from the one hinted by the @e blockSizeInKb
 *  option since the actual size is also constrained by factors such as number
 *  of columns in the table.
 *
 *  The @e blockFillMark property defines how much space of the individual
 *  block will be used when populating the table with data leaving the
 *  remaining space for efficient insert operations. The accapted values are in
 *  the range 0 to 1. The default value is 1 which means that the whole size of
 *  the block is used when filling the table with data.
 *
 *  @ingroup data
 */
class DataTableProperties
{
    enum
    {
        DEFAULT_BLOCK_SIZE_IN_KB = 1024,
        DEFAULT_BLOCK_SIZE_IN_NUMBER_OF_ROWS = 0,
        DEFAULT_BLOCK_FILL_MARK = 1,
        DEFAULT_BLOCK_FILL_MARK_IN_NUMBER_OF_ROWS = 0
    };

public:
    /// Creates default properties.
    DataTableProperties();

    /// Returns the size of table blocks in Kb.
    size_t blockSizeInKb() const { return blockSizeInKb_; }

    /// Sets the size of table blocks in Kb.
    void blockSizeInKb(size_t kb) { blockSizeInKb_ = kb; }

    /// Returns the size of table blocks in number of rows.
    size_t blockSizeInNumberOfRows() const
    { return blockSizeInNumberOfRows_; }

    /// Sets the size of table blocks in number of rows.
    void blockSizeInNumberOfRows(size_t n)
    { blockSizeInNumberOfRows_ = n; }

    /// Returns the block relative fill mark.
    double blockFillMark() const { return blockFillMark_; }

    /// Sets the block relative fill mark.
    void blockFillMark(double ratio) { blockFillMark_ = ratio; }

    /// Returns the block relative fill mark.
    size_t blockFillMarkInNumberOfRows() const
    { return blockFillMarkInNumberOfRows_; }

    /// Sets the block relative fill mark.
    void blockFillMarkInNumberOfRows(size_t n)
    { blockFillMarkInNumberOfRows_ = n; }

private:
    size_t blockSizeInKb_;
    size_t blockSizeInNumberOfRows_;
    double blockFillMark_;
    size_t blockFillMarkInNumberOfRows_;
};

/*! @brief Represents an in-memory table of data.
 *
 *  A DataTable represents a memory resident table of data. DataTable consists
 *  of DataRow objects, which represent the actual data contained in a table,
 *  and DataColumn objects which describe the schema, or structure of a table.
 *  DataTable can be used independently or as a member of a DataSet, in which
 *  case, it can be accessed through the @ref DataSet::tables "tables()" method
 *  of a DataSet class. 
 *
 *  @ingroup data
 */
class DataTable
{
public:
    typedef internal::DataTableIterator<DataRow> DataTableIterator;
    typedef internal::DataTableIterator<const DataRow> ConstDataTableIterator;

    /// Helper class to insert values into the table.
    class CommaInserter
    {
    public:
        template <typename T>
        CommaInserter(DataTable& table, const T& value)
          : table_(table), 
            row_(0),
            size_(table.columns().size()),
            index_(0)
        {
            table_.resize(table_.size() + 1);
            row_ = &table_.back();
            row_->DataRow::set<T>(index_++, value);
        }

        ~CommaInserter()
        {
            ASSERT((index_ == size_)
                && "DataTable::operator<< received invalid number of values");
        }

        CommaInserter& operator,(const char* value)
        {
            if (index_ == size_)
            {
                table_.resize(table_.size() + 1);
                row_ = &table_.back();
                index_ = 0;
            }

            row_->DataRow::set<std::string>(index_++, std::string(value));

            return *this;
        }

        template <typename T>
        CommaInserter& operator,(const T& value)
        {
            if (index_ == size_)
            {
                table_.resize(table_.size() + 1);
                row_ = &table_.back();
                index_ = 0;
            }

            row_->DataRow::set<T>(index_++, value);

            return *this;
        }

    private:
        DataTable& table_;
        DataRow* row_;
        const size_t size_;
        size_t index_;
    };

public:
    /// Value type of the table.
    /// Provided for compatibility with STL algorithms.

    typedef DataRow value_type;

    /// Reference type of the table.
    /// Provided for compatibility with STL algorithms.

    typedef DataRow& reference;

    /// Constant reference type of the table.
    /// Provided for compatibility with STL algorithms.

    typedef const DataRow& const_reference;

    /// Pointer type of the table.
    /// Provided for compatibility with STL algorithms.

    typedef DataRow* pointer;

    /// Constant pointer type of the table.
    /// Provided for compatibility with STL algorithms.

    typedef const DataRow* const_pointer;

    /// Random-access iterator on table rows.
    /// Provided for compatibility with STL algorithms.

    typedef DataTableIterator iterator;

    /// Random-access iterator on table rows (const version).
    /// Provided for compatibility with STL algorithms.

    typedef ConstDataTableIterator const_iterator;

    /// Creates a new table given its @e name and @e columns.
    DataTable(const std::string& name, const DataColumns& columns,
            const DataTableProperties& properties = DataTableProperties());

    /// Frees the table from memory.
    ~DataTable();

    /// Returns the name of the table.
    const std::string& name() const { return name_; }

    /// Returns pointer to the dataset by which the table is owned.
    /// If the table is not part of a dataset, returns null pointer.
    DataSet* const dataset() const { return owner_; }

    /// Returns reference to the collection of table columns.
    const DataColumns& columns() const { return columns_; }

    /// Returns index of the given column name.
    size_t columnIndex(const std::string& name) const
    { return columns_.indexOf(name); }

    /// Returns @c true if the table is empty.
    bool empty() const;

    /// Returns the number of rows in the table.
    size_t size() const;

    /// Returns the allocated storage in terms of number of rows.
    size_t capacity() const;

    /// Pre-allocates memory for the given number of rows.
    void reserve(size_t n);

    /// Resizes the table to the given number of rows.
    ///
    /// If @e n is smaller than the current table size, the content of the table
    /// is reduced to its first @e n rows, the rest being dropped. If @e n is
    /// greater than the current number of rows, the table content is expanded
    /// by adding new rows at the end of the table. If the optional parameter
    /// @e initialize is set to @c true when expanding the table, the elements
    /// of new rows will be initialized to default values of their respective
    /// columns.

    void resize(size_t n, bool initialize = false);

    /// Returns reference to the given row of the table.
    DataRow& operator[](size_t n);

    /// Returns reference to the given row of the table (const overload).
    const DataRow& operator[](size_t n) const
    { return const_cast<const DataTable&>(*this)[n]; }

    /// Appends content of the given row at the end of the table.
    /// This method copies the content of the supplied @e row at
    /// the end of the table and increases the table size by one.

    void push_back(const DataRow& row);

    /// Appends content of the given data array at the end of the table.
    ///
    /// This method copies the content of the supplied @e data array
    /// at the end of the table and increases the table size by one. Note that no
    /// bound checking is performed on the @e data array and it is caller's
    /// responsibility to provide an array of the proper size.

    void push_back(const double* const data);

    /// Inserts the row at the given position.
    ///
    /// This method copies the content of the supplied @e row before the given
    /// @e position and returns iterator pointing to the newly inserted @e row.
    /// Note that inserting a row into a data table effectively increases its
    /// size and may cause memory reallocations thus invalidating all previously
    /// obtained iterators, pointers and references.

    iterator insert(iterator position, const DataRow& row);

    /// Clears the table by dropping all the rows.
    /// This method only clears the data. The schema of the table stays
    /// unchanged. Note that this method does not release any allocated memory
    /// resoures that have been previously acquired by the table.

    void clear();

    /// Returns iterator pointing to the first row of the table.
    iterator begin()
    { return iterator(frontPage_, frontPage_->begin()); }

    /// Returns iterator pointing to the first row of the table (const overload).
    const_iterator begin() const
    { return const_iterator(frontPage_, frontPage_->begin()); }

    /// Returns iterator pointing past the last row of the table.
    iterator end()
    { return iterator(backPage_, backPage_->end()); }

    /// Returns iterator pointing past the last row of the table (const overload).
    const_iterator end() const
    { return iterator(backPage_, backPage_->end()); }

    /// Returns reference to the last row of the table.
    DataRow& back();

    /// Returns reference to the last row of the table (const overload).
    const DataRow& back() const;

    /// Returns the table properties.
    /// This method returns reference to the instance of DataTableProperties
    /// class. Note that table properties can only be set at the construction
    /// of the DataTable and remain unchanged during its life time.

    const DataTableProperties& properties() const { return properties_; }

    /// Inserts values into the table.
    /// This method provides a convenient way to populate a DataTable
    /// using stream insert operator. Note that the number of supplied values
    /// must be a multiple of the number of columns of the table.
    /// @code
    /// table << 11, 273.0, "temperature",
    ///          12, 0.074, "humidity";
    /// @endcode

    template <typename T>
    CommaInserter operator<<(const T& value)
    {
        return CommaInserter(*this, value);
    }

    /// Inserts table description into the output stream.
    friend std::ostream& operator<<(std::ostream& os, DataTable& table);

private:
    /// Delegates ownership to the given dataset.
    void dataset(DataSet* dataset) { owner_ = dataset; }

    /// Extends the table by an additional block.
    /// This method allocates a new table block appending it at the end of the
    /// list of blocks and returns pointer to this newly created block.

    internal::DataPage* extend();

private:
    DataTable(const DataTable&);
    DataTable& operator=(const DataTable&);

    DataSet* owner_;
    std::string name_;
    DataColumns columns_;
    DataTableProperties properties_;
    internal::DataPage* frontPage_;
    internal::DataPage* backPage_;
    internal::DataPage* endPage_;

    friend class DataTables;
};

} // namespace odb

#endif // odb_sql_DataTable_H
