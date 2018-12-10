/// @file   TestDataSet.cc
/// @author Tomas Kral

#include "eckit/exception/Exceptions.h"

#include "odb_api/tools/TestCase.h"
#include "odb_api/tools/ToolFactory.h"
#include "odb_api/DataSet.h"
#include "odb_api/DataTable.h"
#include "odb_api/DataLink.h"

using namespace std;
using namespace odc;

namespace {

struct Fixture
{
    Fixture();
    DataSet dataset;
};

Fixture::Fixture()
  : dataset("dataset")
{}

TEST_FIXTURE(Fixture, InsertingTablesIncreasesSize)
{
    DataColumns columns;

    DataTable* parent = new DataTable("parent", columns);
    DataTable* child = new DataTable("child", columns);

    dataset.tables().insert(parent);
    CHECK_EQUAL(1u, dataset.tables().size());

    dataset.tables().insert(child);
    CHECK_EQUAL(2u, dataset.tables().size());
}

} // namespace
