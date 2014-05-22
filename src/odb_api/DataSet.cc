/// @file   DataSet.cc
/// @author Tomas Kral

#include "eckit/eckit.h"
#include "odb_api/DataLink.h"
#include "odb_api/DataSet.h"
#include "odb_api/DataTable.h"

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
