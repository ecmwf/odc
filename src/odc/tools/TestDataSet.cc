/// @file   TestDataSet.cc
/// @author Tomas Kral

#include "eckit/exception/Exceptions.h"

#include "odc/tools/TestCase.h"
#include "odc/tools/ToolFactory.h"
#include "odc/DataSet.h"
#include "odc/DataTable.h"
#include "odc/DataLink.h"

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
