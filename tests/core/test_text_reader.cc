/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <algorithm>
#include <cmath>
#include <string>

#include "eckit/testing/Test.h"

#include "odc/csv/TextReader.h"
#include "odc/csv/TextReaderIterator.h"

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

    odc::TextReader reader(data, ",");
    odc::TextReader::iterator it = reader.begin();

    std::vector<long> INTEGERS{1, 1234, -5432, -2147483648, 2147483647, 0, 0};
    std::vector<float> REALS{1.001,
                             0.0,
                             -6.543210,
                             6.543210,
                             std::numeric_limits<float>::quiet_NaN(),
                             std::numeric_limits<float>::infinity(),
                             -std::numeric_limits<float>::infinity()};
    std::vector<double> DOUBLES{
        0, 88, 99.999, 11.63e-37, std::numeric_limits<double>::quiet_NaN(), -std::numeric_limits<double>::infinity(),
        0};
    std::vector<std::string> STRINGS{
        "a-string", "b-string", "string-c", "testing", "12345678", "this-is-a-longer-string", "short"};
    std::vector<long> BITFIELDS{0, 2, 4, 7, 8, 11, 0};

    EXPECT(it->columns().size() == 5);

    EXPECT(it->columns()[0]->name() == "col1");
    EXPECT(it->columns()[1]->name() == "col2");
    EXPECT(it->columns()[2]->name() == "col3");
    EXPECT(it->columns()[3]->name() == "col4");
    EXPECT(it->columns()[4]->name() == "col5");

    EXPECT(it->columns()[0]->type() == odc::api::INTEGER);
    EXPECT(it->columns()[1]->type() == odc::api::REAL);
    EXPECT(it->columns()[2]->type() == odc::api::DOUBLE);
    EXPECT(it->columns()[3]->type() == odc::api::STRING);
    EXPECT(it->columns()[4]->type() == odc::api::BITFIELD);

    size_t count = 0;
    for (; it != reader.end(); ++it) {
        ++count;

        // Only resize on new, longer string
        EXPECT(it->isNewDataset() == (count == 6));

        EXPECT(it->data(0) == INTEGERS[count - 1]);
        EXPECT(it->data(1) == double(REALS[count - 1]) || (std::isnan(it->data(1)) && std::isnan(REALS[count - 1])));
        EXPECT(it->data(2) == DOUBLES[count - 1] || (std::isnan(it->data(2)) && std::isnan(DOUBLES[count - 1])));
        EXPECT(it->dataSizeDoubles(3) == ((count >= 6) ? 3 : 1));
        EXPECT(::strncmp(STRINGS[count - 1].c_str(), (char*)&it->data(3), it->dataSizeDoubles(3) * sizeof(double)) ==
               0);
        EXPECT(it->data(4) == BITFIELDS[count - 1]);

        for (const auto& col : it->columns()) {
            EXPECT(!col->hasMissing());
        }
    }

    EXPECT(count == 7);
}

CASE("Starting with long strings") {

    // n.b. have the first long string not be in the first row, which will force the mechanism
    // to resize.

    std::stringstream data;
    data << "col4:STRING\n";
    data << "a-string-is-long\n";
    data << "b-string-is-very-long-indeed-whoah\n";

    odc::TextReader reader(data, ",");
    odc::TextReader::iterator it = reader.begin();

    std::vector<std::string> STRINGS{"a-string-is-long", "b-string-is-very-long-indeed-whoah"};

    EXPECT(it->columns().size() == 1);
    EXPECT(it->columns()[0]->name() == "col4");
    EXPECT(it->columns()[0]->type() == odc::api::STRING);

    size_t count = 0;
    for (; it != reader.end(); ++it) {
        ++count;

        // Only resize on new, longer string
        EXPECT(it->isNewDataset());
        EXPECT(it->dataSizeDoubles(0) == (count == 1 ? 2 : 5));
        EXPECT(::strncmp(STRINGS[count - 1].c_str(), (char*)&it->data(0), it->dataSizeDoubles(0) * sizeof(double)) ==
               0);

        for (const auto& col : it->columns()) {
            EXPECT(!col->hasMissing());
        }
    }

    EXPECT(count == 2);
}

CASE("Test parsing bitfields") {

    std::string bitfieldDefinition =
        "en4_level_flag@hdr:bitfield[TempLevelReject:1;SaltLevelReject:1;LevelVertStability:1;IncreasingDepthCheck:1;"
        "NotUsed1:1;NotUsed2:1;NotUsed3:1;NotUsed4:1;NotUsed5:1;TempLevelStatList:1;TempLevelArgoQC:1;"
        "TempLevelOutOfRangeSetToMDI:1;TempLevelEN3List:1;TempLevelVertCheck:1;TempLevelNoBckgrnd:1;TempLevelBays:1;"
        "TempLevelBaysBud:1;TempLevelBaysBudReinstate:1;TempLevelWaterfallCheck:1;NotUsed6:1;NotUsed7:1;"
        "SaltLevelStatList:1;SaltLevelArgoQC:1;SaltLevelOutOfRangeSetToMDI:1;SaltLevelEN3List:1;SaltLevelVertCheck:1;"
        "SaltLevelNoBckgrnd:1;SaltLevelBays:1;SaltLevelBaysBud:1;SaltLevelBaysBudReinstate:1;SaltLevelWaterfallCheck:"
        "1]";

    eckit::sql::BitfieldDef def(odc::TextReaderIterator::parseBitfields(bitfieldDefinition));
    eckit::sql::FieldNames names(def.first);
    eckit::sql::Sizes sizes(def.second);

    std::vector<std::string> FIELD_NAMES{"TempLevelReject",
                                         "SaltLevelReject",
                                         "LevelVertStability",
                                         "IncreasingDepthCheck",
                                         "NotUsed1",
                                         "NotUsed2",
                                         "NotUsed3",
                                         "NotUsed4",
                                         "NotUsed5",
                                         "TempLevelStatList",
                                         "TempLevelArgoQC",
                                         "TempLevelOutOfRangeSetToMDI",
                                         "TempLevelEN3List",
                                         "TempLevelVertCheck",
                                         "TempLevelNoBckgrnd",
                                         "TempLevelBays",
                                         "TempLevelBaysBud",
                                         "TempLevelBaysBudReinstate",
                                         "TempLevelWaterfallCheck",
                                         "NotUsed6",
                                         "NotUsed7",
                                         "SaltLevelStatList",
                                         "SaltLevelArgoQC",
                                         "SaltLevelOutOfRangeSetToMDI",
                                         "SaltLevelEN3List",
                                         "SaltLevelVertCheck",
                                         "SaltLevelNoBckgrnd",
                                         "SaltLevelBays",
                                         "SaltLevelBaysBud",
                                         "SaltLevelBaysBudReinstate",
                                         "SaltLevelWaterfallCheck"};

    ASSERT(names.size() == 31);
    ASSERT(sizes.size() == 31);

    EXPECT(names == FIELD_NAMES);
    EXPECT(std::all_of(sizes.begin(), sizes.end(), [](int x) { return x == 1; }));
}


CASE("Test parsing bitfields - 32bit limit") {
    std::string bitfieldDefinition =
        "en4_level_flag@hdr:bitfield[TempLevelReject:1;SaltLevelReject:1;LevelVertStability:1;IncreasingDepthCheck:1;"
        "NotUsed1:1;NotUsed2:1;NotUsed3:1;NotUsed4:1;NotUsed5:1;TempLevelStatList:1;TempLevelArgoQC:1;"
        "TempLevelOutOfRangeSetToMDI:1;TempLevelEN3List:1;TempLevelVertCheck:1;TempLevelNoBckgrnd:1;TempLevelBays:1;"
        "TempLevelBaysBud:1;TempLevelBaysBudReinstate:1;TempLevelWaterfallCheck:1;NotUsed6:1;NotUsed7:1;"
        "SaltLevelStatList:1;SaltLevelArgoQC:1;SaltLevelOutOfRangeSetToMDI:1;SaltLevelEN3List:1;SaltLevelVertCheck:1;"
        "SaltLevelNoBckgrnd:1;SaltLevelBays:1;SaltLevelBaysBud:1;SaltLevelBaysBudReinstate:1;SaltLevelWaterfallCheck:1;"
        "NotUsed8:1;NotUsed9:1]";
    EXPECT_THROWS_AS(odc::TextReaderIterator::parseBitfields(bitfieldDefinition), eckit::UserError);
}

// ------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
