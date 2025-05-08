/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file .h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#include <fstream>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/BufferedHandle.h"
#include "eckit/io/FileHandle.h"
#include "eckit/log/Number.h"
#include "eckit/log/Timer.h"
#include "eckit/sql/SQLParser.h"
#include "eckit/sql/SQLSelectFactory.h"

#include "odc/Comparator.h"
#include "odc/DispatchingWriter.h"
#include "odc/ODBAPISettings.h"
#include "odc/Reader.h"
#include "odc/Select.h"
#include "odc/Writer.h"
#include "odc/api/Odb.h"
#include "odc/core/TablesReader.h"
#include "odc/odccapi.h"
#include "odc/tools/CountTool.h"
#include "odc/tools/SplitTool.h"
#include "odc/tools/TestCase.h"

using namespace std;
using namespace eckit;
using namespace odc;
using namespace odc::sql;


typedef long long llong;

static void foobar() {
    Reader in("concatenated.odb");
    Reader::iterator it  = in.begin();
    Reader::iterator end = in.end();

    Writer<> out("copy_of_concatenated.odb");
    Writer<>::iterator o = out.begin();
    o->pass1(it, end);

    Comparator().compare("concatenated.odb", "copy_of_concatenated.odb");
}
// TESTCASE(foobar);

static void createDataForMixedAggregated() {
    // See UnitTest.sql as well
    const char* data =
        "x:INTEGER,y:INTEGER,v:DOUBLE\n"
        "1,1,0.3\n"
        "1,1,0.2\n"
        "2,2,0.4\n"
        "2,2,0.1\n";

    eckit::FileHandle dh("selectAggregatedAndNonAggregated.odb");
    dh.openForWrite(0);
    AutoClose close(dh);
    odc::api::odbFromCSV(data, dh);
}

TEST(selectAggregatedAndNonAggregated) {
    createDataForMixedAggregated();

    odc::Select oda("select x,min(v),max(v) from \"selectAggregatedAndNonAggregated.odb\";");
    odc::Select::iterator it = oda.begin();

    double r0 = (*it)[0], r1 = (*it)[1], r2 = (*it)[2];

    Log::info() << "selectAggregatedAndNonAggregated: " << r0 << ", " << r1 << ", " << r2 << std::endl;

    ASSERT(Comparator::same(r0, 1));
    ASSERT(Comparator::same(r1, 0.2));
    ASSERT(Comparator::same(r2, 0.3));

    ++it;

    r0 = (*it)[0];
    r1 = (*it)[1];
    r2 = (*it)[2];

    Log::info() << "selectAggregatedAndNonAggregated: " << r0 << ", " << r1 << ", " << r2 << std::endl;

    // ASSERT((*it)[0] == 2 && (*it)[1] == 0.1);
    ASSERT(r0 == 2);
    ASSERT(r1 == 0.1);
    ASSERT(r2 == 0.4);
    ++it;
    ASSERT(!(it != oda.end()));
}


static void createDataForMixedAggregated2() {
    Writer<> out("selectAggregatedAndNonAggregated2.odb");
    Writer<>::iterator o = out.begin();
    MetaData md(o->columns());

    md.addColumn /* <DS> */ ("x", "INTEGER");  //, true, .0);
    md.addColumn /* <DS> */ ("y", "INTEGER");  //, true, .0);
    md.addColumn /* <DS> */ ("v", "DOUBLE");   //, true, .0);
    o->columns(md);
    o->writeHeader();

    for (size_t row = 0; row < 1000; ++row)
        for (size_t x = 0; x < 10; (*o)[0] = ++x)
            for (size_t y = 0; y < 10; (*o)[1] = ++y)
                for (double v = 0; v < 10; (*o)[2] = ++v)
                    ++o;
}

TEST(selectAggregatedAndNonAggregated2) {
    createDataForMixedAggregated2();
    odc::Select oda("select x,min(v),y,max(v) from \"selectAggregatedAndNonAggregated2.odb\";");
    odc::Select::iterator it = oda.begin();
    unsigned long counter    = 0;
    for (; it != oda.end(); ++it, ++counter) {
        // double r0 = (*it)[0], r1 = (*it)[1], r2 = (*it)[2], r3 = (*it)[3];
        // Log::info() << "selectAggregatedAndNonAggregated2: " << r0 << ", " << r1 << ", " << r2 << ", " << r3 <<
        // std::endl;
    }
    Log::info() << "selectAggregatedAndNonAggregated2: counter= " << counter << std::endl;
    ASSERT(counter == 110);
}

static void createDataForMixedAggregated3() {
    // See UnitTest.sql as well
    const char* data =
        "x:STRING,y:INTEGER,v:DOUBLE\n"
        "'A',1,0.3\n"
        "'A',1,0.2\n"
        "'B',2,0.4\n"
        "'B',2,0.1\n";

    FileHandle dh("selectAggregatedAndNonAggregated3.odb");
    dh.openForWrite(0);
    AutoClose close(dh);
    odc::api::odbFromCSV(data, dh);
}

TEST(selectAggregatedAndNonAggregated3) {
    createDataForMixedAggregated3();

    odc::Select oda("select x,count(*) from \"selectAggregatedAndNonAggregated3.odb\";");
    odc::Select::iterator it = oda.begin();
    unsigned long counter    = 0;
    for (; it != oda.end(); ++it, ++counter) {
        double r0 = (*it)[0], r1 = (*it)[1];
        Log::info() << "selectAggregatedAndNonAggregated3: " << r0 << ", " << r1 << std::endl;
    }
    Log::info() << "selectAggregatedAndNonAggregated3: counter= " << counter << std::endl;
    ASSERT(counter == 2);
}


static void createDataForMixedAggregatedNULL() {
    // See UnitTest.sql as well
    const char* data =
        "x:REAL,y:INTEGER,v:DOUBLE\n"
        "100,1,0.3\n"
        "100,1,0.2\n"
        "101,2,0.4\n"
        "101,2,0.1\n"
        "NULL,1,0.1\n"
        "NULL,2,0.2\n"
        "NULL,3,0.3\n";

    FileHandle dh("selectAggregatedAndNonAggregatedNULL.odb");
    dh.openForWrite(0);
    AutoClose close(dh);
    odc::api::odbFromCSV(data, dh);
}

TEST(selectAggregatedAndNonAggregatedNULL) {
    createDataForMixedAggregatedNULL();

    odc::Select oda("select x,count(*) from \"selectAggregatedAndNonAggregatedNULL.odb\";");
    odc::Select::iterator it = oda.begin();
    unsigned long counter    = 0;
    for (; it != oda.end(); ++it, ++counter) {
        double r0 = (*it)[0], r1 = (*it)[1];
        Log::info() << "selectAggregatedAndNonAggregatedNULL: " << r0 << ", " << r1 << std::endl;
    }
    Log::info() << "selectAggregatedAndNonAggregatedNULL: counter= " << counter << std::endl;
    ASSERT(counter == 3);
}


/////////////////////////////////////////
// Regular expressions on the select list

static void createDataForRegex1() {
    // See UnitTest.sql as well
    const char* data =
        "aa:INTEGER,ab:INTEGER,ba:INTEGER,bb:INTEGER\n"
        "1,2,3,4\n"
        "10,20,30,40\n"
        "11,22,33,44\n";

    FileHandle dh("regex1.odb");
    dh.openForWrite(0);
    AutoClose close(dh);
    odc::api::odbFromCSV(data, dh);
}

static void regex1() {
    // createDataForRegex1();
    odc::Select oda("select \"/a.*/\" from \"regex1.odb\";");
    odc::Select::iterator it = oda.begin();

    Log::info() << "regex1: " << it->columns() << std::endl;

    ASSERT(it->columns().size() == 2);
}
// TESTCASE(regex1);

TEST(vector_syntax) {

    const char* data =
        "a:INTEGER,b:INTEGER\n"
        "1,1\n"
        "2,2\n"
        "3,3\n"
        "4,4\n"
        "5,5\n"
        "6,6\n"
        "7,7\n"
        "8,8\n"
        "9,9\n"
        "10,10\n";

    {
        FileHandle dh("vector_syntax.odb");
        dh.openForWrite(0);
        AutoClose close(dh);
        odc::api::odbFromCSV(data, dh);
    }

    const char* sql =
        "set $X = [1,2,3,4,5];"
        "select * from \"vector_syntax.odb\" where a in $X;";

    odc::Select oda(sql);
    odc::Select::iterator it  = oda.begin();
    odc::Select::iterator end = oda.end();
    unsigned long counter     = 0;
    for (; it != oda.end(); ++it, ++counter)
        ;
    ASSERT(counter == 5);
}


TEST(bitfieldsLength) {
    Log::info() << "sizeof(eckit::log::Number::W)" << sizeof(eckit::log::Number::W) << std::endl;
    Log::info() << "sizeof(double)" << sizeof(double) << std::endl;

    //>>> int('0b11100110011',2)
    // 1843
    //>>> len('0b11100110011')
    // 13
    //>>>
    // stringstream s;
    // eckit::log::Number::printBinary(s, 1843);
    // string r = s.str();
    // Log::info() << "r: " << r << std::endl;

    ASSERT(eckit::log::Number::printBinary(1843).size() == 11);
    ASSERT(eckit::log::Number::printBinary(1843) == "11100110011");

    ASSERT(eckit::log::Number::printBinary(0).size() == 1);
    ASSERT(eckit::log::Number::printBinary(0) == "0");
}

/// ODB-85
TEST(bitfieldsPrintHexadecimal) {
    ASSERT(eckit::log::Number::printHexadecimal(1843) == std::string("733"));

    // eckit::Log::info() << eckit::log::Number::printHexadecimal(15)  << std::endl;
    ASSERT(eckit::log::Number::printHexadecimal(10) == std::string("a"));
    ASSERT(eckit::log::Number::printHexadecimal(11) == std::string("b"));
    ASSERT(eckit::log::Number::printHexadecimal(15) == std::string("f"));
    ASSERT(eckit::log::Number::printHexadecimal(255) == std::string("ff"));
}

static void create_stringInWhere_file() {
    const char* data =
        "a:STRING,b:INTEGER\n"
        "'aaa',1\n"
        "'aaaa',2\n"
        "'bbb',2\n"
        "'bbbc',2\n";
    FileHandle dh("stringInWhere.odb");
    dh.openForWrite(0);
    AutoClose close(dh);
    odc::api::odbFromCSV(data, dh);
}

TEST(stringInWhere) {
    create_stringInWhere_file();
    odc::Select oda("select * from 'stringInWhere.odb' where a = 'aaa';");

    unsigned long counter = 0;
    for (odc::Select::iterator it = oda.begin(); it != oda.end(); ++it, ++counter)
        ;
    ASSERT(counter == 1);
}

TEST(vector_syntax2) {
    const char* sql = "set $y = 100; set $x = [$y, 'a', 'b', [1, 2]];";
    eckit::sql::SQLSession session;
    eckit::sql::SQLParser::parseString(session, sql);
}

TEST(rownumber1) {
    const char* inputData =
        "a:INTEGER,b:INTEGER\n"
        "1,1\n"
        "2,2\n"
        "3,3\n"
        "4,4\n"
        "5,5\n"
        "6,6\n"
        "7,7\n"
        "8,8\n"
        "9,9\n"
        "10,10\n";
    string path("Test_rownumber1.odb");

    {
        FileHandle dh(path);
        dh.openForWrite(0);
        AutoClose close(dh);
        odc::api::odbFromCSV(inputData, dh);
    }

    string query("SELECT rownumber() from \"" + path + "\";");

    odc::Select select(query);
    odc::Select::iterator it  = select.begin();
    odc::Select::iterator end = select.end();

    llong i = 0;
    for (; it != end; ++it) {
        ASSERT((*it)[0] == ++i);
    }
    ASSERT(i == 10);
}

TEST(sqlOutputFormatting) {
    /*
    // See UnitTest.sql as well
    const char *data =
            "x:REAL,y:INTEGER,v:DOUBLE\n"
            "100,1,0.3\n"
            "100,1,0.2\n"
            "101,2,0.4\n"
            "101,2,0.1\n"
            "NULL,1,0.1\n"
            "NULL,2,0.2\n"
            "NULL,3,0.3\n"
            ;

    const char* testFile("sqlOutputFormatting.odb");

    FileHandle dh(testFile);
    dh.openForWrite(0);
    AutoClose close(dh);
    odc::api::odbFromCSV(data, dh);

    bool doNotWriteColumnNames(false); // -T
    bool doNotWriteNULL(false);        // -N
    string delimiter(" ");           // -delimiter
    string inputFile(testFile);           // -i
    string outputFile;          // -o
    string outputFormat;        // default is ascii

    odc::sql::SQLSelectFactory::instance()
            .config(odc::sql::SQLOutputConfig(doNotWriteColumnNames, doNotWriteNULL, delimiter, outputFile,
    outputFormat));

    ostream& out = cout;
    odc::sql::SQLInteractiveSession session(out);
    odc::sql::SQLParser p;

    FileHandle fh(inputFile);
    fh.openForRead();
    //p.parseString(StringTool::readFile(fileName), &fh, odc::sql::SQLSelectFactory::instance().config());
    p.parseString("select x,y,v;", &fh, odc::sql::SQLSelectFactory::instance().config());
    */
}

static void createDataForWindSpeedWindDirection() {
    const char* data =
        "u:REAL,v:REAL\n"
        "11.7,-5.8\n"
        "0.0,0.0\n"
        "0,5.4\n"
        "5.4,0.0\n";

    FileHandle dh("uv.odb");
    dh.openForWrite(0);
    AutoClose close(dh);
    odc::api::odbFromCSV(data, dh);
}

TEST(windSpeedWindDirection) {
    createDataForWindSpeedWindDirection();
    string path("uv.odb");
    string query("SELECT ff(u,v), dd(u,v), speed(u,v),dir(u,v), sqrt(u*u+v*v), fmod(atan2(-u,-v)+360.,360.) from \"" +
                 path + "\";");

    odc::Select select(query);
    odc::Select::iterator it  = select.begin();
    odc::Select::iterator end = select.end();

    llong i = 0;
    for (; it != end; ++it) {
        Log::info() << " ff = " << (*it)[0] << " speed sqrt= " << (*it)[4] << std::endl;
        Log::info() << " dd = " << (*it)[1] << " direction atan= " << (*it)[5] << std::endl;
        ASSERT((*it)[0] == (*it)[4]);
        ASSERT((*it)[1] == (*it)[5]);
    }
}

// TEST(odbcapi)
//{
//	odc::tool::test::test_odacapi_setup_in_C(0,0);
//	odc::tool::test::test_odacapi3(0,0);
// }


static void SplitTool_chunks() {
    const char* fn                       = "selectAggregatedAndNonAggregated.odb";
    size_t n                             = odc::tool::CountTool::rowCount(fn);
    vector<pair<Offset, Length> > chunks = odc::tool::SplitTool::getChunks(fn);

    Log::info() << "chunks.size():" << chunks.size() << std::endl;
    ASSERT(chunks.size() == 1 && chunks[0].first == Offset(0) && chunks[0].second == Length(357));
}
// TESTCASE(SplitTool_chunks);


static void FilePool1() {
    // FilePool<SimpleFilePoolTraits> pool(1);
}
// TESTCASE(FilePool1);

static void copyVectorToArray() {
    const size_t size = 1024;
    const size_t n    = 1000000;

    vector<double> v(size);
    double a[size];

    {
        Timer timer("std::copy");
        for (size_t i = 0; i < n; ++i)
            std::copy(v.begin(), v.end(), a);
    }

    {
        Timer timer("for loop");
        for (size_t i = 0; i < n; ++i)
            for (size_t j = 0; j < size; ++j)
                a[j] = v[j];
    }
}
// TESTCASE(copyVectorToArray);


static void count(void* counter, const double* data, size_t n) {
    ++*((llong*)counter);
}
static void* create_counter() {
    llong* r = new llong;
    *r       = 0;
    return r;
}
static void destroy_counter(void* counter) {
    delete (llong*)counter;
}
static void* reduce_counter(void* left, void* right) {
    llong* result = new llong;
    *result       = (*(llong*)left) + (*(llong*)right);
    return result;
}

class TemporaryPathName : public PathName {
public:

    TemporaryPathName(const string& fn) : PathName(fn) {}
    ~TemporaryPathName() { unlink(); }
};

typedef TemporaryPathName ScratchFile;

TEST(hash_operator_on_select_list) {
    const char* data =
        "x:INTEGER,y:INTEGER\n"
        "1,1\n"
        "2,2\n"
        "3,3\n"
        "4,4\n"
        "5,5\n"
        "6,6\n"
        "7,7\n"
        "8,8\n"
        "9,9\n"
        "10,10\n";

    ScratchFile f("hash_operator_on_select_list.odb");

    {
        FileHandle dh(f);
        dh.openForWrite(0);
        AutoClose close(dh);
        odc::api::odbFromCSV(data, dh);
    }

    string sql("select x,x#-1,x#1 from \"" + f + "\";");
    odc::Select select(sql);
    odc::Select::iterator it  = select.begin();
    odc::Select::iterator end = select.end();
    for (; it != end; ++it) {
        Log::info() << it << std::endl;
    }
}

/// Shift or hash (#) operator doesn't work in the WHERE clause.
/// This test doesn't test anything yet.
TEST(hash_operator_in_where) {
    const char* data =
        "x:INTEGER,y:INTEGER\n"
        "1,1\n"
        "2,2\n"
        "3,3\n"
        "4,4\n"
        "5,5\n"
        "6,6\n"
        "7,7\n"
        "8,8\n"
        "9,9\n"
        "10,10\n";

    ScratchFile f("hash_operator_in_where.odb");

    {
        FileHandle dh(f);
        dh.openForWrite(0);
        AutoClose close(dh);
        odc::api::odbFromCSV(data, dh);
    }

    string sql("select x,x#-1,x#1 from \"" + f + "\" where x=2 and x#1=3;");
    odc::Select select(sql);
    odc::Select::iterator it  = select.begin();
    odc::Select::iterator end = select.end();
    for (; it != end; ++it) {
        Log::info() << it << std::endl;
    }
}

TEST(bitfields_hash_operator) {
    PathName f("2000010106-reduced.odb");
    // odc::Select select("select lat,lat#1 from \"" + f + "\"");
    odc::Select select("select anflag@body,anflag.final@body,anflag.*@body from \"" + f + "\";");
    odc::Select::iterator it  = select.begin();
    odc::Select::iterator end = select.end();
    for (; it != end; ++it) {
        Log::info() << it << std::endl;
    }
}

TEST(select_constant_value) {
    const char* sql =
        "set $foo = 27;"
        "select $foo;";

    odc::Select o(sql);

    odc::Select::iterator it  = o.begin();
    odc::Select::iterator end = o.end();
    unsigned long counter     = 0;
    for (; it != o.end(); ++it, ++counter) {
        Log::info() << it << std::endl;
        CHECK_EQUAL(it->data(0), 27);
    }
    CHECK_EQUAL(counter, 1);
}

TEST(include) {
    ofstream f("stuff.sql");
    f
        //<< "select * from \"file1.odb\";" << endl
        << "set $foo = 10;" << endl
        << "set $bar = 20;" << std::endl;
    f.close();

    const char* sql =
        "#include \"stuff.sql\"\n"
        "set $baz = $bar;"
        "select $foo * $bar;";

    unsigned long counter = 0;
    odc::Select o(sql);
    for (odc::Select::iterator it(o.begin()), end(o.end()); it != o.end(); ++it, ++counter) {
        Log::info() << it << std::endl;
    }
}

TEST(log_error) {
    Log::error() << "Just a logger test" << std::endl;
    // TODO: test Log::error writes to stderr
    // TODO: test Log::error has a prefirx with timestamp and other things
}

/*
TEST(create_table_using_variable)
{

    const char *sql =
    "SET $c = { 2 : \"foo\" };\n"
    "SET $s = 2;\n"
    "CREATE TABLE t AS (c[$s]);"
    ;

    unsigned long counter = 0;
    odc::Select o(sql);
    for (odc::Select::iterator it(o.begin()), end(o.end());
        it != o.end();
        ++it, ++counter)
    {
        Log::info() << it << std::endl;
    }
}
*/

TEST(meta_data_reader_checks_if_file_truncated) {
    ASSERT(0 == std::system("dd if=disp.7.1.odb of=disp.7.1.odb.truncated bs=30121 count=1"));
    core::TablesReader mdr("disp.7.1.odb.truncated");
    try {
        for (auto it(mdr.begin()), end(mdr.end()); it != end; ++it)
            ;
        ASSERT(0 && "Scanning of truncated file did not fail");
    }
    catch (odc::core::ODBIncomplete ex) {
        Log::info() << "Scanning of truncated file disp.7.1.odb.truncated failed as expected." << std::endl;
    }
}

TEST(operator_ge) {
    const char* data =
        "a:INTEGER,b:INTEGER\n"
        "1,1\n"
        "2,2\n"
        "3,3\n"
        "4,4\n"
        "5,5\n"
        "6,6\n"
        "7,7\n"
        "8,8\n"
        "9,9\n"
        "10,10\n";

    {
        FileHandle dh("1to10.odb");
        dh.openForWrite(0);
        AutoClose close(dh);
        odc::api::odbFromCSV(data, dh);
    }

    odc::Select odb("select a,b from \"1to10.odb\" where a >= 3;");
    unsigned long counter = 0;
    for (odc::Select::iterator it = odb.begin(), end = odb.end(); it != end; ++it, ++counter)
        ;
    ASSERT(counter == 8);
}

static void create_1to10() {
    const char* data =
        "a:INTEGER,b:INTEGER\n"
        "1,1\n"
        "2,2\n"
        "3,3\n"
        "4,4\n"
        "5,5\n"
        "6,6\n"
        "7,7\n"
        "8,8\n"
        "9,9\n"
        "10,10\n";

    FileHandle dh("1to10.odb");
    dh.openForWrite(0);
    AutoClose close(dh);
    odc::api::odbFromCSV(data, dh);
}

/* FIXME
TEST(Select_isNewDataset)
{
    create_1to10();
    size_t blocks (0);

    odc::Select odb("select * from \"1to10.odb\";");
    for (odc::Select::iterator it = odb.begin(), end = odb.end();
        it != end;
        ++it)
        if (it->isNewDataset())
            ++blocks;
    ASSERT(blocks == 1);
}
*/

template <typename T>
static void test_isNewDataset() {
    create_1to10();
    size_t blocks(0);

    blocks = 0;
    T odb("1to10.odb");
    for (typename T::iterator it = odb.begin(), end = odb.end(); it != end; ++it)
        if (it->isNewDataset())
            ++blocks;
    ASSERT(blocks == 1);
}

/*
TEST(Gabor)
{
    const char * cfg =
    "CLASS: class\n"
    "DATE: andate\n"
    "TIME: antime\n"
    "TYPE: type\n"
    "OBSGROUP: groupid\n"
    "REPORTYPE: reportype\n"
    "STREAM: stream\n"
    "EXPVER: expver\n"
    ;
    const string fileName("/tmp/gabor/massaged.odb");

    odc::FastODA2Request<odc::ODA2RequestClientTraits> o2r;
    o2r.parseConfig(cfg);

    eckit::OffsetList offsets;
    eckit::LengthList lengths;
    vector<ODAHandle*> handles;
    bool rc = o2r.scanFile(fileName, offsets, lengths, handles);
    for (size_t i = 0; i < handles.size(); ++i)
        delete handles[i];
    handles.clear();
    ASSERT(rc);

    ASSERT(lengths.size());
    ASSERT(lengths.size() == offsets.size());
    for(size_t i = 1; i < offsets.size(); i++)
        ASSERT(offsets[i] > offsets[i-1]);
    size_t last = offsets.size()-1;
    ASSERT(PathName(fileName).size() == offsets[last] + lengths[last]);

    unsigned long long cnt = o2r.rowsNumber();

    string filesRequest = "ARCHIVE,\n";
    filesRequest += o2r.genRequest();
}
// FIXME
TEST(Reader_isNewDataset) { test_isNewDataset<Reader>(); }
// FIXME
TEST(MetaDataReader_isNewDataset) { test_isNewDataset<odc::MetaDataReader<MetaDataReaderIterator> >(); }

TEST(create_temporary_table)
{
    const char* sql = "CREATE "
                      " TEMPORARY "
                      " TABLE foo AS (col1 pk9real, col2 pk9real,) INHERITS (bar,baz);";

    cout << "Trying to execute: '" << sql << "'" << std::endl;

    odc::Select o(sql);
    odc::tool::SQLTool::execute(sql);

}
*/


TEST(JULIAN_SECONDS) {
    ASSERT(1 == (*odc::Select("select julian_seconds(19750311,0) < julian_seconds(20140210,0) from dual;").begin())[0]);
}

TEST(CREATE_TABLE_and_SELECT_INTO) {
    const char* inputData =
        R"(a:INTEGER,b:INTEGER
        1,1
        2,2
        3,3
        4,4
        5,5
        6,6
        7,7
        8,8
        9,9
        10,10)";

    {
        FileHandle dh("CREATE_TABLE_and_SELECT_INTO.odb");
        dh.openForWrite(0);
        AutoClose close(dh);
        odc::api::odbFromCSV(inputData, dh);
    }

    const char* sql = R"(
        CREATE TYPE mybitfield AS (
            codetype bit9,
            instype bit10,
            retrtype bit6,
            geoarea bit6,
        );

        CREATE TABLE "foo.odb" AS (
            lat real,
            lon real,
            status mybitfield,
        );

        SELECT a,b,a*b INTO "foo.odb" FROM "CREATE_TABLE_and_SELECT_INTO.odb";
    )";

    {
        odc::Select o(sql);
        odc::Select::iterator it = o.begin();
        unsigned long counter    = 0;
        Log::info() << "Inside select" << std::endl;
        for (; it != o.end(); ++it, ++counter) {
            Log::info() << "Getting a line..." << std::endl;
            ;
        }
        Log::info() << "CREATE_TABLE_and_SELECT_INTO: counter=" << counter << endl;
    }
    std::system("ls -l foo.odb; ");
    std::system((eckit::PathName("~/bin/odc").asString() + " header foo.odb").c_str());
    std::system((eckit::PathName("~/bin/odc").asString() + " ls foo.odb").c_str());
}

/*
TEST(SELECT_ALL)
{
    ostream& L(eckit::Log::info());
    odc::api::odbFromCSV("a:INTEGER,b:INTEGER\n1,2\n", "select_all_1.odb");
    odc::api::odbFromCSV("a:INTEGER,b:INTEGER,c:INTEGER\n1,2,3\n", "select_all_2.odb");
    std::system("cat select_all_1.odb select_all_2.odb >select_all.odb");

    L << "--- Test_SELECT_ALL: open select_all.odb" << endl;
    odc::Select o("SELECT ALL * FROM \"select_all.odb\";");
    odc::Select::iterator it (o.begin()), end (o.end());
    L << "--- Test_SELECT_ALL: row #0" << endl;
    ++it;
    ASSERT(it->columns().size() == 2);
    L << "--- Test_SELECT_ALL: row #1" << endl;
    ++it;
    ASSERT(it->columns().size() == 3);
}
*/

// ODB-106
TEST(SELECT_WHERE_0) {
    {
        FileHandle dh("select_where_0.odb");
        dh.openForWrite(0);
        AutoClose close(dh);
        odc::api::odbFromCSV("a:INTEGER,b:INTEGER\n1,2\n3,4\n", dh);
    }
    odc::Select o("SELECT * FROM \"select_where_0.odb\" WHERE 0;");
    odc::Select::iterator it(o.begin()), end(o.end());
    ++it;
}

TEST(QuestionMarkHandlingWhenSplittingByStringColumn_ODB235) {
    const char* inFile("ODB_235.odb");
    const char* data(
        "a:INTEGER,b:INTEGER,expver:STRING\n"
        "1,1,'?'\n"
        "2,2,'?'\n"
        "3,3,'?'\n");
    const char* outFileTemplate("ODB_235_{a}_{expver}.odb");

    {
        FileHandle dh(inFile);
        dh.openForWrite(0);
        AutoClose close(dh);
        odc::api::odbFromCSV(data, dh);
    }

    odc::Reader in(inFile);
    odc::DispatchingWriter out(outFileTemplate, /*maxOpenFiles*/ 3);

    odc::DispatchingWriter::iterator outIt(out.begin());
    outIt->pass1(in.begin(), in.end());

    ASSERT(PathName("ODB_235_1_?.odb").exists());
    ASSERT(PathName("ODB_235_2_?.odb").exists());
    ASSERT(PathName("ODB_235_3_?.odb").exists());

    PathName("ODB_235_1_?.odb").unlink();
    PathName("ODB_235_2_?.odb").unlink();
    PathName("ODB_235_3_?.odb").unlink();
    PathName("ODB_235.odb").unlink();
}

TEST(LegacyAPIExecuteSelectTwice) {
    const std::string fn("legacy_execute_select_twice.odb");
    {
        FileHandle dh(fn);
        dh.openForWrite(0);
        AutoClose close(dh);
        odc::api::odbFromCSV("a:INTEGER,b:INTEGER\n1,2\n3,4\n", dh);
    }
    odc::Select o(std::string("SELECT * FROM \"") + fn + "\";");

    int i(0), j(0);

    for (odc::Select::iterator it(o.begin()), end(o.end()); it != end; ++it)
        ++i;

    ASSERT(i == 2);

    for (odc::Select::iterator it(o.begin()), end(o.end()); it != end; ++it)
        ++j;

    ASSERT(j == 2);
}

TEST(LegacyAPITraverseReaderTwice) {
    const std::string fn("legacy_traverse_reader_twice.odb");
    {
        FileHandle dh(fn);
        dh.openForWrite(0);
        AutoClose close(dh);
        odc::api::odbFromCSV("a:INTEGER,b:INTEGER\n1,2\n3,4\n", dh);
    }
    odc::Reader o(fn);

    int i(0), j(0);

    for (odc::Reader::iterator it(o.begin()), end(o.end()); it != end; ++it)
        ++i;

    ASSERT(i == 2);

    for (odc::Reader::iterator it(o.begin()), end(o.end()); it != end; ++it)
        ++j;

    ASSERT(j == 2);
}
