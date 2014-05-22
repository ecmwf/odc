/// @file   UnitTest.cc
/// @author Tomas Kral

#include "eckit/exception/Exceptions.h"
#include "odblib/DataColumns.h"
#include "odblib/DataSet.h"
#include "odblib/DataTable.h"
#include "TestCase.h"


using namespace std;
using namespace odb;



struct Fixture2
{
    Fixture2();
    DataSet dataset;
};

Fixture2::Fixture2()
  : dataset("dataset")
{}

TEST_FIXTURE(Fixture2, InsertingTablesIncreasesSize)
{
    DataColumns columns;

    DataTable* parent = new DataTable("parent", columns);
    DataTable* child = new DataTable("child", columns);

    dataset.tables().insert(parent);
    CHECK_EQUAL(1u, dataset.tables().size());

    dataset.tables().insert(child);
    CHECK_EQUAL(2u, dataset.tables().size());
}

