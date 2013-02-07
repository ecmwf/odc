/// @file   TestDataSet.cc
/// @author Tomas Kral

#include "eclib/Exceptions.h"

#include "odblib/TestCase.h"
#include "odblib/ToolFactory.h"
#include "odblib/DataSet.h"
#include "odblib/DataTable.h"
#include "odblib/DataLink.h"

using namespace std;
using namespace odb;

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
