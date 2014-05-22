/// @file   UnitTest.cc
/// @author Tomas Kral

#include "odb_api/odblib/DataTable.h"
#include "TestCase.h"


using namespace std;
using namespace odb;



struct EmptyTable
{
    static DataColumns columns();
    static DataTableProperties properties();
    static size_t capacity() { return 10; }
    EmptyTable() : table("table", columns(), properties()) {}
    DataTable table;
};

DataColumns EmptyTable::columns()
{
    DataColumns columns;
    columns.add("column", "INTEGER");
    return columns;
}

DataTableProperties EmptyTable::properties()
{
    DataTableProperties properties;
    properties.blockSizeInNumberOfRows(10);
    return properties;
}

struct FilledTable : public EmptyTable
{
    static size_t size() { return 1.5 * capacity(); }
    FilledTable();
};

FilledTable::FilledTable()
  : EmptyTable()
{
    table.resize(size());

    for (size_t i = 0; i < table.size(); i++)
    {
        DataRow& row = table[i];
        row.set(0, i);
    }
}

TEST_FIXTURE(EmptyTable, HasZeroSize)
{
    CHECK_EQUAL(0u, table.size());
}

TEST_FIXTURE(FilledTable, HasExpectedNonZeroSize)
{
    CHECK_EQUAL(FilledTable::size(), table.size());
}

TEST_FIXTURE(EmptyTable, IsEmpty)
{
    CHECK(table.empty());
}

TEST_FIXTURE(FilledTable, IsNotEmpty)
{
    CHECK(!table.empty());
}

TEST_FIXTURE(EmptyTable, HasExpectedCapacity)
{
    CHECK_EQUAL(capacity(), table.capacity());
}

TEST_FIXTURE(FilledTable, HasExpectedCapacity)
{
    CHECK_EQUAL(2 * capacity(), table.capacity());
}

TEST_FIXTURE(EmptyTable, CapacityCanBeIncreased)
{
    size_t n = 2 * capacity();
    table.reserve(n);
    CHECK_EQUAL(n, table.capacity());
}

TEST_FIXTURE(FilledTable, CapacityCanBeIncreased)
{
    size_t n = 3 * capacity();
    table.reserve(n);
    CHECK_EQUAL(n, table.capacity());
}


TEST_FIXTURE(EmptyTable, SizeCanBeIncreased)
{
    size_t n = 2 * capacity();
    table.resize(n, true);
    CHECK_EQUAL(n, table.size());

    for (size_t i = 0; i < table.size(); i++)
    {
        DataRow& row = table[i];
        CHECK_EQUAL(2147483647, row.get<int>(0));
    }
}

TEST_FIXTURE(FilledTable, SizeCanBeDecreased)
{
    size_t n = capacity() / 2;
    table.resize(n);
    CHECK_EQUAL(n, table.size());

    for (size_t i = 0; i < table.size(); i++)
    {
        DataRow& row = table[i];
        CHECK_EQUAL(i, row.get<size_t>(0));
    }
}

TEST_FIXTURE(FilledTable, SizeCanBeIncreased)
{
    size_t n = 3 * capacity();
    table.resize(n, true);
    CHECK_EQUAL(n, table.size());

    size_t i = 0;
    for (; i < FilledTable::size(); i++)
    {
        CHECK_EQUAL(i, table[i].get<size_t>(0));
    }

    for (; i < table.size(); i++)
    {
        CHECK_EQUAL(2147483647, table[i].get<int>(0));
    }
}

TEST_FIXTURE(EmptyTable, CanBeCleared)
{
    table.clear();
    CHECK_EQUAL(0u, table.size());
}

TEST_FIXTURE(FilledTable, CanBeCleared)
{
    table.clear();
    CHECK_EQUAL(0u, table.size());
}

TEST_FIXTURE(FilledTable, CanIterateForeward)
{
    size_t count = 0;

    for (DataTable::const_iterator it = table.begin();
            it != table.end(); ++it, ++count)
    {
        CHECK_EQUAL(count, it->get<size_t>(0));
    }
    
    CHECK_EQUAL(table.size(), count);
}

TEST_FIXTURE(FilledTable, CanIterateBackward)
{
    size_t count = table.size() - 1;

    for (DataTable::const_iterator it = table.end() - 1;
            it != table.begin(); --it, --count)
    {
        CHECK_EQUAL(count, it->get<size_t>(0));
    }
    
    CHECK_EQUAL(0u, count);
}

TEST_FIXTURE(FilledTable, AdvanceIteratorByZero)
{
    size_t n = 0;
    DataTable::iterator it = table.begin() + n;
    CHECK_EQUAL(n, it->get<size_t>(0));
}

TEST_FIXTURE(FilledTable, AdvanceIteratorForewardByOne)
{
    size_t n = 1;
    DataTable::iterator it = table.begin() + n;
    CHECK_EQUAL(n, it->get<size_t>(0));
}

TEST_FIXTURE(FilledTable, AdvanceIteratorByHalf)
{
    size_t n = table.size() / 2;
    DataTable::iterator it = table.begin() + n;
    CHECK_EQUAL(n, it->get<size_t>(0));
}

TEST_FIXTURE(FilledTable, AdvanceIteratorToTheEnd)
{
    size_t n = table.size();
    DataTable::iterator it = table.begin() + n;
    CHECK(it == table.end());
}

TEST_FIXTURE(FilledTable, AdvanceIteratorBackward)
{
    size_t n = table.size();
    DataTable::iterator it = table.end() - n;
    CHECK_EQUAL(0u, it->get<size_t>(0));
}

TEST_FIXTURE(FilledTable, DistanceBetweenTheSameIteratorsIsZero)
{
    CHECK_EQUAL(0, table.begin() - table.begin());
}

TEST_FIXTURE(FilledTable, DistanceBetweenBeginAndEndIteratorIsAsExpected)
{
    CHECK_EQUAL((ptrdiff_t)table.size(), table.end() - table.begin());
    CHECK_EQUAL(-(ptrdiff_t)table.size(), table.begin() - table.end());
}

TEST_FIXTURE(FilledTable, InsertRowAtTheBeginning)
{
    size_t n = table.size();
    DataRow row(1, -1);
    DataTable::iterator begin = table.begin();
    table.insert(begin, row);

    CHECK_EQUAL(n + 1, table.size());

    int ordinal = -1;
    for (DataTable::const_iterator it = table.begin();
            it != table.end(); ++it, ++ordinal)
    {
        CHECK_EQUAL(ordinal, it->get<int>(0));
    }
}

TEST_FIXTURE(FilledTable, InsertRowAtTheEnd)
{
    size_t n = table.size();
    DataRow row(1, n);
    DataTable::iterator end = table.end();
    table.insert(end, row);

    CHECK_EQUAL(n + 1, table.size());

    size_t ordinal = 0;
    for (DataTable::const_iterator it = table.begin();
            it != table.end(); ++it, ++ordinal)
    {
        CHECK_EQUAL(ordinal, it->get<size_t>(0));
    }
}

TEST_FIXTURE(FilledTable, GetRowAtTheBack)
{
    int expected = table.size() - 1;
    int actual = table.back().get<int>(0);
    CHECK_EQUAL(expected, actual);
}

struct ThreeColumnTable
{
    static DataColumns columns();
    static size_t capacity() { return 10; }
    ThreeColumnTable() : table("table", columns()) {}
    DataTable table;
};

DataColumns ThreeColumnTable::columns()
{
    DataColumns columns;
    columns.add("ordinal", "INTEGER");
    columns.add("value", "DOUBLE");
    columns.add("name", "STRING");
    return columns;
}

TEST_FIXTURE(ThreeColumnTable, LineByLineInitialization)
{
    table << 1, 1.0, string("one");
    table << 2, 2.0, string("two");

    CHECK_EQUAL(2u, table.size());
    CHECK_EQUAL(1.0, table[0].get<double>(1));
    CHECK_EQUAL(2.0, table[1].get<double>(1));
}

TEST_FIXTURE(ThreeColumnTable, MultiLineInitialization)
{
    table << 1, 1.0, string("one"),
             2, 2.0, string("two");

    CHECK_EQUAL(2u, table.size());
    CHECK_EQUAL(1.0, table[0].get<double>(1));
    CHECK_EQUAL(2.0, table[1].get<double>(1));
}


