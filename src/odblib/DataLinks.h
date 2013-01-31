/// @file   DataLinks.h
/// @author Tomas Kral

#ifndef DATALINKS_H_
#define DATALINKS_H_

#include <set>

namespace odb {

class DataLink;
class DataSet;
class DataTable;

/*! Represents a collection of dataset links.
 *
 *  The DataLinks collection can be accessed trough the @ref DataSet::links
 *  "links()" method of the DataSet class.
 *
 *  @ingroup data
 */
class DataLinks
{
public:
    /// Iterates over links in dataset.
    typedef std::set<DataLink*>::iterator iterator;

    /// Iterates over links in dataset (const version).
    typedef std::set<DataLink*>::const_iterator const_iterator;

    /// Adds a link to the dataset.
    /// The DataSet takes ownership of the DataLink.
    void insert(DataLink* link);

    /// Finds a link between the @p parent and the @p child table.
    iterator find(const DataTable* const parent,
            const DataTable* const child);

    /// Returns the number of links in dataset.
    size_t size() const { return links_.size(); }

    /// Returns true if there are no links in the dataset.
    bool empty() const { return links_.empty(); }

    /// Returns iterator to the first link in the dataset.
    iterator begin() { return links_.begin(); }

    /// Returns iterator past the last link in the dataset.
    iterator end() { return links_.end(); }

    /// Returns iterator to the first link in the dataset (const overload).
    const_iterator begin() const { return links_.begin(); }

    /// Returns iterator past the last link in the dataset (const overload).
    const_iterator end() const { return links_.end(); }

private:
    /// Creates a news collection of links.
    explicit DataLinks(DataSet& owner);

private:
    DataLinks(const DataLinks&);
    DataLinks& operator=(const DataLinks&);

    DataSet& owner_;
    std::set<DataLink*> links_;

    friend class DataSet;
};

} // namespace odb

#endif // DATALINKS_H_
