/// @file   DataRow.h
/// @author Tomas Kral

#ifndef DATAROW_H_
#define DATAROW_H_

//#include <string>
#include <stdint.h>

namespace odb {

class DataTable;
class DataColumns;

namespace internal {

class DataPage;
class DataSelectIterator;
template <typename R, typename T> class DataTableIterator;

/*! @internal
 *  Represents a base class implementing common functionalities
 *  of both DataRow and DataRowProxy classes.
 *
 *  @ingroup data
 */
class DataRowBase
{
public:
    /// Bit flags indicating the status of the row.
    enum Flag
    {
        INITIALIZED = 0x01,
        USED = 0x02,
        MODIFIED = 0x04,
        STANDALONE = 0x08,
    };

protected:
    union Meta
    {
        struct { uint32_t size, flag; } meta;
        uint64_t data;
    };

public:
    /// Returns reference to the given element.
    double& operator[](size_t n) { modified(true); return data()[n]; }

    /// Returns reference to the given element (const overload).
    const double& operator[](size_t n) const { return data()[n]; }

    /// Returns iterator pointing to the first row element.
    double* begin() { modified(true); return data_ + 1; }

    /// Returns iterator pointing to the first row element (const overload).
    const double* begin() const { return data_ + 1; }

    /// Returns iterator pointing past the last row element.
    double* end() { return data_ + 1 + size(); }

    /// Returns iterator pointing past the last row element (const overload).
    const double* end() const { return data_ + 1 + size(); }

    /// Returns pointer to the row's data.
    const double* const data() const { return data_ + 1; }

    /// Returns the number of row's elements.
    const uint32_t& size() const { return meta().meta.size; }

    /// Returns true if the row has been initialized.
    bool initialized() const { return flag() & INITIALIZED; }

    /// Returns true if the row has been modified.
    bool modified() const { return flag() & MODIFIED; }

    /// Returns true if the row is not attached to any table.
    bool standalone() const { return flag() & STANDALONE; }

    /// Returns true if the row is being actively used by the table.
    bool used() const { return flag() & USED; }

protected:
    DataRowBase() : data_(0) {}
    DataRowBase(double* const data) : data_(data) {}

    double* const data() { return data_ + 1; }
    void initialize(const DataColumns& columns);
    Meta& meta() const { return reinterpret_cast<Meta&>(data_[0]); }
    uint32_t& flag() const { return meta().meta.flag; }
    void flag(uint32_t flag) { meta().meta.flag = flag; }
    void size(uint32_t n) { meta().meta.size = n; }
    void initialized(bool b) { b ? flag() |= INITIALIZED : flag() &= (~INITIALIZED); }
    void used(bool b) { b ? flag() |= USED : flag() &= (~USED); }
    void modified(bool b) { b ? flag() |= MODIFIED : flag() &= (~MODIFIED); }
    void standalone(bool b) { b ? flag() |= STANDALONE : flag() &= (~STANDALONE); }

    double* data_;

    friend class odb::internal::DataSelectIterator;
};

} // namespace internal

/*! Represents a row of data.
 *
 *  The DataRow class represents an actual data contained in a DataTable. It is
 *  used to access and modify values in a DataTable. The DataRow can be either
 *  a single standalone row not attached to any table, or it can be a part of a
 *  table. To access rows of a DataTable one can use an @ref DataTable::iterator
 *  "iterator" of the DataTable class or a direct access operator.
 *
 *  @see DataRecord, DataField
 *  @ingroup data
 */
class DataRow : public internal::DataRowBase
{
public:
    /// Creates a new standalone row.
    /// This constructor creates a new row with the given number of elements.
    /// Note that the row elements are left un-initialized.

    explicit DataRow(size_t n);

    /// Creates a new standalone row.
    /// This constructor creates a new row with the given number of elements.
    /// The row elements are initialized to the given default @e value.

    DataRow(size_t n, double value);

    /// Creates a new standalone row.
    /// This constructor creates a new row with the number of elements equal
    /// to the number of @e columns in the supplied DataColumns collection.
    /// If the optional parameter @e initialize is true, the row elements
    /// will be initialized to the default values of their respective columns.

    explicit DataRow(const DataColumns& columns, bool initialize = false);

    /// Creates a new standalone row.
    /// This constructor creates a new row which is a copy of the @e other row.

    DataRow(const DataRow& other);

    /// Frees memory of standalone rows.
    ~DataRow();

    /// Gets a value of the given element casted to the specific type.
    template <typename T>
    T get(size_t index) const
    {
        return static_cast<T>(at(index));
    }

    /// Sets a value of the given element of the row.
    template <typename T>
    void set(size_t index, const T& value)
    {
        modified(true);
        at(index) = static_cast<double>(value);
    }

private:
    DataRow();
    DataRow& operator=(const DataRow&);

    /// Creates a new row from the pre-allocated @e data buffer.
    DataRow(double* const data) : DataRowBase(data) {}

    /// Returns reference to the given element.
    double& at(size_t index) const { return data_[index+1]; }
};

// Declaration of template specializations.

template <>
std::string DataRow::get<std::string>(size_t index) const;

template <>
void DataRow::set<std::string>(size_t index, const std::string& value);

namespace internal {

/*! @internal
 *  Represents a proxy for a data row.
 *
 *  The only difference with DataRow class is the construction, destruction
 *  and copy implementation. The DataRowProxy is used internaly by DataPage
 *  to reference to a pre-allocated memory. The DataRowProxy never owns the
 *  actual data.
 *
 *  @ingroup data
 */
class DataRowProxy : public DataRowBase
{
public:
    using DataRowBase::size;
    using DataRowBase::flag;
    using DataRowBase::data;

    DataRowProxy()
      : DataRowBase() {}

    DataRowProxy(double* const data)
      : DataRowBase(data) {}

    DataRowProxy& operator=(double* const data)
    { data_ = data; return *this; }

    operator DataRow&()
    { return reinterpret_cast<DataRow&>(*this); } 

    operator const DataRow&() const
    { return reinterpret_cast<const DataRow&>(*this); } 

private:
    friend class DataPage;
    friend class DataTable;
    template <typename R, typename T> friend class DataTableIterator;
};

} // namespace internal
} // namespace odb

#endif // DATAROW_H_
