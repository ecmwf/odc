/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"

#include "odc/Select.h"
#include "odc/sql/TODATable.h"

using namespace eckit::testing;

// ------------------------------------------------------------------------------------------------------

CASE("Simple select on CSV (one column) data") {

    // Construct some data to use
    std::stringstream DATA;
    DATA << "a:REAL\n";
    for (size_t i = 1; i <= 10; i++) {
        DATA << i << "\n";
    }

    SECTION("Select with where condition") {
        odc::Select select("select * where a > 4");
        select.database().addImplicitTable(new odc::sql::ODBCSVTable(select.database(), DATA, "input", ","));

        size_t count = 0;
        for (odc::Select::iterator it = select.begin(); it != select.end(); ++it) {
            count++;
            EXPECT((*it)[0] == count+4);
        }

        EXPECT(count == 6);
    }


    SECTION("Sum one column") {
        odc::Select select("select sum(a), count(a), count(*);");
        select.database().addImplicitTable(new odc::sql::ODBCSVTable(select.database(), DATA, "input", ","));

        odc::Select::iterator it = select.begin();

        EXPECT(it != select.end());
        EXPECT((*it)[0] == 55);
        EXPECT((*it)[1] == 10);
        EXPECT((*it)[2] == 10);

        ++it;
        EXPECT(it == select.end());
    }

    SECTION("Mixed output and aggregate") {
        odc::Select select("select a, sum(a), count(a)");
        select.database().addImplicitTable(new odc::sql::ODBCSVTable(select.database(), DATA, "input", ","));

        odc::Select::iterator it = select.begin();

        EXPECT(it != select.end());

        // n.b. as all values of a are different, we get counts of 1

        size_t count = 0;
        for (odc::Select::iterator it = select.begin(); it != select.end(); ++it) {
            count++;
            EXPECT(it->data(0) == count);
            EXPECT(it->data(1) == count);
            EXPECT(it->data(2) == 1);
        }
        EXPECT(count = 10);
    }
}


CASE("Simple select on CSV (two columns) data") {

    // Construct some data to use
    std::stringstream DATA;
    DATA << "a:REAL,b:STRING,c:INTEGER,d:INTEGER\n";
    for (size_t i = 1; i <= 10; i++) {
        DATA << i << ",";
        // length=16/17 --> tests overflowing 16 char and resizing string on last row
        DATA << "a-long-" << i << "-str****,";
        DATA << 11-i << ",";
        DATA << int(i/3) << "\n";
    }

    SECTION("Select with where condition") {
        odc::Select select("select * where a > 4");
        select.database().addImplicitTable(new odc::sql::ODBCSVTable(select.database(), DATA, "input", ","));

        size_t count = 0;
        for (odc::Select::iterator it = select.begin(); it != select.end(); ++it) {
            count++;
            std::stringstream colb;
            colb << "a-long-" << (count+4) << "-str****";
            EXPECT(it->data(0) == count+4);
            EXPECT(it->dataSizeDoubles(1) == (count == 10 ? 3 : 2));
            EXPECT(::strncmp(colb.str().c_str(), (char*)&it->data(1), it->dataSizeDoubles(1)*sizeof(double)) == 0);
            EXPECT(it->data(2) == 11-(count+4));
        }

        EXPECT(count == 6);
    }


    SECTION("Sum and count") {
        odc::Select select("select sum(a), count(a), count(*), sum(c), count(c), count(b);");
        select.database().addImplicitTable(new odc::sql::ODBCSVTable(select.database(), DATA, "input", ","));

        odc::Select::iterator it = select.begin();

        EXPECT(it != select.end());
        EXPECT(it->data(0) == 55);
        EXPECT(it->data(1) == 10);
        EXPECT(it->data(2) == 10);
        EXPECT(it->data(3) == 55);
        EXPECT(it->data(4) == 10);
        EXPECT(it->data(5) == 10);

        ++it;
        EXPECT(it == select.end());
    }

    SECTION("Mixed output and aggregate") {
        odc::Select select("select d, sum(a), count(c)");
        select.database().addImplicitTable(new odc::sql::ODBCSVTable(select.database(), DATA, "input", ","));

        odc::Select::iterator it = select.begin();

        EXPECT(it != select.end());
        bool found[4] = {false, false, false, false};
        for (odc::Select::iterator it = select.begin(); it != select.end(); ++it) {
            EXPECT(0 <= it->data(0) && 3 >= it->data(0));
            found[int(it->data(0))] = true;
            switch(int(it->data(0))) {
            case 0:
                EXPECT(it->data(1) == 3);
                EXPECT(it->data(2) == 2);
                break;
            case 1:
                EXPECT(it->data(1) == 12);
                EXPECT(it->data(2) == 3);
                break;
            case 2:
                EXPECT(it->data(1) == 21);
                EXPECT(it->data(2) == 3);
                break;
            case 3:
                EXPECT(it->data(1) == 19);
                EXPECT(it->data(2) == 2);
                break;
            default:
                ASSERT(false);
            }
        }
        EXPECT(std::all_of(std::begin(found), std::end(found), [](bool x){return x;}));
    }
}

// ------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
