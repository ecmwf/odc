/// @file   DataView.h
/// @author Tomas Kral

#ifndef DATAVIEW_H_
#define DATAVIEW_H_

#include "odb_api/DataLink.h"

namespace odc {

class DataSet;
class DataView;

namespace internal { class DataViewIterator; }

/*! Helper class populating a DataView.
 *
 *  The DataViewFiller is responsible for filling up those columns of a
 *  DataView which correspond to the given parent and child tables of a
 *  DataLink that was provided in the constructor.
 *
 *  @ingroup data
 */
class DataViewFiller
{
public:
    DataViewFiller(DataView& view, const DataLink& link);

    void fill(double* const data);
    bool done() const;

private:
    size_t columnIndex(const std::string& name) const;

private:
    DataView* view_;
    bool fillLinks_;
    DataLink::const_iterator linkIt_;
    DataLink::const_iterator linkEnd_;
    DataTable::const_iterator parentIt_;
    DataTable::const_iterator childIt_;
    DataTable::const_iterator childEnd_;
    size_t parentOffset_;
    size_t parentSize_;
    size_t childOffset_;
    size_t childSize_;
    size_t linkOffset_;
    size_t linkSize_;
    size_t offset_;
    bool fillParent_;
};

/*! Provides a flat view of a dataset.
 *
 *  The DataView is typically used when a flat view of a dataset is needed. It
 *  provides a read-only @ref DataView::iterator "iterator" which gives a
 *  sequential access to the flattened representation of the content of all
 *  related tables in a dataset.
 *
 *  Note that presently the DataView works only with one-level deep table
 *  hierarchies and assumes that all tables participating in the parent-child
 *  relationship are aligned.
 *
 *  @ingroup data
 */
class DataView
{
    typedef std::vector<DataViewFiller> DataViewFillers;
public:
    /// Iterates through rows of a flat view.
    typedef internal::DataViewIterator iterator;

    /// Represents collection of DataColumn objects.
    typedef std::vector<DataColumn> Columns;

    /// Creates a new flat view for the given @p dataset.
    /// @param dataset a dataset containing tables to view in the flat format
    /// @param masterName name of the master table whose descendants are to be
    ///                   included in the view
    /// @param viewLinks if @c true, link columns will be included in the view
    DataView(const DataSet& dataset, const std::string& masterName,
            bool viewLinks = true);

    /// Destroys a flat view object.
    ~DataView();

    /// Returns reference to the collection of columns.
    Columns& columns() { return columns_; }

    /// Returns const reference to the collection of columns.
    const Columns& columns() const { return columns_; }

    /// Returns iterator pointing to the beginning of the flat view.
    iterator begin();

    /// Returns iterator pointing past the end of the flat view.
    iterator end();

private:
    DataView(const DataView&);
    DataView& operator=(const DataView&);

    Columns columns_;
    DataViewFillers fillers_;
    double* rowBuffer_;
    size_t rowBufferSize_;
    bool viewLinks_;

    friend class internal::DataViewIterator;
    friend class DataViewFiller;
};

namespace internal {

/*! @internal
 *  @brief Input iterator providing read-only access to flattened dataset view.
 *  @see DataView
 *  @ingroup data
 */
class DataViewIterator :
    public std::iterator<std::input_iterator_tag, const double*>
{
    typedef std::vector<DataViewFiller> DataViewFillers;
public:
    bool operator!=(const DataViewIterator& other);
    const double* operator*() const { return rowBuffer_; }
    DataViewIterator& operator++();

private:
    DataViewIterator(DataView& owner, bool end);

    DataView* owner_;
    DataViewFillers* fillers_;
    double* rowBuffer_;
    size_t rowBufferSize_;
    bool done_;

    friend class odc::DataView;
};

} // namespace internal
} // namespace odc

#endif // DATAVIEW_H_
