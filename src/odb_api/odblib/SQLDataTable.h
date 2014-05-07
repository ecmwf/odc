#ifndef SQLDATATABLE_H_
#define SQLDATATABLE_H_

#include "odb_api/odblib/SQLTable.h"

namespace odb {
namespace sql {

    class SQLColumn;
    class SQLDatabase;
    class SQLTableIterator;

}}

namespace odb {

class DataTable;

class SQLDataTable : public odb::sql::SQLTable
{
public:
    SQLDataTable(odb::sql::SQLDatabase& db, const DataTable& table);

    ~SQLDataTable();

    virtual odb::sql::SQLColumn* column(const std::string& name);
    virtual bool hasColumn(const std::string& name, std::string* fullName = 0);
    virtual odb::sql::SQLTableIterator* iterator(const std::vector<odb::sql::SQLColumn*>&) const;

protected:
	virtual odb::sql::SQLColumn* createSQLColumn(const odb::sql::type::SQLType& type,
            const std::string& name, int index, bool hasMissingValue,
            double missingValue, const odb::BitfieldDef&);

	virtual odb::sql::SQLColumn* createSQLColumn(const odb::sql::type::SQLType& type,
            const std::string& name, int index, bool hasMissingValue,
            double missingValue);

private:
    SQLDataTable(const SQLDataTable&);
    SQLDataTable& operator=(const SQLDataTable&);

    void populateColumns();

    const DataTable& table_;
    double* data_;
};

} // namespace odb

#endif // SQLDATATABLE_H_
