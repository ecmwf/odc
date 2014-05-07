/// @file   UnitTest.cc
/// @author Tomas Kral

#include "eckit/exception/Exceptions.h"
#include "odb_api/odblib/DataColumns.h"
#include "odb_api/odblib/DataSet.h"
#include "odb_api/odblib/DataTable.h"
#include "eckit/testing/UnitTest.h"


using namespace std;
using namespace odb;



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

RUN_ALL_TESTS
