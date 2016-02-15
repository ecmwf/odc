/// @file   DataSelect.h
/// @author Tomas Kral

#ifndef ODBLIB_DATASELECT_H_
#define ODBLIB_DATASELECT_H_

#include "eckit/eckit.h"
#include "experimental/eckit/ecml/core/ExecutionContext.h"

#include "odb_api/DataColumns.h"
#include "odb_api/SharedIterator.h"

namespace odb { namespace sql { class SQLDatabase; } }

namespace odb {

class DataSet;
class DataTable;

namespace internal { class DataSelectIterator; }

/*! Provides means of executing SQL select statements on in-memory data tables.
 *
 * The DataSelect class is typically used in situations when you need to
 * execute one or more SQL select statements on a single DataTable. This table
 * can be either a standalone table or a table which is part of a DataSet.
 *
 * The primary advantage of using DataSelect over other SQL querying methods
 * (e.g.  odb::Select) is that it operates purely on in-memory data and thus
 * eliminates any overhead that might be incurred by reading data direcly from
 * disk devices.  This becomes increasingly relevant if you want to perform
 * more than one SQL query on the same table during the execution of your
 * program. However, the downside of this method is that it requires to hold
 * the entire table in memory so one needs to choose the right tradeoff.
 *
 * Results of the SQL query can be obtained using the @ref DataSelect::iterator
 * "iterator" of the DataSelect class. This iterator provides a sequential,
 * read-only access to the individual @ref DataRow "rows" of the SQL query's
 * result.
 *
 * To see an example how to use DataSelect to populate a DataTable one can
 * refer to @ref UsingDataSelectToPopulateADataTable.
 * 
 * @see DataTable, DataSet
 * 
 * @ingroup data
 */
class DataSelect
{
public:
    /// Represents an iterator on the results of the SQL query.
    typedef SharedIterator<internal::DataSelectIterator> iterator;

    /// Creates new query on the given @em dataset.
    DataSelect(const std::string& statement, const DataSet& dataset, eckit::ExecutionContext*);

    /// Creates new query on the given @em table.
    DataSelect(const std::string& statement, const DataTable& table, eckit::ExecutionContext*);

    /// Frees the query from memory.
    ~DataSelect();

    /// Returns columns of the query's result.
    const DataColumns& columns() const { return columns_; }

    /// Returns the query's statement.
    const std::string& statement() const { return statement_; }

    /// Returns iterator pointing to the first result of the query.
    iterator begin();

    /// Returns iterator pointing past the last result of the query.
    iterator end() const;

private:
    DataSelect(const DataSelect&);
    DataSelect& operator=(const DataSelect&);

    odb::sql::SQLDatabase* database() const;
    void populateColumns();

    const std::string statement_;
    const DataSet* const dataset_;
    const DataTable* const table_;
    std::auto_ptr<internal::DataSelectIterator> begin_;
    DataColumns columns_;
    eckit::ExecutionContext* context_;

    friend class internal::DataSelectIterator;
};

} // namespace odb

#endif // ODBLIB_DATASELECT_H_
