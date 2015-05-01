/// @file   TestDataLoader.cc
/// @author Tomas Kral

#include <string>

#include "eckit/filesystem/TmpFile.h"
#include "odb_api/Reader.h"

#include "odb_api/tools/TestCase.h"
#include "odb_api/tools/ToolFactory.h"
#include "odb_api/DataLoader.h"
#include "odb_api/DataSaver.h"
#include "odb_api/DataTable.h"
#include "odb_api/DataTableMappings.h"
#include "odb_api/DataSet.h"

using namespace std;
using namespace odb;
using namespace eckit;

namespace {

typedef odb::DataStream<odb::SameByteOrder, odb::PrettyFastInMemoryDataHandle> DataStream;

struct Fixture
{
    Fixture();

    TmpFile input;
    TmpFile output;
};

const int PARENT_ROWS_COUNT = 5; 
const int CHILD_ROWS_COUNT = 10; 
const int TOTAL_ROWS_COUNT = PARENT_ROWS_COUNT * CHILD_ROWS_COUNT;

Fixture::Fixture()
  : input(),
    output()
{
    odb::Writer<> writer(input);
    odb::Writer<>::iterator it = writer.begin();

    MetaData md (it->columns());
    md.addColumn<DataStream>("parent_id@parent", "INTEGER");
    md.addColumn<DataStream>("child.offset@parent", "INTEGER");
    md.addColumn<DataStream>("child.len@parent", "INTEGER");
    md.addColumn<DataStream>("child_id@child", "INTEGER");
    it->columns(md);
    it->writeHeader();

    for (int ordinal = 0; ordinal < TOTAL_ROWS_COUNT; ++ordinal, ++it)
    {
        const int offset = ordinal / CHILD_ROWS_COUNT * CHILD_ROWS_COUNT;

        it->data()[0] = offset / 10;
        it->data()[1] = offset;
        it->data()[2] = CHILD_ROWS_COUNT;
        it->data()[3] = ordinal;
    }
}

TEST_FIXTURE(Fixture, LoadDataSetFromSqlQueries)
{
    DataLoader loader(input);
    DataSet dataset("test");

    loader.select("SELECT DISTINCT parent_id;");
    loader.fill(dataset, "parent");
    loader.select("SELECT child_id;");
    loader.fill(dataset, "child");

    DataTable* parent = *dataset.tables().find("parent");
    DataTable* child = *dataset.tables().find("child");

    CHECK(parent);
    CHECK(child);

    CHECK_EQUAL(PARENT_ROWS_COUNT, (int)parent->size());
    CHECK_EQUAL(TOTAL_ROWS_COUNT, (int)child->size());

    int ordinal = 0;
    for (DataTable::const_iterator it = parent->begin();
            it != parent->end(); ++it, ++ordinal)
    {
        CHECK_EQUAL(ordinal, it->get<int>(0));
    }

    ordinal = 0;
    for (DataTable::const_iterator it = child->begin();
            it != child->end(); ++it, ++ordinal)
    {
        CHECK_EQUAL(ordinal, it->get<int>(0));
    }
}

TEST_FIXTURE(Fixture, DataLoaderWithTableMappings)
{
    DataSet dataset; 

    DataTableMappings mappings;
    mappings.add("parent", "header");
    mappings.add("child", "body");

    DataLoader loader(input);
    loader.mappings(mappings);
    loader.fill(dataset);

    CHECK(dataset.tables().count("header"));
    CHECK(dataset.tables().count("body"));
}

struct FilledDataSet : public Fixture
{
    DataSet dataset;

    FilledDataSet() : dataset()
    {
        DataLoader loader(input);
        loader.fill(dataset);
    }
};

TEST_FIXTURE(FilledDataSet, DataLoaderReturnsExpectedNumberOfTables)
{
    CHECK_EQUAL(2u, dataset.tables().size());
}

TEST_FIXTURE(FilledDataSet, DataLoaderReturnsExpectedTables)
{
    CHECK(dataset.tables().count("parent"));
    CHECK(dataset.tables().count("child"));
}

struct ParentTable : public FilledDataSet
{
    const DataTable& parent;

    ParentTable()
      : FilledDataSet(),
        parent(*dataset.tables()["parent"])
    {}
};

TEST_FIXTURE(ParentTable, DataLoaderReturnsExpectedNumberOfParentTableColumns)
{
    CHECK_EQUAL(1u, parent.columns().size());
}

TEST_FIXTURE(ParentTable, DataLoaderReturnsExpectedNumberOfParentTableRows)
{
    int count = 0;

    for (DataTable::const_iterator row = parent.begin();
            row != parent.end(); ++row, ++count);

    CHECK_EQUAL(PARENT_ROWS_COUNT, count);
}

TEST_FIXTURE(ParentTable, DataLoaderReturnsExpectedParentTableRows)
{
    int ordinal = 0;

    for (DataTable::const_iterator row = parent.begin();
            row != parent.end(); ++row, ++ordinal)
    {
        CHECK_EQUAL(ordinal, row->get<int>(0));
    }
}

struct ChildTable : public FilledDataSet
{
    const DataTable& child;

    ChildTable()
      : FilledDataSet(),
        child(*dataset.tables()["child"])
    {}
};

TEST_FIXTURE(ChildTable, DataLoaderReturnsExpectedNumberOfChildTableColumns)
{
    CHECK_EQUAL(1u, child.columns().size());
}

TEST_FIXTURE(ChildTable, DataLoaderReturnsExpectedNumberOfChildTableRows)
{
    int count = 0;

    for (DataTable::const_iterator row = child.begin();
            row != child.end(); ++row, ++count);

    CHECK_EQUAL(TOTAL_ROWS_COUNT, count);
}

TEST_FIXTURE(ChildTable, DataLoaderReturnsExpectedChildTableRows)
{
    int ordinal = 0;

    for (DataTable::const_iterator row = child.begin();
            row != child.end(); ++row, ++ordinal)
    {
        CHECK_EQUAL(ordinal, row->get<int>(0));
    }
}

TEST_FIXTURE(FilledDataSet, DataSaverOutputContainsExpectedRows)
{
    DataSaver saver(output);
    saver.save(dataset, "parent");
    saver.close();

    Reader expected(input);
    Reader actual(output);

    Reader::iterator e = expected.begin();
    Reader::iterator a = actual.begin();

    for (size_t size = e->columns().size();
            e != expected.end(); ++e, ++a)
    {
        CHECK_ARRAY_EQUAL(e->data(), a->data(), size);
    }
}

} // namespace
