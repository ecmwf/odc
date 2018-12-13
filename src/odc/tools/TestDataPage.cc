/// @file   TestDataPage.cc
/// @author Tomas Kral

#include <string>
#include <iostream>

#include "odc/tools/TestCase.h"
#include "odc/tools/ToolFactory.h"
#include "odc/DataTable.h"
#include "odc/DataPage.h"

using namespace std;
using namespace odc;
using namespace odc::internal;

namespace {

struct EmptyPage
{
    static DataColumns columns();
    static DataTableProperties properties();
    static size_t capacity() { return 10; }

    EmptyPage()
      : table("table", columns(), properties()),
        page(table) {}

    EmptyPage(const DataTableProperties& properties)
      : table("table", columns(), properties),
        page(table) {}

    DataTable table;
    DataPage page;
};

DataColumns EmptyPage::columns()
{
    DataColumns columns;
    columns.add("ordinal", "INTEGER");
    return columns;
}

DataTableProperties EmptyPage::properties()
{
    DataTableProperties properties;
    properties.blockSizeInNumberOfRows(10);
    return properties;
}

struct HalfEmptyPage : public EmptyPage
{
    HalfEmptyPage();
};

HalfEmptyPage::HalfEmptyPage()
  : EmptyPage()
{
    DataRow row(1);

    for (size_t i = 0; i < capacity() / 2; i++)
    {
        row[0] = i;
        page.push_back(row);
    }
}

struct FullPage : public EmptyPage
{
    FullPage();
};

FullPage::FullPage()
  : EmptyPage()
{
    DataRow row(1);

    for (size_t i = 0; i < capacity(); i++)
    {
        row[0] = i;
        page.push_back(row);
    }
}

TEST_FIXTURE(EmptyPage, HasTheCorrectCapacity)
{
    CHECK_EQUAL(capacity(), page.capacity());
}

TEST_FIXTURE(EmptyPage, EmptyPageIsEmpty)
{
    CHECK(page.empty());
}

TEST_FIXTURE(FullPage, FullPageIsNotEmpty)
{
    CHECK(!page.empty());
}

TEST_FIXTURE(EmptyPage, EmptyPageIsNotFull)
{
    CHECK(!page.full());
}

TEST_FIXTURE(FullPage, FullPageIsFull)
{
    CHECK(page.full());
}

TEST_FIXTURE(EmptyPage, EmptyPageHasZeroSize)
{
    CHECK_EQUAL(0u, page.size());
}

TEST_FIXTURE(FullPage, FullPageHasSizeOfItsCapacity)
{
    CHECK_EQUAL(page.capacity(), page.size());
}

TEST_FIXTURE(EmptyPage, EmptyPageCanBeCleared)
{
    page.clear();
    CHECK_EQUAL(0u, page.size());
}

TEST_FIXTURE(FullPage, FullPageCanBeCleared)
{
    page.clear();
    CHECK_EQUAL(0u, page.size());
}

TEST_FIXTURE(EmptyPage, IteratorReturnsExpectedNumberOfRows)
{
    const unsigned expected = 0;
    unsigned actual = 0;

    for (DataRowProxy* row = page.begin();
            row != page.end(); ++row, ++actual);

    CHECK_EQUAL(expected, actual);
}

TEST_FIXTURE(FullPage, IteratorReturnsExpectedNumberOfRows)
{
    const unsigned expected = page.size();
    unsigned actual = 0;

    for (DataRowProxy* row = page.begin();
            row != page.end(); ++row, ++actual);

    CHECK_EQUAL(expected, actual);
}

TEST_FIXTURE(FullPage, IteratorReturnsExpectedRows)
{
    int ordinal = 0;

    for (DataRowProxy* row = page.begin();
            row != page.end(); ++row, ++ordinal)
    {
        DataRow& r = static_cast<DataRow&>(*row);
        CHECK_EQUAL(ordinal, r.get<int>(0));
    }
}

TEST_FIXTURE(EmptyPage, PushingBackRowsToAnEmptyPage)
{
    DataRow row(1);

    for (unsigned i = 0; i < capacity(); i++)
    {
        row[0] = i;
        CHECK_EQUAL(true, page.push_back(row));
    }

    for (unsigned i = 0; i < capacity(); i++)
    {
        DataRow& row = page[i];
        CHECK_EQUAL(i, row.get<unsigned>(0));
        CHECK_EQUAL(false, row.modified());
    }
}

TEST_FIXTURE(FullPage, PushingBackRowsToTheFullPage)
{
    DataRow row(1);

    CHECK_EQUAL(false, page.push_back(row));
    CHECK_EQUAL(capacity(), page.size());
}

TEST_FIXTURE(EmptyPage, DecreadingSizeOfEmptyPageToZero)
{
    const unsigned n = 0;

    CHECK_EQUAL(n, page.resize(n));
    CHECK_EQUAL(n, page.size());
}

TEST_FIXTURE(FullPage, DecreadingSizeOfFullPageByHalf)
{
    const unsigned n = page.size() / 2;

    CHECK_EQUAL(n, page.resize(n));
    CHECK_EQUAL(n, page.size());
}

TEST_FIXTURE(FullPage, DecreadingSizeOfFullPageToZero)
{
    const unsigned n = 0;

    CHECK_EQUAL(n, page.resize(n));
    CHECK_EQUAL(n, page.size());
}

TEST_FIXTURE(EmptyPage, IncreadingSizeOfEmptyPageToHalfTheCapacity)
{
    const unsigned n = page.capacity() / 2;

    CHECK_EQUAL(n, page.resize(n));
    CHECK_EQUAL(n, page.size());

    for (unsigned i = 0; i < page.size(); i++)
    {
        const DataRow& row = page[i];
        CHECK(!row.initialized());
        CHECK(row.used());
    }
}

TEST_FIXTURE(FullPage, IncreadingSizeOfFullPageBeyondItsCapacity)
{
    const unsigned n = page.capacity() + 1;
    const unsigned size = page.size();

    CHECK_EQUAL(size, page.resize(n));
    CHECK_EQUAL(size, page.size());
}

TEST_FIXTURE(FullPage, DecreasingSizeOfFullPageToHalfTheCapacity)
{
    const unsigned n = page.capacity() / 2;

    CHECK_EQUAL(n, page.resize(n));
    CHECK_EQUAL(n, page.size());

    for (unsigned i = 0; i < page.size(); i++)
    {
        const DataRow& row = page[i];
        CHECK_EQUAL(false, row.initialized());
        CHECK_EQUAL(true, row.used());
    }
}

struct EmptyPageWithFillMark : public EmptyPage
{
    static DataTableProperties properties();
    EmptyPageWithFillMark() : EmptyPage(properties()) {}
};

DataTableProperties EmptyPageWithFillMark::properties()
{
    DataTableProperties properties = EmptyPage::properties();
    size_t n = properties.blockSizeInNumberOfRows();
    properties.blockFillMarkInNumberOfRows(n / 2);
    return properties;
}

TEST_FIXTURE(EmptyPageWithFillMark, ResizeEmptyPageWithFillMark)
{
    size_t n = page.capacity();
    size_t m = table.properties().blockFillMarkInNumberOfRows();

    CHECK_EQUAL(m, page.resize(n));
    CHECK_EQUAL(m, page.size());

    for (size_t i = 0; i < page.size(); i++)
    {
        DataRow& row = page[i];
        CHECK(!row.initialized());
        CHECK(row.used());
    }
}

TEST_FIXTURE(EmptyPageWithFillMark, PushBackRowsToEmptyPageWithFillMark)
{
    size_t n = page.capacity();
    size_t m = table.properties().blockFillMarkInNumberOfRows();

    DataRow row(1);

    for (size_t i = 0; i < m; i++)
    {
        row.set(0, i);

        CHECK(page.push_back(row));
        CHECK(!page.back().initialized());
        CHECK(page.back().used());
        CHECK_EQUAL(double(i), page.back()[0]);
    }

    for (size_t i = m; i < n; i++)
    {
        row.set(0, i);
        CHECK(!page.push_back(row));
        CHECK_EQUAL(double(m-1), page.back()[0]);
    }
}

TEST_FIXTURE(EmptyPage, CanIncreasePageSizeAndInitializeValues)
{
    size_t n = page.capacity() / 2;

    CHECK_EQUAL(n, page.resize(n, true));
    CHECK_EQUAL(n, page.size());

    for (size_t i = 0; i < page.size(); i++)
    {
        DataRow& row = page[i];
        CHECK_EQUAL(2147483647, row.get<int>(0));
        CHECK(row.initialized());
        CHECK(row.used());
    }
}

TEST_FIXTURE(EmptyPage, CannotIncreasePageSizeBeyondItsCapacity)
{
    size_t c = page.capacity();
    size_t n = c + 1;

    CHECK_EQUAL(c, page.resize(n));
    CHECK_EQUAL(c, page.size());
}

TEST_FIXTURE(FullPage, CanDecreasePageSize)
{
    size_t n = page.capacity() / 2;

    CHECK_EQUAL(n, page.resize(n));
    CHECK_EQUAL(n, page.size());

    for (size_t i = 0; i < page.size(); i++)
    {
        DataRow& row = page[i];
        CHECK_EQUAL(i, row.get<size_t>(0));
    }
}

TEST_FIXTURE(HalfEmptyPage, InsertRowAtTheBeginning)
{
    DataRow row(1);
    row[0] = -1;
    size_t n = page.size();

    CHECK(page.insert(page.begin(), row));
    CHECK_EQUAL(n + 1, page.size());

    for (size_t i = 0; i < page.size(); i++)
    {
        DataRow& row = page[i];
        CHECK_EQUAL(int(i) - 1, row.get<int>(0));
    }
}

TEST_FIXTURE(HalfEmptyPage, InsertRowAtTheEnd)
{
    DataRow row(1);
    row[0] = page.size();
    size_t n = page.size();

    CHECK_EQUAL(true, page.insert(page.end(), row));
    CHECK_EQUAL(n + 1, page.size());

    for (size_t i = 0; i < page.size(); i++)
    {
        DataRow& row = page[i];
        CHECK_EQUAL(i, row.get<size_t>(0));
    }
}

TEST_FIXTURE(FullPage, CanNotInsertRowToAFullPage)
{
    DataRow row(1);
    CHECK_EQUAL(false, page.insert(page.begin(), row));
}

TEST_FIXTURE(FullPage, SplitFullPage)
{
    size_t newSize = page.size() / 2;
    DataPage* newPage = page.split();

    CHECK_EQUAL(newSize, page.size());
    CHECK_EQUAL(newSize, newPage->size());

    for (size_t i = 0; i < page.size(); i++)
    {
        DataRow& row = page[i];
        CHECK_EQUAL(i, row.get<size_t>(0));
    }

    for (size_t i = 0; i < newPage->size(); i++)
    {
        DataRow& row = (*newPage)[i];
        CHECK_EQUAL(i + newSize, row.get<size_t>(0));
    }

    delete newPage;
}

} // namespace
