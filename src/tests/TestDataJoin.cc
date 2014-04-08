/// @file   UnitTest.cc
/// @author Tomas Kral

#include "odblib/DataJoin.h"
#include "odblib/DataTable.h"
#include "eckit/testing/UnitTest.h"


using namespace std;
using namespace eckit;
using namespace odb;

struct Tables
{
    Tables()
      : left("left", columns("left")),
        right("right", columns("right"))
    {}

    static DataColumns columns(const std::string& name);

    DataTable left;
    DataTable right;
};

DataColumns Tables::columns(const std::string& name)
{
    DataColumns columns;

    if (name == "left")
        columns.add("id@left", "INTEGER");
    else if (name == "right")
        columns.add("id@right", "INTEGER");
    else if (name == "result")
    {
        columns.add("id@left", "INTEGER");
        columns.add("id@right", "INTEGER");
    }

    return columns;
}

struct LinkedTables : public Tables
{
    DataJoin join;
    DataTable result;

    LinkedTables()
      : Tables(),
        join(left, right, "id@left", "id@right"),
        result("expected", columns("result"))
    {
        left  << 1, 2, 3, 4, 5;

        right << 1,
                 2, 2,
                 3, 3, 3,
                 4, 4, 4, 4,
                 5, 5, 5, 5, 5;

        result << 1, 1,
                  2, 2,
                  2, 2,
                  3, 3, 3,
                  3, 3, 3,
                  4, 4, 4, 4,
                  4, 4, 4, 4,
                  5, 5, 5, 5, 5,
                  5, 5, 5, 5, 5;
    }
};

TEST_FIXTURE(LinkedTables, DataJoinHasExpectedNumberOfColumns)
{
    unsigned expected = left.columns().size() + right.columns().size();
    unsigned actual = join.columns().size();

    CHECK_EQUAL(expected, actual);
}

TEST_FIXTURE(LinkedTables, DataJoinHasExpectedColumns)
{
    CHECK((left.columns() + right.columns()) == join.columns());
}

TEST_FIXTURE(LinkedTables, DataJoinReturnsExpectedNumberOfResults)
{
    unsigned count = 0;

    for (DataJoin::iterator it = join.begin();
            it != join.end(); ++it, ++count);

    CHECK_EQUAL(right.size(), count);
}

TEST_FIXTURE(LinkedTables, DataJoinReturnsExpectedValues)
{
    DataJoin::iterator actual = join.begin();
    DataTable::const_iterator expected = result.begin();

    for (const unsigned size = join.columns().size();
            actual != join.end(); ++actual, ++expected)
    {
        CHECK_ARRAY_EQUAL(expected->data(), actual->data(), size);
    }
}

TEST_FIXTURE(LinkedTables, DataJoinCanBeUsedWithStlAlgorithms)
{
    DataTable copied("copied", join.columns());

    copy(join.begin(), join.end(), back_inserter(copied));

    DataTable::const_iterator actual = copied.begin();
    DataTable::const_iterator expected = result.begin();

    for (const unsigned size = copied.columns().size();
            actual != copied.end(); ++actual, ++expected)
    {
        CHECK_ARRAY_EQUAL(expected->data(), actual->data(), size);
    }
}

struct RelatedTables : public Tables
{
    RelatedTables() : Tables()
    {
        left << 0, 1, 2, 3, 4;
        right << 1, 2, 2, 3, 3, 3, 5;
    }
};

struct InnerJoin : public RelatedTables
{
    DataJoin join;
    DataTable expected;

    InnerJoin()
      : RelatedTables(),
        join(left, right, "id@left", "id@right", DataJoin::INNER),
        expected("result", join.columns())
    {
        expected << 1, 1,
                    2, 2,
                    2, 2,
                    3, 3,
                    3, 3,
                    3, 3;
    }

};

TEST_FIXTURE(InnerJoin, InnerJoinHasExpectedNumberOfColumns)
{
    unsigned expected = left.columns().size() + right.columns().size();
    unsigned actual = join.columns().size();

    CHECK_EQUAL(expected, actual);
}

TEST_FIXTURE(InnerJoin, InnerJoinHasExpectedColumns)
{
    CHECK((left.columns() + right.columns()) == join.columns());
}

TEST_FIXTURE(InnerJoin, InnerJoinReturnsExpectedNumberOfResults)
{
    unsigned count = 0;

    for (DataJoin::iterator it = join.begin();
            it != join.end(); ++it, ++count);

    CHECK_EQUAL(expected.size(), count);
}

TEST_FIXTURE(InnerJoin, InnerJoinReturnsExpectedValues)
{
    DataTable::const_iterator expect = expected.begin();
    DataJoin::iterator actual = join.begin();

    for (const unsigned size = expected.columns().size();
            actual != join.end(); ++expect, ++actual)
    {
        CHECK_ARRAY_EQUAL(expect->data(), actual->data(), size);
    }
}


RUN_ALL_TESTS
