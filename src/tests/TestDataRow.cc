/// @file   UnitTest.cc
/// @author Tomas Kral

#include "eckit/exception/Exceptions.h"
#include "odblib/DataColumns.h"
#include "odblib/DataRow.h"
#include "tests/UnitTest.h"


using namespace std;
using namespace odb;

namespace {

TEST(UnInitializedDataRowHasExpectedSize)
{
    DataRow row(5u);
    CHECK_EQUAL(5u, row.size());
}

TEST(UnInitializedDataRowHasExpectedFlags)
{
    DataRow row(5u);
    CHECK(row.standalone());
    CHECK(!row.initialized());
    CHECK(!row.modified());
}

TEST(InitializedDataRowHasExpectedSize)
{
    DataRow row(5, 1.0);
    CHECK_EQUAL(5u, row.size());
}

TEST(InitializedDataRowHasExpectedFlags)
{
    DataRow row(5, 1.0);
    CHECK(row.standalone());
    CHECK(row.initialized());
    CHECK(!row.modified());
}

TEST(InitializedDataRowHasExpectedValues)
{
    DataRow row(5, 1.0);
    for (unsigned i = 0; i < row.size(); i++)
        CHECK_EQUAL(1.0, row[i]);
}

TEST(DataRowInitializedFromColumnsHasExpectedSize)
{
    DataColumns columns;
    columns.add("one", "INTEGER");
    columns.add("two", "INTEGER");

    DataRow row(columns);

    CHECK_EQUAL(columns.size(), row.size());
}

TEST(DataRowInitializedFromColumnsHasExpectedFlags)
{
    DataColumns columns;
    columns.add("one", "INTEGER");
    columns.add("two", "INTEGER");

    DataRow row(columns);

    CHECK(row.standalone());
    CHECK(!row.initialized());
    CHECK(!row.modified());
}

TEST(DataRowInitializedFromColumnsHasExpectedValues)
{
    DataColumns columns;
    columns.add("one", "INTEGER");
    columns.add("two", "INTEGER");

    DataRow row(columns, true);

    for (unsigned i = 0; i < row.size(); i++)
        CHECK_EQUAL(columns[i].defaultValue(), row[i]);
}

struct Ordinals
{
    DataRow ordinals;

    Ordinals() : ordinals(5)
    {
        for (unsigned i = 0; i < ordinals.size(); i++)
            ordinals[i] = i;
    }
};

TEST_FIXTURE(Ordinals, DataRowCopyHasExpectedSize)
{
    DataRow row(ordinals);
    CHECK_EQUAL(ordinals.size(), row.size());
}

TEST_FIXTURE(Ordinals, DataRowCopyHasExpectedFlags)
{
    DataRow row(ordinals);
    CHECK(row.standalone());
    CHECK(row.initialized());
    CHECK(!row.modified());
}

TEST_FIXTURE(Ordinals, DataRowCopyHasExpectedValues)
{
    DataRow row(ordinals);
    for (unsigned i = 0; i < row.size(); i++)
        CHECK_EQUAL(ordinals[i], row[i]);
}

TEST_FIXTURE(Ordinals, CopiedDataRowHasExpectedFlags)
{
    DataRow row(ordinals.size(), 0);
    copy(ordinals.begin(), ordinals.end(), row.begin());

    CHECK(row.standalone());
    CHECK(row.initialized());
    CHECK(row.modified());
}

TEST_FIXTURE(Ordinals, CopiedDataRowHasExpectedValues)
{
    DataRow row(ordinals.size(), 0);
    copy(ordinals.begin(), ordinals.end(), row.begin());

    for (unsigned i = 0; i < row.size(); i++)
        CHECK_EQUAL(ordinals[i], row[i]);
}

TEST(DataRowCanSetIntegers)
{
    DataRow row(1);
    row.DataRow::set<int>(0, static_cast<int>(3.14) ); // this conversion to int is intentional
    CHECK(row.modified());
    CHECK_EQUAL(3, row[0]);
}

TEST(DataRowCanSetDoubles)
{
    DataRow row(1);
    row.DataRow::set<double>(0, 3.14);
    CHECK(row.modified());
    CHECK_EQUAL(3.14, row[0]);
}

TEST(DataRowCanSetStrings)
{
    DataRow row(1);
    row.DataRow::set<string>(0, "ABCD");
    CHECK(row.modified());
    CHECK_ARRAY_EQUAL("ABCD    ", reinterpret_cast<char*>(&row[0]), sizeof(double));
}

} // namespace


