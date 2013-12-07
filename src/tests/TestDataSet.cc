/// @file   UnitTest.cc
/// @author Tomas Kral

#include "eckit/exception/Exceptions.h"
#include "odblib/DataColumns.h"
#include "odblib/DataSet.h"
#include "odblib/DataTable.h"
#include "tests/UnitTest.h"


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





static void test(){}
static void setUp(){}
static void tearDown(){}

TEST_MAIN;
