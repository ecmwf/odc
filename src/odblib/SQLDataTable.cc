#include "odblib/SQLDataTable.h"

#include <map>

#include "odblib/DataTable.h"
#include "odblib/SQLDataColumn.h"
#include "odblib/SQLDataTableIterator.h"

using namespace std;
using namespace odb;
using namespace odb::sql;

using namespace eckit;

namespace odb {

SQLDataTable::SQLDataTable(odb::sql::SQLDatabase& db, const DataTable& table)
  : SQLTable(db, PathName("<>"), table.name()),
    table_(table),
    data_(0)
{
    populateColumns();
}

SQLDataTable::~SQLDataTable()
{
    delete [] data_;
}

SQLColumn* SQLDataTable::column(const string& name)
{
    SQLColumn* column = 0;
    map<string, SQLColumn*>::iterator it = columnsByName_.begin();

    for (; it != columnsByName_.end(); ++it)
    {
        string s = it->first;

        if (s.find(name + "@") == 0)
        {
            if (column)
                throw eckit::UserError(string("SQLDataTable::column: name \"") + name + "\" is ambiguous");
            else 
                column = it->second;
        }
    }

    if (column)
        return column;

    return SQLTable::column(name);
}

bool SQLDataTable::hasColumn(const string& name, string* fullName)
{
    if (SQLTable::hasColumn(name))
    {
        if (fullName)
            *fullName = name;

        return true;
    }

    int n = 0;
    map<string,SQLColumn*>::iterator it = columnsByName_.begin();

    for (; it != columnsByName_.end(); ++it)
    {
        if (it->first.find(name + "@") == 0)
        {
            n++;

            if (fullName)
                *fullName = it->first;
        }
    }

    if (n == 0) return false;
    if (n == 1) return true;

    throw eckit::UserError(string("SQLDataTable:hasColumn(\"") + name + "\"): ambiguous name");
    return false;
}

SQLTableIterator* SQLDataTable::iterator(const vector<SQLColumn*>& columns) const
{
    return new SQLDataTableIterator(table_, const_cast<double*>(data_), columns);
}

SQLColumn* SQLDataTable::createSQLColumn(const type::SQLType& type, const string& name,
    int index, bool hasMissingValue, double missingValue, bool isBitfield,
    const BitfieldDef& bitfieldDef)
{
    return new SQLDataColumn(type, *this, name, index, hasMissingValue, missingValue,
        isBitfield, bitfieldDef, &data_[index]);
}

void SQLDataTable::populateColumns()
{
    size_t count = table_.columns().size();

    data_ = new double[count]; ASSERT(data_);

    for (size_t index = 0; index < count; ++index)
    {
        const DataColumn& column = table_.columns()[index];

        const string name = column.name();
        bool hasMissingValue = true; // TODO: implement DataColumn::hasMissing() method
        double missingValue = column.missingValue();
        odb::BitfieldDef bitfieldDef = column.bitfieldDef();
    
        string sqlType;

        switch (column.type())
        {
            case INTEGER: sqlType = "integer"; break;
            case REAL:    sqlType = "real";    break;
            case DOUBLE:  sqlType = "double";  break;
            case STRING:  sqlType = "string";  break;

            case BITFIELD:
            {
                string signature = type::SQLBitfield::make("Bitfield",
                    bitfieldDef.first, bitfieldDef.second, "DummyTypeAlias");
                addColumn(name, index, type::SQLType::lookup(signature),
                    hasMissingValue, missingValue, true, bitfieldDef);
                continue;
            } break;

            default:
                ASSERT(!"Unknown type");
                break;
        }

        SQLColumn *c = new SQLDataColumn(type::SQLType::lookup(sqlType), *this,
            name, index, hasMissingValue, missingValue, column.type() == BITFIELD,
            bitfieldDef, &data_[index]);

        addColumn(c, name, index);
    }
}

} // namespace odb
