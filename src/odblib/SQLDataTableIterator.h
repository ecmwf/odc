#ifndef SQLDATATABLEITERATOR_H_
#define SQLDATATABLEITERATOR_H_

#include <cstddef>
#include <vector>

#include "odblib/SQLTable.h"
#include "odblib/DataTable.h"

namespace odb { namespace sql { class SQLColumn; } }

namespace odb {

class SQLDataTableIterator : public odb::sql::SQLTableIterator
{
public:
    SQLDataTableIterator(const DataTable& table, double* data, const std::vector<odb::sql::SQLColumn*>&);
    ~SQLDataTableIterator();

    virtual void rewind();
    virtual bool next();

private:
    SQLDataTableIterator(const SQLDataTableIterator&);
    SQLDataTableIterator& operator=(const SQLDataTableIterator&);

    void copyRow();

    const DataTable& table_;
    DataTable::const_iterator it_;
    DataTable::const_iterator end_;
    const std::vector<odb::sql::SQLColumn*>& columns_;
    double* data_;
    bool firstRow_;
};

} // namespace odb

#endif // SQLDATATABLEITERATOR_H_
