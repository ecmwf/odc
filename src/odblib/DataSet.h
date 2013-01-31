/// @file   DataSet.h
/// @author Tomas Kral

#ifndef DATASET_H_
#define DATASET_H_

#include <string>

#include "odblib/DataTables.h"
#include "odblib/DataLinks.h"

namespace odb {

class DataTable;
class DataLink;

/*! Represents an in-memory representation of a database.
 *
 *  A DataSet is an in-memory data container that looks like a database. A
 *  DataSet consists of a collection of tables that can be related to each
 *  other via links. Whereas the tables hold the actual data, the
 *  links allow to navigate through the table hierarchy.
 *
 *  @ingroup data
 */
class DataSet
{
public:
    /// Creates a new dataset with the given @p name.
    explicit DataSet(const std::string& name = "unknown");

    /// Destroys dataset object.
    virtual ~DataSet();

    /// Returns the name of the dataset.
    const std::string& name() const { return name_; }

    /// Returns reference to the collection of tables.
    DataTables& tables() { return tables_; }

    /// Returns const reference to the collection of tables.
    const DataTables& tables() const { return tables_; }

    /// Returns reference to the collection of links.
    DataLinks& links() { return links_; }

    /// Returns const reference to the collection of links.
    const DataLinks& links() const { return links_; }

    /// Clears the dataset by removing all rows in all tables.
    void clear();

private:
    DataSet(const DataSet&);
    DataSet& operator=(const DataSet&);

    std::string name_;
    DataTables tables_;
    DataLinks links_;
};

} // namespace odb

#endif // DATASET_H_
