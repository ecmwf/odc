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

#include "eckit/io/FileHandle.h"
#include "eckit/io/MemoryHandle.h"
#include "eckit/log/Log.h"
#include "eckit/types/FixedString.h"

#include "eckit/sql/SQLOutputConfig.h"
#include "eckit/sql/SQLParser.h"
#include "eckit/sql/SQLSession.h"
#include "eckit/sql/SQLStatement.h"

#include "odc/Writer.h"
#include "odc/api/Odb.h"
#include "odc/sql/ODAOutput.h"
#include "odc/sql/TODATable.h"

#include <cstdint>
#include <sstream>
#include <type_traits>
#include <iomanip>

using namespace eckit::testing;
using eckit::Log;

// ------------------------------------------------------------------------------------------------------

template <typename T> odc::api::ColumnType typeToName() { ASSERT(false); return odc::api::IGNORE; }
template <> odc::api::ColumnType typeToName<double>() { return odc::api::DOUBLE; }
template <> odc::api::ColumnType typeToName<int64_t>() { return odc::api::INTEGER; }
template <> odc::api::ColumnType typeToName<uint64_t>() { return odc::api::BITFIELD; }
template <> odc::api::ColumnType typeToName<eckit::FixedString<8>>() { return odc::api::STRING; }
template <> odc::api::ColumnType typeToName<eckit::FixedString<16>>() { return odc::api::STRING; }
template <> odc::api::ColumnType typeToName<eckit::FixedString<32>>() { return odc::api::STRING; }

void quick_encode_internal(eckit::DataHandle& out,
                           std::vector<odc::api::ColumnInfo>& columnInfo,
                           std::vector<odc::api::ConstStridedData>& strides) {
    odc::api::encode(out, columnInfo, strides);
}

template <typename T,
          typename ...Ts>
typename std::enable_if<!std::is_same<T, uint64_t>::value, void>::type
quick_encode_internal(eckit::DataHandle& out,
                           std::vector<odc::api::ColumnInfo>& columnInfo,
                           std::vector<odc::api::ConstStridedData>& strides,
                           const std::string& colname,
                           const std::vector<T>& vals,
                           Ts&&... args) {

    columnInfo.emplace_back(odc::api::ColumnInfo{colname, typeToName<T>(), sizeof(vals[0])});
    strides.emplace_back(odc::api::ConstStridedData{&vals[0], vals.size(), sizeof(vals[0]), sizeof(vals[0])});
    quick_encode_internal(out, columnInfo, strides, std::forward<Ts>(args)...);
}

// Bitfields need some extra info

template <typename ...Ts>
void quick_encode_internal(eckit::DataHandle& out,
                           std::vector<odc::api::ColumnInfo>& columnInfo,
                           std::vector<odc::api::ConstStridedData>& strides,
                           const std::string& colname,
                           const std::vector<uint64_t>& vals,
                           const std::vector<odc::api::ColumnInfo::Bit>& bits,
                           Ts&&... args) {

    columnInfo.emplace_back(odc::api::ColumnInfo{colname, odc::api::BITFIELD, sizeof(vals[0]), bits});
    strides.emplace_back(odc::api::ConstStridedData{&vals[0], vals.size(), sizeof(vals[0]), sizeof(vals[0])});
    quick_encode_internal(out, columnInfo, strides, std::forward<Ts>(args)...);
}

template <typename ...Ts>
void quick_encode(eckit::DataHandle& out, Ts&&... args) {
    std::vector<odc::api::ColumnInfo> columnInfo;
    std::vector<odc::api::ConstStridedData> strides;
    quick_encode_internal(out, columnInfo, strides, std::forward<Ts>(args)...);
}

// ------------------------------------------------------------------------------------------------------

// We need to specialise the comparison for FixedString, so that we can have different sized fixed
// buffers containing the same string

template <typename T, typename S>
bool vectorEquals(const std::vector<T>& lhs, const std::vector<S>& rhs) {
    return lhs == rhs;
}

template <int L1, int L2>
bool vectorEquals(const std::vector<eckit::FixedString<L1>>& lhs, const std::vector<eckit::FixedString<L2>>& rhs) {
    if (lhs.size() != rhs.size()) return false;
    for (size_t i = 0; i < lhs.size(); ++i) {
        if (lhs[i].asString() != rhs[i].asString()) return false;
    }
    return true;
}

template <typename ...Ts>
void decode_and_test_internal(eckit::DataHandle& in,
                              std::vector<std::string>& columnNames,
                              std::vector<odc::api::StridedData>& strides) {

    odc::api::Decoder decoder(columnNames, strides);

    in.openForRead();
    eckit::AutoClose closer(in);

    // n.b. we don't use an aggregated reader, as we may well be able to extract the specified
    //      columns, even if the overall structure cannot be aggregated.
    odc::api::Reader reader(in);

    size_t nrows = 0;
    while (nrows < strides[0].nelem()) {
        odc::api::Frame frame = reader.next();
        EXPECT(frame);
        size_t frameRows = frame.rowCount();
        odc::api::Decoder&& subDecoder = decoder.slice(nrows, frameRows);
        subDecoder.decode(frame);
        nrows += frameRows;
    }

    // We have consumed all the data
    EXPECT(!reader.next());
}

template <typename T, typename ...Ts>
void decode_and_test_internal(eckit::DataHandle& in,
                              std::vector<std::string>& columnNames,
                              std::vector<odc::api::StridedData>& strides,
                              const std::string& colname,
                              const std::vector<T>& correctVals,
                              Ts&&... args) {

    std::vector<T> decodeBuffer(correctVals.size());
    ASSERT(decodeBuffer.size() == correctVals.size());
    columnNames.push_back(colname);
    strides.emplace_back(&decodeBuffer[0], decodeBuffer.size(), sizeof(decodeBuffer[0]), sizeof(decodeBuffer[0]));

    decode_and_test_internal(in, columnNames, strides, std::forward<Ts>(args)...);

    if (!vectorEquals(decodeBuffer, correctVals)) {
        Log::warning() << "Buffers mismatched for column " << colname << std::endl;
        ASSERT(decodeBuffer.size() == correctVals.size());
        for (size_t i = 0; i < decodeBuffer.size(); ++i) {
            bool match = (decodeBuffer[i] == correctVals[i]);
            Log::warning() << (match ? "      " : "  *** ")
                           << std::setw(15) << std::right << correctVals[i]
                           << (match ? " == " : " != ")
                           << std::setw(15) << std::left << decodeBuffer[i]
                           << (match ? "      " : "  *** ")
                           << std::endl;
        }
    }
    EXPECT(vectorEquals(decodeBuffer, correctVals));
}

template <typename ...Ts>
void decode_and_test(eckit::DataHandle& in, Ts&&... args) {
    odc::api::Settings::treatIntegersAsDoubles(false);
    std::vector<std::string> columnNames;
    std::vector<odc::api::StridedData> strides;
    decode_and_test_internal(in, columnNames, strides, std::forward<Ts>(args)...);
}


// ------------------------------------------------------------------------------------------------------

class DHOutputConfig : public eckit::sql::SQLOutputConfig {
    eckit::DataHandle& dh_;

public:
    DHOutputConfig(eckit::DataHandle& dh) :
        eckit::sql::SQLOutputConfig(), dh_(dh) {}

    eckit::sql::SQLOutput* buildOutput(const eckit::PathName& path) const override {
        return new odc::sql::ODAOutput<odc::Writer<>>(new odc::Writer<>(dh_));
    }
};

void oda_select_filter(const std::string& sql, eckit::DataHandle& dh_in, eckit::DataHandle& dh_out) {

    std::unique_ptr<eckit::sql::SQLOutputConfig> output_config(new DHOutputConfig(dh_out));
    eckit::sql::SQLSession session{std::move(output_config)};

    dh_in.openForRead();
    eckit::AutoClose closer(dh_in);

    eckit::sql::SQLDatabase& db(session.currentDatabase());
    db.addImplicitTable( new odc::sql::ODATable(db, dh_in));;

    eckit::sql::SQLParser parser;
    parser.parseString(session, sql);
    session.statement().execute();
}


// ------------------------------------------------------------------------------------------------------

CASE("short and then longer strings") {

    double dmiss = odc::api::Settings::doubleMissingValue();
    int64_t imiss = odc::api::Settings::integerMissingValue();

    std::vector<double> doubleVals{111.1, 222.2, dmiss, 333.3};
    std::vector<int64_t> intVals{1, imiss, 2, 3};
    std::vector<eckit::FixedString<8>> stringVals1{"a-string", "bbbb", "", "zzzzzzzz"};
    std::vector<eckit::FixedString<16>> stringVals2{"a-stringa-string", "bbbbaaacccppp", "", "zzzzzzzz"};

    // Construct the source odb

    eckit::MemoryHandle source_odb;

    {
        source_odb.openForWrite(0);
        eckit::AutoClose closer(source_odb);

        odc::api::Settings::treatIntegersAsDoubles(false);
        quick_encode(source_odb, "doublecol", doubleVals, "stringcol", stringVals1, "intcolumn", intVals);
        quick_encode(source_odb, "doublecol", doubleVals, "stringcol", stringVals2, "intcolumn", intVals);
        quick_encode(source_odb, "doublecol", doubleVals, "stringcol", stringVals1, "intcolumn", intVals);
        EXPECT(source_odb.position() == eckit::Offset(1289));
    }
    EXPECT(source_odb.size() == eckit::Length(1289));

    eckit::MemoryHandle output_odb;
    std::string select_string = "select *";
    bool filtered = false;

    {
        SECTION("Select Class (integer internals)") {
            odc::api::Settings::treatIntegersAsDoubles(false);
            ::odc::api::filter(select_string, source_odb, output_odb);
            filtered = true;
        }

        SECTION("Select Class (double internals)") {
            odc::api::Settings::treatIntegersAsDoubles(true);
            ::odc::api::filter(select_string, source_odb, output_odb);
            filtered = true;
        }

        SECTION( "ODAOutput (integer internals)") {
            odc::api::Settings::treatIntegersAsDoubles(false);
            oda_select_filter(select_string, source_odb, output_odb);
            filtered = true;
        }

        SECTION( "ODAOutput (double internals)") {
            odc::api::Settings::treatIntegersAsDoubles(true);
            oda_select_filter(select_string, source_odb, output_odb);
            filtered = true;
        }
    }

    // Read back from the source ODB, and check that the contents are sane

    if (filtered) {
        std::vector<double> doubleCorrect;
        std::vector<int64_t> intCorrect;
        for (int i = 0; i < 3; i++) {
            doubleCorrect.insert(doubleCorrect.end(), doubleVals.begin(), doubleVals.end());
            intCorrect.insert(intCorrect.end(), intVals.begin(), intVals.end());
        }

        std::vector<eckit::FixedString<16>> stringCorrect;
        stringCorrect.insert(stringCorrect.end(), stringVals1.begin(), stringVals1.end());
        stringCorrect.insert(stringCorrect.end(), stringVals2.begin(), stringVals2.end());
        stringCorrect.insert(stringCorrect.end(), stringVals1.begin(), stringVals1.end());

        decode_and_test(output_odb, "doublecol", doubleCorrect, "stringcol", stringCorrect, "intcolumn", intCorrect);
    }
}

CASE("Extraction of bitfield values") {

    int64_t imiss = odc::api::Settings::integerMissingValue();
    uint64_t uimiss = static_cast<uint64_t>(odc::api::Settings::integerMissingValue());

    std::vector<odc::api::ColumnInfo::Bit> bits1 {
        {"bit1", 1, 0},
        {"bit2", 2, 1},
        {"bit3", 1, 3}};
    std::vector<odc::api::ColumnInfo::Bit> bits2 {
        {"bit4", 4, 0},
        {"bit5", 2, 4},
        {"bit6", 1, 6}};

    std::vector<uint64_t> bf1Vals {0, 5, uimiss, 15};
    std::vector<uint64_t> bf2Vals {127, uimiss, 42, 0};

    std::vector<int64_t> bit1Vals {0, 1, imiss, 1};
    std::vector<int64_t> bit2Vals {0, 2, imiss, 3};
    std::vector<int64_t> bit3Vals {0, 0, imiss, 1};
    std::vector<int64_t> bit4Vals {15, imiss, 10, 0};
    std::vector<int64_t> bit5Vals { 3, imiss,  2, 0};
    std::vector<int64_t> bit6Vals { 1, imiss,  0, 0};

    // Construct the source odb

    static int cnt = 0;
    eckit::MemoryHandle source_odb;

    {
        source_odb.openForWrite(0);
        eckit::AutoClose closer(source_odb);

        odc::api::Settings::treatIntegersAsDoubles(false);
        quick_encode(source_odb, "bitfield1", bf1Vals, bits1, "bitfield2", bf2Vals, bits2);
    }

    // Select a selection of bits, both with and without the entire column

    /// @note - it would be really nice to test the SQL layer using treatIntegersAsDoubles(false). Unfortunately, it
    ///         doesn't work, and can't cleanly work. eckit has to be agnostic to the odc interface, and it just
    ///         passes values through (whether doubles, or integers punned into doubles). But to do bitfield
    ///         extraction/manipulation, it really needs to know what form they are. This is hard coded to involve
    ///         casting around doubles, which is really yucky. Bleurgh.
    /// TODO: Make the eckit layer use only proper integers internally...

    eckit::MemoryHandle output_odb;
    std::string select_string = "select bitfield1.bit1, bitfield1.bit3, bitfield1.bit2, bitfield1, bitfield2.bit5, bitfield2.bit4, bitfield2.bit6";
    bool filtered = false;

    {
#if 0
        SECTION("Select Class (integer internals)") {
            odc::api::Settings::treatIntegersAsDoubles(false);
            EXPECT_THROWS_AS(::odc::api::filter(select_string, source_odb, output_odb), eckit::SeriousBug);
        }
#endif

        SECTION("Select Class (double internals)") {
            odc::api::Settings::treatIntegersAsDoubles(true);
            ::odc::api::filter(select_string, source_odb, output_odb);
            filtered = true;
        }

#if 0
        SECTION( "ODAOutput (integer internals)") {
            odc::api::Settings::treatIntegersAsDoubles(false);
            EXPECT_THROWS_AS(oda_select_filter(select_string, source_odb, output_odb), eckit::SeriousBug);
        }
#endif

        SECTION( "ODAOutput (double internals)") {
            odc::api::Settings::treatIntegersAsDoubles(true);
            oda_select_filter(select_string, source_odb, output_odb);
            filtered = true;
        }
    }

    // Read back from the source ODB, and check that the contents are sane

    if (filtered) {
        decode_and_test(output_odb,
                        "bitfield1.bit1", bit1Vals,
                        "bitfield1.bit3", bit3Vals,
                        "bitfield1.bit2", bit2Vals,
                        "bitfield1", bf1Vals,
                        "bitfield2.bit5", bit5Vals,
                        "bitfield2.bit4", bit4Vals,
                        "bitfield2.bit6", bit6Vals);
    }
}

// ------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
