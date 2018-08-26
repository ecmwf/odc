/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <string>
#include <cmath>

#include "eckit/testing/Test.h"

#include "odb_api/csv/TextReader.h"
#include "odb_api/csv/TextReaderIterator.h"

using namespace eckit::testing;


// ------------------------------------------------------------------------------------------------------

CASE("Read columnar data from CSV") {

    // n.b. have the first long string not be in the first row, which will force the mechanism
    // to resize.

    std::stringstream data;
    data << "col1:INTEGER,col2:REAL,col3:DOUBLE,col4:STRING,col5:BITFIELD[a:1;b:2;c:5]\n";
    data << "1,1.001,0,a-string,0\n";
    data << "1234,0,88,b-string,2\n";
    data << "-5432,-6.543210,99.999,string-c,4\n";
    data << "-2147483648,6.543210,11.63e-37,testing,7\n";
    data << "2147483647,NaN,Nan,12345678,8\n";
    data << "0,+inf,-inf,this-is-a-longer-string,11\n";
    data << "0,-inf,0,short,0\n";

    odb::TextReader reader(data, ",");
    odb::TextReader::iterator it = reader.begin();

    std::vector<long> INTEGERS {1, 1234, -5432, -2147483648, 2147483647, 0, 0};
    std::vector<float> REALS {1.001, 0.0, -6.543210, 6.543210, std::numeric_limits<float>::quiet_NaN(),
                              std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity()};
    std::vector<double> DOUBLES {0, 88, 99.999, 11.63e-37, std::numeric_limits<double>::quiet_NaN(),
                                 -std::numeric_limits<double>::infinity(), 0};
    std::vector<std::string> STRINGS {"a-string", "b-string", "string-c", "testing", "12345678", "this-is-a-longer-string", "short"};
    std::vector<long> BITFIELDS {0, 2, 4, 7, 8, 11, 0};

    ASSERT(it->columns().size() == 5);

    ASSERT(it->columns()[0]->name() == "col1");
    ASSERT(it->columns()[1]->name() == "col2");
    ASSERT(it->columns()[2]->name() == "col3");
    ASSERT(it->columns()[3]->name() == "col4");
    ASSERT(it->columns()[4]->name() == "col5");

    ASSERT(it->columns()[0]->type() == odb::INTEGER);
    ASSERT(it->columns()[1]->type() == odb::REAL);
    ASSERT(it->columns()[2]->type() == odb::DOUBLE);
    ASSERT(it->columns()[3]->type() == odb::STRING);
    ASSERT(it->columns()[4]->type() == odb::BITFIELD);

    size_t count = 0;
    for (; it != reader.end(); ++it) {
        ++count;

        // Only resize on new, longer string
        ASSERT(it->isNewDataset() == (count == 6));

        ASSERT(it->data(0) == INTEGERS[count-1]);
        ASSERT(it->data(1) == double(REALS[count-1]) || (std::isnan(it->data(1)) && std::isnan(REALS[count-1])));
        ASSERT(it->data(2) == DOUBLES[count-1] || (std::isnan(it->data(2)) && std::isnan(DOUBLES[count-1])));
        ASSERT(it->dataSizeDoubles(3) == ((count >= 6) ? 3 : 1));
        ASSERT(::strncmp(STRINGS[count-1].c_str(), (char*)&it->data(3), it->dataSizeDoubles(3) * sizeof(double)) == 0);
        ASSERT(it->data(4) == BITFIELDS[count-1]);
    }

    ASSERT(count == 7);
}

CASE("Missing values detected") {}

CASE("Duplicate column names result in an error") {}

CASE("BITFIELD out of range (i.e. unspecified bits set)") {}


// ------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
