/// @file   DataSet.cc
/// @author Tomas Kral

//#include <algorithm>
//#include <functional>

//#include "odblib/DataSet.h"
//#include "odblib/DataTable.h"
//#include "odblib/DataLink.h"
//#include "odblib/DataTables.h"

using namespace std;

namespace odb {

DataSet::DataSet(const std::string& name)
  : name_(name),
    tables_(*this),
    links_(*this)
{}

DataSet::~DataSet()
{
    for (DataTables::iterator it = tables_.begin();
            it != tables_.end(); ++it)
        delete (*it);
}

void DataSet::clear()
{
    for_each(tables_.begin(), tables_.end(), mem_fun(&DataTable::clear));
    for_each(links_.begin(), links_.end(), mem_fun(&DataLink::clear));
}

} // namespace odb
