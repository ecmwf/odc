/// @file   TestDataLink.cc
/// @author Tomas Kral

#include <string>
#include <vector>

#include "odc/tools/TestCase.h"
#include "odc/tools/ToolFactory.h"
#include "odc/DataTable.h"
#include "odc/DataLink.h"

using namespace std;
using namespace odc;

namespace {

struct FilledTables
{
    FilledTables();
   ~FilledTables();

    static DataColumns columns(const string& name);
    static DataTableProperties properties();
    
    DataTable parent;
    DataTable child;
    vector<DataTable*> result;
};

FilledTables::FilledTables()
  : parent("parent", columns("parent"), properties()),
    child("child", columns("child"), properties()),
    result(2)
{
    parent << 0,
              1;

    child << 0, 0.0,
             0, 1.0,
             0, 2.0,
             0, 3.0,
             0, 4.0,
             1, 5.0,
             1, 6.0,
             1, 7.0,
             1, 8.0,
             1, 9.0;

    for (unsigned i = 0; i < result.size(); ++i)
        result[i] = new DataTable("result", columns("child"));

    (*result[0]) << 0, 0.0,
                    0, 1.0,
                    0, 2.0,
                    0, 3.0,
                    0, 4.0;

    (*result[1]) << 1, 5.0,
                    1, 6.0,
                    1, 7.0,
                    1, 8.0,
                    1, 9.0;
}

FilledTables::~FilledTables()
{
    for (unsigned i = 0; i < result.size(); ++i)
        delete result[i];
}

DataColumns FilledTables::columns(const string& name)
{
    DataColumns columns;

    if (name == "parent")
    {
        columns.add("key", "INTEGER");
    }
    else if (name == "child")
    {
        columns.add("key", "INTEGER");
        columns.add("ordinal", "DOUBLE");
    }
    
    return columns;
}

DataTableProperties FilledTables::properties()
{
    DataTableProperties properties;
    properties.blockSizeInNumberOfRows(5);
    return properties;
}

struct FilledLink : public FilledTables
{
    FilledLink()
      : FilledTables(),
        link(parent, child, "key", "key") {}

    DataLink link;
};

TEST_FIXTURE(FilledLink, DataLinkReturnsExpectedParentTable)
{
    CHECK_EQUAL(&parent, &link.parent());
}

TEST_FIXTURE(FilledLink, DataLinkReturnsExpectedChildTable)
{
    CHECK_EQUAL(&child, &link.child());
}

TEST_FIXTURE(FilledLink, DataLinkHasExpectedSize)
{
    CHECK_EQUAL(parent.size(), link.size());
}

TEST_FIXTURE(FilledLink, DataLinkIteratorReturnsExpectedNumberOfParentRows)
{
    unsigned count = 0;

    for (DataLink::const_iterator range = link.begin();
            range != link.end(); ++range, ++count);

    CHECK_EQUAL(parent.size(), count);
}

TEST_FIXTURE(FilledLink, DataLinkIteratorReturnsExpectedNumberOfChildRows)
{
    unsigned i = 0;

    for (DataLink::const_iterator range = link.begin(),
            end = link.end(); range != end; ++range, ++i)
    {
        const unsigned expected = result[i]->size();
        unsigned actual = 0;

        for (DataTable::const_iterator it = range->begin();
                it != range->end(); ++it, ++actual);

        CHECK_EQUAL(expected, actual);
    }
}

TEST_FIXTURE(FilledLink, DataLinkIteratorReturnsExpectedRows)
{
    unsigned i = 0;

    for (DataLink::const_iterator range = link.begin(),
            end = link.end(); range != end; ++range, ++i)
    {
        const unsigned size = child.columns().size();

        for (DataTable::const_iterator expected = result[i]->begin(),
                actual = range->begin(); actual != range->end();
                ++expected, ++actual)
        {
            CHECK_ARRAY_EQUAL(expected->data(), actual->data(), size);
        }
    }
}

TEST_FIXTURE(FilledLink, InsertRowAtTheBeginningOfTheFirstRange)
{
    DataRow row(2);

    row[0] =  0;
    row[1] = -1;

    DataLink::iterator range = link.begin();
    DataTable::iterator position = range->begin();

    size_t oldSize = range->size();
    link.insert(range, position, row);
    CHECK_EQUAL(oldSize + 1, range->size());

    int ordinal = -1;
    for (DataTable::iterator it = range->begin();
            it != range->end(); ++it, ++ordinal)
    {
        CHECK_EQUAL(ordinal, it->get<int>(1));
    }
}

TEST_FIXTURE(FilledLink, InsertRowAtTheEndOfTheLastRange)
{
    DataLink::iterator range = ++link.begin();
    DataTable::iterator first = range->begin();
    DataTable::iterator last = range->end();

    DataRow row(2);

    row[0] =  (*first)[0];
    row[1] =  (*(last-1))[1] + 1;

    size_t oldSize = range->size();
    link.insert(range, last, row);
    CHECK_EQUAL(oldSize + 1, range->size());

    int ordinal = (*range->begin())[1];
    for (DataTable::iterator it = range->begin();
            it != range->end(); ++it, ++ordinal)
    {
        CHECK_EQUAL(ordinal, it->get<int>(1));
    }
}

} // namespace
