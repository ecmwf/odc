/// @file SQLDataSet.cc
/// @author Tomas Kral

#include "odblib/SQLDataSet.h"

using namespace std;
using namespace eclib;

namespace odb {
namespace sql {

SQLDataSet::SQLDataSet()
  : SQLDatabase("anonymous")
{}

SQLDataSet::SQLDataSet(const string& name)
  : SQLDatabase(name)
{}

SQLDataSet::~SQLDataSet()
{}

void SQLDataSet::open()
{}

void SQLDataSet::close()
{
    SQLDatabase::close();
}

SQLTable* SQLDataSet::openDataHandle(DataHandle&, DataFormat)
{
    NOTIMP;
    return 0;
}

SQLTable* SQLDataSet::openDataStream(istream&, DataFormat)
{
    NOTIMP;
    return 0;
}

} // namespace sql
} // namespace odb
