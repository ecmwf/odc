/// @file   TestDataSelect.cc
/// @author Tomas Kral

#include <string>

#include "odblib/TestCase.h"
#include "odblib/ToolFactory.h"
#include "odblib/DataSet.h"
#include "odblib/DataTable.h"
#include "odblib/DataSelect.h"
#include "odblib/DataSelectIterator.h"

using namespace std;
using namespace odb;

namespace {

struct FilledDataSet
{
    DataSet dataset;
    DataTable* table;
    FilledDataSet();
};

FilledDataSet::FilledDataSet()
  : dataset("TestDataSelect")
{
    DataColumns columns;
    columns.add("digit", "INTEGER");
    columns.add("name", "STRING");
    columns.add("flag", "BITFIELD");

    table = new DataTable("ordinals", columns);

    *table << 0, "zero",  1 << 0,
              1, "one",   1 << 1,
              2, "two",   1 << 2,
              3, "three", 1 << 3,
              4, "four",  1 << 4,
              5, "five",  1 << 5;

    dataset.tables().insert(table);
}

struct SelectAll : public FilledDataSet
{
    DataSelect select;

    SelectAll()
     : FilledDataSet(),
       select("SELECT * FROM ordinals;", dataset)
    {}
};

struct SelectWhere : public FilledDataSet
{
    DataSelect select;

    SelectWhere()
     : FilledDataSet(),
       select("SELECT digit, flag FROM ordinals WHERE digit IN (0, 2, 4);", dataset)
    {}
};

struct SelectAggregate : public FilledDataSet
{
    DataSelect select;

    SelectAggregate()
     : FilledDataSet(),
       select("SELECT SUM(digit) FROM ordinals;", dataset)
    {}
};

struct SelectFirst : public FilledDataSet
{
    DataSelect select;

    SelectFirst()
     : FilledDataSet(),
       select("SELECT digit FROM ordinals;", dataset)
    {}
};

TEST_FIXTURE(SelectAll, ReturnsExpectedNumberOfColumns)
{
    CHECK_EQUAL(table->columns().size(), select.columns().size());
}


TEST_FIXTURE(SelectWhere, ReturnsExpectedNumberOfColumns)
{
    CHECK_EQUAL(2u, select.columns().size());
}

TEST_FIXTURE(SelectAggregate, ReturnsExpectedNumberOfColumns)
{
    CHECK_EQUAL(1u, select.columns().size());
}


TEST_FIXTURE(SelectAll, ReturnsExpectedColumns)
{
    const DataColumns& expected = table->columns();
    const DataColumns& actual = select.columns();

    for (unsigned i = 0; i < actual.size(); i++)
        CHECK(expected[i] == actual[i]);
}


TEST_FIXTURE(SelectWhere, ReturnsExpectedColumns)
{
    const DataColumns& expected = table->columns();
    const DataColumns& actual = select.columns();

    CHECK(expected["digit"] == actual[0]);
    CHECK(expected["flag"] == actual[1]);
}


TEST_FIXTURE(SelectAggregate, ReturnsExpectedColumns)
{
    const DataColumn& column = select.columns()[0];
    CHECK_EQUAL("sum(digit)", column.name());
}


TEST_FIXTURE(SelectAll, ReturnsExpectedNumberOfResults)
{
    unsigned count = 0;

    for (DataSelect::iterator it = select.begin();
            it != select.end(); ++it, ++count);

    CHECK_EQUAL(table->size(), count);
}

TEST_FIXTURE(SelectWhere, ReturnsExpectedNumberOfResults)
{
    unsigned count = 0;

    for (DataSelect::iterator it = select.begin();
            it != select.end(); ++it, ++count);

    CHECK_EQUAL(3u, count);
}


TEST_FIXTURE(SelectAggregate, ReturnsExpectedNumberOfResults)
{
    unsigned count = 0;

    for (DataSelect::iterator it = select.begin();
            it != select.end(); ++it, ++count);

    CHECK_EQUAL(1u, count);
}


TEST_FIXTURE(SelectAll, ReturnsExpectedResults)
{
    DataSelect::iterator actual = select.begin();
    DataTable::iterator expected = table->begin();

    for (; actual != select.end(); ++actual, ++expected)
        for (unsigned i = 0; i < select.columns().size(); i++)
            CHECK_EQUAL((*expected)[i], (*actual)[i]);
}

TEST_FIXTURE(SelectWhere, ReturnsExpectedResults)
{
    DataSelect::iterator actual = select.begin();
    DataTable::iterator expected = table->begin();

    for (; actual != select.end(); ++actual, ++expected)
    {
        if (expected->get<int>(0) % 2) ++expected;

        CHECK_EQUAL((*expected)[0], (*actual)[0]);
        CHECK_EQUAL((*expected)[2], (*actual)[1]);
    }
}

TEST_FIXTURE(SelectAggregate, ReturnsExpectedResults)
{
    DataSelect::iterator actual = select.begin();
    CHECK_EQUAL(15, (*actual)[0]);
}

TEST_FIXTURE(SelectAll, CanUseCopyToAppendResults)
{
    DataTable results("results", select.columns());

    copy(select.begin(), select.end(), back_inserter(results));

    DataTable::iterator expected = table->begin();
    DataTable::iterator actual = results.begin();

    for (; actual != results.end(); ++actual, ++expected)
        for (unsigned i = 0; i < select.columns().size(); i++)
            CHECK_EQUAL((*expected)[i], (*actual)[i]);
}

} // namespace
