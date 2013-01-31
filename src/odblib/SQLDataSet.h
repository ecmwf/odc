/// @file   SQLDataSet.h
/// @author Tomas Kral

#ifndef SQLDATASET_H_
#define SQLDATASET_H_

#include <iosfwd>

#include "odblib/SQLDatabase.h"

namespace odb {

class SQLDataSet : public odb::sql::SQLDatabase
{
public:
    SQLDataSet() : odb::sql::SQLDatabase("anonymous") {}
    SQLDataSet(const std::string& name) : odb::sql::SQLDatabase(name) {}
    ~SQLDataSet() {}

private:
    SQLDataSet(const SQLDataSet&);
    SQLDataSet& operator=(const SQLDataSet&);

    virtual void open() {}
    virtual void close() { odb::sql::SQLDatabase::close(); }

    virtual odb::sql::SQLTable* openDataHandle(DataHandle&, DataFormat) {}
    virtual odb::sql::SQLTable* openDataStream(std::istream&, DataFormat) {}
};    

} // namespace odb

#endif // SQLDATASET_H_
