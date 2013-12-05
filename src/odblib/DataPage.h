/// @file   DataPage.h
/// @author Tomas Kral

#ifndef DATAPAGE_H_
#define DATAPAGE_H_

//#include <cstddef>
//#include <cassert>

//#include "odblib/DataRow.h"

namespace odb {

class DataTable;

namespace internal {

/** @internal
 *
 *  @brief Represents a page of table data.
 *
 *  @ingroup data
 */
class DataPage
{
    enum { MIN_BLOCK_HEIGHT = 1000 };
public:
    /// Creates a new page.
    explicit DataPage(DataTable& table);

    /// Frees the page from memory.
    ~DataPage();

    /// Returns reference to the given row of the page.
    DataRowProxy& operator[](size_t n) const { return begin_[n]; }

    /// Returns reference to the given row of the page.
    DataRowProxy& at(size_t n) const { assert(n < size_); return begin_[n]; }

    /// Returns pointer to the first row in the page.
    DataRowProxy* begin() const { return begin_; }

    /// Returns pointer past the last row in the page.
    DataRowProxy* end() const { return begin_ + size_; }

    /// Returns reference to the firtst row in the page.
    DataRowProxy& front() { return *begin_; }

    /// Returns reference to the firtst row in the page.
    const DataRowProxy& front() const { return *begin_; }

    /// Returns reference to the last row in the page.
    DataRowProxy& back() { return *(begin_ + size_ - 1); }

    /// Returns reference to the last row in the page.
    const DataRowProxy& back() const { return *(begin_ + size_ - 1); }

    /// Returns the number of rows stored in the page.
    size_t size() const { return size_; }

    /// Returns the maximum number of rows that can be stored in the page.
    size_t capacity() const { return height_; }

    /// Attempts to resize the page to the given number of rows.
    /// Returns the actual number of rows to which the page was resized. If the
    /// optional parameter @e initialize is @c true, the elements of the newly
    /// added rows will be initialized to their respective column missing
    /// values. Note that this method does not change the @ref DataPage::capacity
    /// "capacity" of the page but the number of the actively used rows.

    size_t resize(size_t n, bool initialize = false);

    /// Clears the page by dropping the content of all its rows.
    void clear();

    /// Returns @c true if the page is empty.
    bool empty() const { return size_ == 0; }

    /// Returns @c true if the page is full.
    bool full() const { return size_ == capacity(); }

    /// Attempts to add elements of the row at the end of the page.
    ///
    /// This method attempts to copy elements of the supplied @e row at the
    /// end of the data page. Returns @c true if the @e row was successfully
    /// added to the page, otherwise, if the page is already full or the
    /// filling mark has been reached, returns @c false.

    bool push_back(const DataRow& row);

    /// Attempts to add elements of the data array at the end of the page.
    ///
    /// This method attempts to copy elements of the supplied @e data array at the
    /// end of the data page. Returns @c true if the @e data were successfully
    /// added to the page, otherwise, if the page is already full or the
    /// filling mark has been reached, returns @c false. Note that no bound
    /// checking is performed on the supllied @e data array and it is user's
    /// responsibility to provide an array of the proper size.
    //
    /// This method is typically used when populating the page from a data
    /// source which does not provide a DataRow interface.

    bool push_back(const double* const data);

    /// Attempts to insert the row at the given position.
    /// This method attempts to copy elements of the supplied @e row before the
    /// given @e position. Returns @c true if the @e row could be inserted into
    /// the page, otherwise, if the page is already full, returns @c false.
    /// Note that inserting a row into a page may invalidate all previously
    /// obtained pointers and references.

    bool insert(DataRowProxy* position, const DataRow& row);

    /// Splits the page and moves half of its rows to a new page.
    /// Returns pointer to a new page.
    DataPage* split();

    /// Returns link to the next page.
    DataPage* next() const { return next_; }

    /// Sets the link to the next page.
    void next(DataPage* next) { next_ = next; }

    /// Returns link to the previous page.
    DataPage* previous() const { return previous_; }

    /// Sets the link to the previous page.
    void previous(DataPage* previous) { previous_ = previous; }

    /// Returns the rank of the page.
    int rank() const { return rank_; }

    /// Sets the rank of the page.
    void rank(int rank) { rank_ = rank; }

private:
    DataPage(const DataPage&);
    DataPage& operator=(const DataPage&);

    void drop(DataRowProxy* begin, DataRowProxy* end);

    // Aligns first n rows with the data buffer.
    void alignRows(size_t n);

    /// Initializes first n rows of the page.
    void initializeRows(size_t n);

    static size_t optimizeHeight(const DataTable& table);
    static size_t optimizeFillMark(const DataTable& table);

private:
    DataTable& table_;
    size_t width_;
    size_t height_;
    size_t fillMark_;
    size_t size_;
    bool rowsAligned_;
    double* const buffer_;
    double* const data_;
    DataRowProxy* const rows_;
    DataRowProxy* begin_;
    DataPage* previous_;
    DataPage* next_;
    int rank_;
};

} // namespace internal
} // namespace odb

#endif // DATAPAGE_H_
