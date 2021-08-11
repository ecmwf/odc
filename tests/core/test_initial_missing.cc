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
#include "eckit/log/Log.h"

#include "odc/api/Odb.h"
#include "odc/Reader.h"


using namespace eckit::testing;

namespace {
    struct TestIntegerDecoding {
        TestIntegerDecoding() { odc::api::Settings::treatIntegersAsDoubles(false); }
        ~TestIntegerDecoding() { odc::api::Settings::treatIntegersAsDoubles(true); }
    };
}

// ------------------------------------------------------------------------------------------------------

namespace {
    int64_t integer_cast(const double& val) {
        static_assert(sizeof(double) == sizeof(int64_t), "Punning is not valid");
        auto punnable_val = reinterpret_cast<const char*>(&val);
        auto punned_val = reinterpret_cast<const int64_t*>(punnable_val);
        return *punned_val;
    }
}

CASE("Test all-missing first row with Decoder integers-as-doubles") {

    odc::api::Reader reader("odb_533_1.odb", /* aggregated */ false);

    odc::api::Frame frame = reader.next();
    EXPECT(frame.columnCount() == 4);
    EXPECT(frame.rowCount() == 2);
    EXPECT(frame.columnInfo()[0].name == "stringval");
    EXPECT(frame.columnInfo()[1].name == "intval");
    EXPECT(frame.columnInfo()[2].name == "realval");
    EXPECT(frame.columnInfo()[3].name == "doubleval");

    constexpr size_t nrows = 2;
    char stringvals[nrows][8];
    double intvals[nrows];
    double realvals[nrows];
    double doublevals[nrows];

    // n.b. columns in reverse order, to be awkward
    std::vector<std::string> columns { "doubleval", "realval", "intval", "stringval" };
    std::vector<odc::api::StridedData> strides {
            {doublevals, nrows, sizeof(doublevals[0]), sizeof(doublevals[0])},
            {realvals,   nrows, sizeof(realvals[0]),   sizeof(realvals[0])},
            {intvals,    nrows, sizeof(intvals[0]),    sizeof(intvals[0])},
            {stringvals, nrows, sizeof(stringvals[0]), sizeof(stringvals[0])},
    };

    odc::api::Decoder decoder(columns, strides);
    decoder.decode(frame);

    EXPECT(::memcmp(stringvals[0], "\0\0\0\0\0\0\0\0", 8) == 0);
    EXPECT(intvals[0] == odc::api::Settings::integerMissingValue());
    EXPECT(realvals[0] == odc::api::Settings::doubleMissingValue());
    EXPECT(doublevals[0] == odc::api::Settings::doubleMissingValue());
    EXPECT(::memcmp(stringvals[1], "testing\0", 8) == 0);
    EXPECT(intvals[1] == 12345678);
    EXPECT(realvals[1] == 1234.56);
    EXPECT(doublevals[1] == 9876.54);

    EXPECT(!reader.next());
}

CASE("Test all-missing first row with Decoder integers-as-integers") {

    TestIntegerDecoding integers;
    odc::api::Reader reader("odb_533_1.odb", /* aggregated */ false);

    odc::api::Frame frame = reader.next();
    EXPECT(frame.columnCount() == 4);
    EXPECT(frame.rowCount() == 2);
    EXPECT(frame.columnInfo()[0].name == "stringval");
    EXPECT(frame.columnInfo()[1].name == "intval");
    EXPECT(frame.columnInfo()[2].name == "realval");
    EXPECT(frame.columnInfo()[3].name == "doubleval");

    constexpr size_t nrows = 2;
    char stringvals[nrows][8];
    int64_t intvals[nrows];
    double realvals[nrows];
    double doublevals[nrows];

    // n.b. columns in reverse order, to be awkward
    std::vector<std::string> columns { "doubleval", "realval", "intval", "stringval" };
    std::vector<odc::api::StridedData> strides {
            {doublevals, nrows, sizeof(doublevals[0]), sizeof(doublevals[0])},
            {realvals,   nrows, sizeof(realvals[0]),   sizeof(realvals[0])},
            {intvals,    nrows, sizeof(intvals[0]),    sizeof(intvals[0])},
            {stringvals, nrows, sizeof(stringvals[0]), sizeof(stringvals[0])},
    };

    odc::api::Decoder decoder(columns, strides);
    decoder.decode(frame);

    EXPECT(::memcmp(stringvals[0], "\0\0\0\0\0\0\0\0", 8) == 0);
    EXPECT(intvals[0] == odc::api::Settings::integerMissingValue());
    EXPECT(realvals[0] == odc::api::Settings::doubleMissingValue());
    EXPECT(doublevals[0] == odc::api::Settings::doubleMissingValue());
    EXPECT(::memcmp(stringvals[1], "testing\0", 8) == 0);
    EXPECT(intvals[1] == 12345678);
    EXPECT(realvals[1] == 1234.56);
    EXPECT(doublevals[1] == 9876.54);

    EXPECT(!reader.next());
}

CASE("Test some-missing first row with Decoder integers-as-doubles") {

    odc::api::Reader reader("odb_533_2.odb", /* aggregated */ false);

    odc::api::Frame frame = reader.next();
    EXPECT(frame.columnCount() == 5);
    EXPECT(frame.rowCount() == 2);
    EXPECT(frame.columnInfo()[0].name == "stringval");
    EXPECT(frame.columnInfo()[1].name == "intval");
    EXPECT(frame.columnInfo()[2].name == "realval");
    EXPECT(frame.columnInfo()[3].name == "doubleval");
    EXPECT(frame.columnInfo()[4].name == "changing");

    constexpr size_t nrows = 2;
    char stringvals[nrows][8];
    double intvals[nrows];
    double realvals[nrows];
    double doublevals[nrows];
    double changing[nrows];

    // n.b. columns in reverse order, to be awkward
    std::vector<std::string> columns { "changing", "doubleval", "realval", "intval", "stringval" };
    std::vector<odc::api::StridedData> strides {
            {changing,   nrows, sizeof(changing[0]),   sizeof(changing[0])},
            {doublevals, nrows, sizeof(doublevals[0]), sizeof(doublevals[0])},
            {realvals,   nrows, sizeof(realvals[0]),   sizeof(realvals[0])},
            {intvals,    nrows, sizeof(intvals[0]),    sizeof(intvals[0])},
            {stringvals, nrows, sizeof(stringvals[0]), sizeof(stringvals[0])},
    };

    odc::api::Decoder decoder(columns, strides);
    decoder.decode(frame);

    EXPECT(::memcmp(stringvals[0], "\0\0\0\0\0\0\0\0", 8) == 0);
    EXPECT(intvals[0] == odc::api::Settings::integerMissingValue());
    EXPECT(realvals[0] == odc::api::Settings::doubleMissingValue());
    EXPECT(doublevals[0] == odc::api::Settings::doubleMissingValue());
    EXPECT(changing[0] == 1234);
    EXPECT(::memcmp(stringvals[1], "testing\0", 8) == 0);
    EXPECT(intvals[1] == 12345678);
    EXPECT(realvals[1] == 1234.56);
    EXPECT(doublevals[1] == 9876.54);
    EXPECT(changing[1] == 5678);

    EXPECT(!reader.next());
}

CASE("Test some-missing first row with Decoder integers-as-integers") {

    TestIntegerDecoding integers;
    odc::api::Reader reader("odb_533_2.odb", /* aggregated */ false);

    odc::api::Frame frame = reader.next();
    EXPECT(frame.columnCount() == 5);
    EXPECT(frame.rowCount() == 2);
    EXPECT(frame.columnInfo()[0].name == "stringval");
    EXPECT(frame.columnInfo()[1].name == "intval");
    EXPECT(frame.columnInfo()[2].name == "realval");
    EXPECT(frame.columnInfo()[3].name == "doubleval");
    EXPECT(frame.columnInfo()[4].name == "changing");

    constexpr size_t nrows = 2;
    char stringvals[nrows][8];
    int64_t intvals[nrows];
    double realvals[nrows];
    double doublevals[nrows];
    int64_t changing[nrows];

    // n.b. columns in reverse order, to be awkward
    std::vector<std::string> columns { "changing", "doubleval", "realval", "intval", "stringval" };
    std::vector<odc::api::StridedData> strides {
            {changing,   nrows, sizeof(changing[0]),   sizeof(changing[0])},
            {doublevals, nrows, sizeof(doublevals[0]), sizeof(doublevals[0])},
            {realvals,   nrows, sizeof(realvals[0]),   sizeof(realvals[0])},
            {intvals,    nrows, sizeof(intvals[0]),    sizeof(intvals[0])},
            {stringvals, nrows, sizeof(stringvals[0]), sizeof(stringvals[0])},
    };

    odc::api::Decoder decoder(columns, strides);
    decoder.decode(frame);

    EXPECT(::memcmp(stringvals[0], "\0\0\0\0\0\0\0\0", 8) == 0);
    EXPECT(intvals[0] == odc::api::Settings::integerMissingValue());
    EXPECT(realvals[0] == odc::api::Settings::doubleMissingValue());
    EXPECT(doublevals[0] == odc::api::Settings::doubleMissingValue());
    EXPECT(changing[0] == 1234);
    EXPECT(::memcmp(stringvals[1], "testing\0", 8) == 0);
    EXPECT(intvals[1] == 12345678);
    EXPECT(realvals[1] == 1234.56);
    EXPECT(doublevals[1] == 9876.54);
    EXPECT(changing[1] == 5678);

    EXPECT(!reader.next());
}

CASE("Test all-missing first row with Reader iterator integers-as-doubles") {

    odc::Reader reader("odb_533_1.odb");

    auto it = reader.begin();
    auto end = reader.end();
    EXPECT(it != end);

    EXPECT(it->columns().size() == 4);
    EXPECT(it->columns()[0]->name() == "stringval");
    EXPECT(it->columns()[0]->coder().name() == "int8_string");
    EXPECT(it->columns()[1]->name() == "intval");
    EXPECT(it->columns()[1]->coder().name() == "constant_or_missing");
    EXPECT(it->columns()[2]->name() == "realval");
    EXPECT(it->columns()[2]->coder().name() == "real_constant_or_missing");
    EXPECT(it->columns()[3]->name() == "doubleval");
    EXPECT(it->columns()[3]->coder().name() == "real_constant_or_missing");

    EXPECT( it->isMissing(1));
    EXPECT( it->isMissing(2));
    EXPECT( it->isMissing(3));
    EXPECT(it->string(0) == "");
    EXPECT((*it)[1] == odc::api::Settings::integerMissingValue());
    EXPECT(static_cast<int64_t>((*it)[1]) == odc::api::Settings::integerMissingValue());
    EXPECT((*it)[2] == odc::api::Settings::doubleMissingValue());
    EXPECT((*it)[3] == odc::api::Settings::doubleMissingValue());

    ++it;
    EXPECT(it != end);

    EXPECT( !it->isMissing(1));
    EXPECT( !it->isMissing(2));
    EXPECT( !it->isMissing(3));
    EXPECT(it->string(0) == "testing");
    EXPECT((*it)[1] == 12345678);
    EXPECT(static_cast<int64_t>((*it)[1]) == 12345678);
    EXPECT((*it)[2] == 1234.56);
    EXPECT((*it)[3] == 9876.54);

    ++it;
    EXPECT(it == end);
}

CASE("Test all-missing first row with Reader iterator integers-as-integers") {

    TestIntegerDecoding integers;
    odc::Reader reader("odb_533_1.odb");

    auto it = reader.begin();
    auto end = reader.end();
    EXPECT(it != end);

    EXPECT(it->columns().size() == 4);
    EXPECT(it->columns()[0]->name() == "stringval");
    EXPECT(it->columns()[0]->coder().name() == "int8_string");
    EXPECT(it->columns()[1]->name() == "intval");
    EXPECT(it->columns()[1]->coder().name() == "constant_or_missing");
    EXPECT(it->columns()[2]->name() == "realval");
    EXPECT(it->columns()[2]->coder().name() == "real_constant_or_missing");
    EXPECT(it->columns()[3]->name() == "doubleval");
    EXPECT(it->columns()[3]->coder().name() == "real_constant_or_missing");

    EXPECT( it->isMissing(1));
    EXPECT( it->isMissing(2));
    EXPECT( it->isMissing(3));
    EXPECT(it->string(0) == "");
    EXPECT(integer_cast((*it)[1]) == odc::api::Settings::integerMissingValue());
    EXPECT((*it)[2] == odc::api::Settings::doubleMissingValue());
    EXPECT((*it)[3] == odc::api::Settings::doubleMissingValue());

    ++it;
    EXPECT(it != end);

    EXPECT( !it->isMissing(1));
    EXPECT( !it->isMissing(2));
    EXPECT( !it->isMissing(3));
    EXPECT(it->string(0) == "testing");
    EXPECT(integer_cast((*it)[1]) == 12345678);
    EXPECT((*it)[2] == 1234.56);
    EXPECT((*it)[3] == 9876.54);

    ++it;
    EXPECT(it == end);
}

CASE("Test some-missing first row with Reader iterator integers-as-doubles") {

    odc::Reader reader("odb_533_2.odb");

    auto it = reader.begin();
    auto end = reader.end();
    EXPECT(it != end);

    EXPECT(it->columns().size() == 5);
    EXPECT(it->columns()[0]->name() == "stringval");
    EXPECT(it->columns()[0]->coder().name() == "int8_string");
    EXPECT(it->columns()[1]->name() == "intval");
    EXPECT(it->columns()[1]->coder().name() == "constant_or_missing");
    EXPECT(it->columns()[2]->name() == "realval");
    EXPECT(it->columns()[2]->coder().name() == "real_constant_or_missing");
    EXPECT(it->columns()[3]->name() == "doubleval");
    EXPECT(it->columns()[3]->coder().name() == "real_constant_or_missing");
    EXPECT(it->columns()[4]->name() == "changing");
    EXPECT(it->columns()[4]->coder().name() == "int16");

    EXPECT( it->isMissing(1));
    EXPECT( it->isMissing(2));
    EXPECT( it->isMissing(3));
    EXPECT( !it->isMissing(4));
    EXPECT(it->string(0) == "");
    EXPECT((*it)[1] == odc::api::Settings::integerMissingValue());
    EXPECT(static_cast<int64_t>((*it)[1]) == odc::api::Settings::integerMissingValue());
    EXPECT((*it)[2] == odc::api::Settings::doubleMissingValue());
    EXPECT((*it)[3] == odc::api::Settings::doubleMissingValue());
    EXPECT((*it)[4] == 1234);

    ++it;
    EXPECT(it != end);

    EXPECT( !it->isMissing(1));
    EXPECT( !it->isMissing(2));
    EXPECT( !it->isMissing(3));
    EXPECT(it->string(0) == "testing");
    EXPECT((*it)[1] == 12345678);
    EXPECT(static_cast<int64_t>((*it)[1]) == 12345678);
    EXPECT((*it)[2] == 1234.56);
    EXPECT((*it)[3] == 9876.54);
    EXPECT((*it)[4] == 5678);

    ++it;
    EXPECT(it == end);
}

CASE("Test some-missing first row with Reader iterator integers-as-integers") {

    TestIntegerDecoding integers;
    odc::Reader reader("odb_533_2.odb");

    auto it = reader.begin();
    auto end = reader.end();
    EXPECT(it != end);

    EXPECT(it->columns().size() == 5);
    EXPECT(it->columns()[0]->name() == "stringval");
    EXPECT(it->columns()[0]->coder().name() == "int8_string");
    EXPECT(it->columns()[1]->name() == "intval");
    EXPECT(it->columns()[1]->coder().name() == "constant_or_missing");
    EXPECT(it->columns()[2]->name() == "realval");
    EXPECT(it->columns()[2]->coder().name() == "real_constant_or_missing");
    EXPECT(it->columns()[3]->name() == "doubleval");
    EXPECT(it->columns()[3]->coder().name() == "real_constant_or_missing");
    EXPECT(it->columns()[4]->name() == "changing");
    EXPECT(it->columns()[4]->coder().name() == "int16");

    EXPECT( it->isMissing(1));
    EXPECT( it->isMissing(2));
    EXPECT( it->isMissing(3));
    EXPECT(it->string(0) == "");
    EXPECT(integer_cast((*it)[1]) == odc::api::Settings::integerMissingValue());
    EXPECT((*it)[2] == odc::api::Settings::doubleMissingValue());
    EXPECT((*it)[3] == odc::api::Settings::doubleMissingValue());
    EXPECT(integer_cast((*it)[4]) == 1234);

    ++it;
    EXPECT(it != end);

    EXPECT( !it->isMissing(1));
    EXPECT( !it->isMissing(2));
    EXPECT( !it->isMissing(3));
    EXPECT(it->string(0) == "testing");
    EXPECT(integer_cast((*it)[1]) == 12345678);
    EXPECT((*it)[2] == 1234.56);
    EXPECT((*it)[3] == 9876.54);
    EXPECT(integer_cast((*it)[4]) == 5678);

    ++it;
    EXPECT(it == end);
}

// ------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {

    return run_tests(argc, argv);
}
