/// @file   DataJoin.h
/// @author Tomas Kral

#ifndef DATAJOIN_H_
#define DATAJOIN_H_

#include "odb_api/DataJoinIterator.h"
#include "odb_api/SharedIterator.h"

namespace odb {

class DataTable;
class DataLink;

/*! Represents two joined tables.
 *
 *  The DataJoin class provides a view of two joined tables. The two tables
 *  are joined on a matching key. The DataJoin provides an input iterator,
 *  which is a foreward iterator giving read-only access to the records of the 
 *  two joined tables.
 *
 *  @ingroup data
 */
class DataJoin
{
public:
    enum Type { INNER = 0, LINKED = 1 };

    /// Iterator providing read-only access to the result of joined tables.
    typedef SharedIterator<internal::DataJoinIterator> iterator;

    /// Creates join of two tables given the primary and foreign key.
    /// If the optional parameter @e linked is @c true (default), DataJoin
    /// will assume that the two tables are linked and thus will optimize a
    /// foreign key lookups.

    DataJoin(const DataTable& left, const DataTable& right,
             const std::string& primaryKey, const std::string& foreignKey,
             Type type = LINKED);

    /// Creates join of linked tables.
    explicit DataJoin(const DataLink& link);

    /// Returns columns of the joined tables.
    const DataColumns& columns() const
    { return joinedColumns_; }

    /// Returns reference to the left table.
    const DataTable& leftTable() const { return leftTable_; }

    /// Returns reference to the right table.
    const DataTable& rightTable() const { return rightTable_; }

    /// Returns primary key of the left table.
    const std::string& primaryKey() const { return primaryKey_; }

    /// Returns foreign key of the right table.
    const std::string& foreignKey() const { return foreignKey_; }

    /// Returns iterator pointing to the first join result.
    iterator begin() const;

    /// Returns iterator pointing past the last join result.
    iterator end() const;

private:
    DataJoin(const DataJoin&);
    DataJoin& operator=(const DataJoin&);

    const DataTable& leftTable_;
    const DataTable& rightTable_;
    const DataColumns joinedColumns_;
    std::string primaryKey_;
    std::string foreignKey_;
    Type type_;

    friend class internal::DataJoinIterator;
};

} // namespace odb

#endif // DATAJOIN_H_
