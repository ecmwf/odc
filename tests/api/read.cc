/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <fstream>
#include <memory>

#include "eckit/io/FileHandle.h"
#include "eckit/testing/Test.h"

#include "odc/api/Odb.h"
#include "odc/api/odc.h"

using namespace eckit::testing;

// ------------------------------------------------------------------------------------------------------

bool test_check_file_exists(std::string file_path) {
    std::ifstream file_stream(file_path);
    return file_stream.good();
}

void test_generate_odb_properties(const std::string& path, int propertiesMode = 0) {
    odc::api::Settings::treatIntegersAsDoubles(false);

    // Define row count
    const size_t nrows = 10;

    // Allocate data array for each column
    char data0[nrows][8];
    int64_t data1[nrows];
    double data2[nrows];

    size_t i;

    // Set up the allocated arrays with scratch data
    for (i = 0; i < nrows; i++) {
        snprintf(data0[i], 8, "xxxx");  // expver
        data1[i] = 20210527;            // date@hdr
        data2[i] = 12.3456 * i;         // obsvalue@body
    }

    // Define all column names, their types and sizes
    std::vector<odc::api::ColumnInfo> columns = {
        {std::string("expver"), odc::api::ColumnType(odc::api::STRING), 8},
        {std::string("date@hdr"), odc::api::ColumnType(odc::api::INTEGER), sizeof(int64_t)},
        {std::string("obsvalue@body"), odc::api::ColumnType(odc::api::REAL), sizeof(double)},
    };

    // Set a custom data layout and data array for each column
    std::vector<odc::api::ConstStridedData> strides{
        // ptr, nrows, element_size, stride
        {data0, nrows, 8, 8},
        {data1, nrows, sizeof(int64_t), sizeof(int64_t)},
        {data2, nrows, sizeof(double), sizeof(double)},
    };

    std::map<std::string, std::string> properties = {};

    const eckit::Length length;

    eckit::FileHandle fh(path);
    fh.openForWrite(length);
    eckit::AutoClose closer(fh);

    // Encode two ODB-2 frames with the same data
    for (i = 0; i < 2; i++) {

        // Encode additional properties depending on the current mode
        switch (propertiesMode) {

            // Where the properties in the two frames are distinct
            case 1: {
                if (i == 0)
                    properties["foo"] = "bar";
                else {
                    properties        = {};  // reset
                    properties["baz"] = "qux";
                }
                break;
            }

            // Where the properties in the two frames overlap with entries that are the same
            case 2: {
                properties["foo"] = "bar";
                properties["baz"] = "qux";
                break;
            }

            // Where the properties overlap with entries whose keys are the same, but the values different
            case 3: {
                if (i == 0)
                    properties["foo"] = "bar";
                else
                    properties["foo"] = "baz";
                break;
            }

            default:
                break;
        }

        // Encode the ODB-2 frame into a data handle
        encode(fh, columns, strides, properties);
    }

    ASSERT(test_check_file_exists(path));
}

void test_generate_odb_span(const std::string& path) {
    odc::api::Settings::treatIntegersAsDoubles(false);

    // Define row count
    const size_t nrows = 10;

    // Allocate data array for each column in all three of the frames
    char data0_0[nrows][8];
    int64_t data0_1[nrows];
    double data0_2[nrows];
    double data0_3[nrows];

    char data1_0[nrows][8];
    int64_t data1_1[nrows];
    double data1_2[nrows];
    double data1_3[nrows];

    char data2_0[nrows][8];
    int64_t data2_1[nrows];
    double data2_2[nrows];
    double data2_3[nrows];

    int i;

    // Set up the allocated arrays with scratch data
    for (i = 0; i < nrows; i++) {
        snprintf(data0_0[i], 8, "xxxx");                        // expver
        data0_1[i] = 20210527;                                  // date@hdr
        data0_2[i] = 12.3456 * i;                               // obsvalue@body
        data0_3[i] = odc::api::Settings::doubleMissingValue();  // missing_value

        snprintf(data1_0[i], 8, "xxxx");                        // expver
        data1_1[i] = 20210528;                                  // date@hdr
        data1_2[i] = 12.3456 * i;                               // obsvalue@body
        data1_3[i] = odc::api::Settings::doubleMissingValue();  // missing_value

        snprintf(data2_0[i], 8, "xxxx");                        // expver
        data2_1[i] = 20210529;                                  // date@hdr
        data2_2[i] = 12.3456 * i;                               // obsvalue@body
        data2_3[i] = odc::api::Settings::doubleMissingValue();  // missing_value
    }

    // Define all column names, their types and sizes
    std::vector<odc::api::ColumnInfo> columns = {
        {std::string("expver"), odc::api::ColumnType(odc::api::STRING), 8},
        {std::string("date@hdr"), odc::api::ColumnType(odc::api::INTEGER), sizeof(int64_t)},
        {std::string("obsvalue@body"), odc::api::ColumnType(odc::api::REAL), sizeof(double)},
        {std::string("missing_value"), odc::api::ColumnType(odc::api::REAL), sizeof(double)},
    };

    // Set a custom data layout and data array for each column and frame
    std::vector<odc::api::ConstStridedData> strides0{
        // ptr, nrows, element_size, stride
        {data0_0, nrows, 8, 8},
        {data0_1, nrows, sizeof(int64_t), sizeof(int64_t)},
        {data0_2, nrows, sizeof(double), sizeof(double)},
        {data0_3, nrows, sizeof(double), sizeof(double)},
    };

    std::vector<odc::api::ConstStridedData> strides1{
        // ptr, nrows, element_size, stride
        {data1_0, nrows, 8, 8},
        {data1_1, nrows, sizeof(int64_t), sizeof(int64_t)},
        {data1_2, nrows, sizeof(double), sizeof(double)},
        {data1_3, nrows, sizeof(double), sizeof(double)},
    };

    std::vector<odc::api::ConstStridedData> strides2{
        // ptr, nrows, element_size, stride
        {data2_0, nrows, 8, 8},
        {data2_1, nrows, sizeof(int64_t), sizeof(int64_t)},
        {data2_2, nrows, sizeof(double), sizeof(double)},
        {data2_3, nrows, sizeof(double), sizeof(double)},
    };

    const eckit::Length length;

    eckit::FileHandle fh(path);
    fh.openForWrite(length);
    eckit::AutoClose closer(fh);

    // Encode each ODB-2 frame into the same data handle
    encode(fh, columns, strides0);
    encode(fh, columns, strides1);
    encode(fh, columns, strides2);

    ASSERT(test_check_file_exists(path));
}

// ------------------------------------------------------------------------------------------------------

CASE("Count lines in an existing ODB file") {

    bool aggregated = false;
    odc::api::Reader reader("../2000010106-reduced.odb", aggregated);

    size_t nframes   = 0;
    size_t totalRows = 0;

    odc::api::Frame frame;

    while ((frame = reader.next())) {
        totalRows += frame.rowCount();
        EXPECT(frame.columnCount() == 51);
        ++nframes;
    }

    EXPECT(nframes == 5);
    EXPECT(totalRows == 50000);
}

CASE("Check column details in an existing ODB file") {

    std::vector<std::string> cols{
        "expver@desc",
        "andate@desc",
        "antime@desc",
        "seqno@hdr",
        "obstype@hdr",
        "obschar@hdr",
        "subtype@hdr",
        "date@hdr",
        "time@hdr",
        "rdbflag@hdr",
        "status@hdr",
        "event1@hdr",
        "blacklist@hdr",
        "sortbox@hdr",
        "sitedep@hdr",
        "statid@hdr",
        "ident@hdr",
        "lat@hdr",
        "lon@hdr",
        "stalt@hdr",
        "modoro@hdr",
        "trlat@hdr",
        "trlon@hdr",
        "instspec@hdr",
        "event2@hdr",
        "anemoht@hdr",
        "baroht@hdr",
        "sensor@hdr",
        "numlev@hdr",
        "varno_presence@hdr",
        "varno@body",
        "vertco_type@body",
        "rdbflag@body",
        "anflag@body",
        "status@body",
        "event1@body",
        "blacklist@body",
        "entryno@body",
        "press@body",
        "press_rl@body",
        "obsvalue@body",
        "aux1@body",
        "event2@body",
        "ppcode@body",
        "level@body",
        "biascorr@body",
        "final_obs_error@errstat",
        "obs_error@errstat",
        "repres_error@errstat",
        "pers_error@errstat",
        "fg_error@errstat",
    };

    std::vector<int> types{
        odc::api::STRING,   odc::api::INTEGER,  odc::api::INTEGER,  odc::api::INTEGER,  odc::api::INTEGER,
        odc::api::BITFIELD, odc::api::INTEGER,  odc::api::INTEGER,  odc::api::INTEGER,  odc::api::BITFIELD,
        odc::api::BITFIELD, odc::api::BITFIELD, odc::api::BITFIELD, odc::api::INTEGER,  odc::api::INTEGER,
        odc::api::STRING,   odc::api::INTEGER,  odc::api::REAL,     odc::api::REAL,     odc::api::REAL,
        odc::api::REAL,     odc::api::REAL,     odc::api::REAL,     odc::api::INTEGER,  odc::api::INTEGER,
        odc::api::REAL,     odc::api::REAL,     odc::api::INTEGER,  odc::api::INTEGER,  odc::api::BITFIELD,
        odc::api::INTEGER,  odc::api::INTEGER,  odc::api::BITFIELD, odc::api::BITFIELD, odc::api::BITFIELD,
        odc::api::BITFIELD, odc::api::BITFIELD, odc::api::INTEGER,  odc::api::REAL,     odc::api::REAL,
        odc::api::REAL,     odc::api::REAL,     odc::api::INTEGER,  odc::api::INTEGER,  odc::api::BITFIELD,
        odc::api::REAL,     odc::api::REAL,     odc::api::REAL,     odc::api::REAL,     odc::api::REAL,
        odc::api::REAL,
    };

    std::vector<std::string> bitfields{
        "lat_humon",   "lat_qcsub",   "lat_override",   "lat_flag",   "lat_hqc_flag",
        "lon_humon",   "lon_qcsub",   "lon_override",   "lon_flag",   "lon_hqc_flag",
        "date_humon",  "date_qcsub",  "date_override",  "date_flag",  "date_hqc_flag",
        "time_humon",  "time_qcsub",  "time_override",  "time_flag",  "time_hqc_flag",
        "stalt_humon", "stalt_qcsub", "stalt_override", "stalt_flag", "stalt_hqc_flag",
    };

    std::vector<int> bitfield_sizes{
        1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1,
    };

    bool aggregated = false;
    odc::api::Reader reader("../2000010106-reduced.odb", aggregated);

    // Get the first frame
    odc::api::Frame frame = reader.next();

    EXPECT(frame.columnCount() == 51);

    int i = 0;

    // Iterate over frame columns
    for (const auto& column : frame.columnInfo()) {
        const int col = i++;

        EXPECT(column.name == cols.at(col));
        EXPECT(column.type == types.at(col));
        EXPECT(column.decodedSize == 8);

        if (column.type == odc::api::BITFIELD) {
            EXPECT(column.bitfield.size() > 0);
        }
        else {
            EXPECT(column.bitfield.size() == 0);
        }

        // Test bitfields for column 10
        if (col == 9) {
            EXPECT(column.bitfield.size() == 25);

            int j               = 0;
            int expected_offset = 0;

            for (auto const& bitfield : column.bitfield) {
                const int bf = j++;

                EXPECT(bitfield.name == bitfields.at(bf));
                EXPECT(bitfield.size == bitfield_sizes.at(bf));
                EXPECT(bitfield.offset == expected_offset);

                expected_offset = expected_offset + bitfield.size;
            }
        }
    }
}

CASE("Decode data in an existing ODB file") {

    odc::api::Settings::treatIntegersAsDoubles(false);

    bool aggregated = false;
    odc::api::Reader reader("../2000010106-reduced.odb", aggregated);

    // Get the first frame
    odc::api::Frame frame = reader.next();

    // Read the second frame, because why not
    frame = reader.next();

    // Properties of this frame

    size_t ncols           = frame.columnCount();
    size_t nrows           = frame.rowCount();
    const auto& columnInfo = frame.columnInfo();

    EXPECT(nrows == 10000);
    EXPECT(ncols == 51);

    // Determine storage requirements

    size_t row_size = 0;
    size_t i;

    for (i = 0; i < frame.columnCount(); ++i) {
        const auto& col(columnInfo[i]);
        row_size += col.decodedSize;
    }

    // Allocate storage required

    size_t storage_size = row_size * nrows;
    std::vector<char> buffer(storage_size);

    // Decoder prerequisites

    std::vector<std::string> columns;
    std::vector<odc::api::StridedData> strides;

    char* ptr = &buffer[0];
    for (const auto& col : columnInfo) {
        columns.push_back(col.name);
        strides.emplace_back(odc::api::StridedData{ptr, nrows, col.decodedSize, col.decodedSize});
        ptr += nrows * col.decodedSize;
    }

    EXPECT(ptr == (&buffer[0] + storage_size));

    // Decode the data

    int nthreads = 4;
    odc::api::Decoder decoder(columns, strides);
    decoder.decode(frame, nthreads);

    std::vector<int64_t> expected_seqno{(int64_t)6106691, (int64_t)6002945, (int64_t)6003233, (int64_t)6105819};

    std::vector<int64_t> expected_obschar{
        (int64_t)537918674,
        (int64_t)135265490,
        (int64_t)135265490,
        (int64_t)537918674,
    };

    std::vector<double> expected_lat{
        (double)0.370279,
        (double)0.226484,
        (double)0.105947,
        (double)-0.0300668,
    };

    const int width = 8;

    long integer_missing  = odc::api::Settings::integerMissingValue();
    double double_missing = odc::api::Settings::doubleMissingValue();

    size_t row;

    for (i = 0; i < 4; ++i) {
        row = i * 765;

        // expver@desc (ODC_STRING, col 1)
        EXPECT(std::string(strides[0][row], ::strnlen(strides[0][row], columnInfo[0].decodedSize)).substr(0, 4) ==
               "0018");

        // seqno@hdr (ODC_INTEGER, col 4)
        EXPECT(*reinterpret_cast<const int64_t*>(strides[3][row]) == expected_seqno.at(i));

        // obschar@hdr (ODC_BITFIELD, col 6)
        EXPECT(*reinterpret_cast<const int64_t*>(strides[5][row]) == expected_obschar.at(i));

        // sortbox@hdr (ODC_INTEGER, col 14, missing value!)
        EXPECT(*reinterpret_cast<const int64_t*>(strides[13][row]) == integer_missing);

        // lat@hdr (ODC_REAL, col 18)
        EXPECT(eckit::types::is_approximately_equal(*reinterpret_cast<const double*>(strides[17][row]),
                                                    expected_lat.at(i), 0.000001));

        // repres_error@errstat (ODC_REAL, col 49, missing value!)
        EXPECT(*reinterpret_cast<const double*>(strides[48][row]) == double_missing);
    }
}

// ------------------------------------------------------------------------------------------------------

CASE("Where the properties in the two frames are distinct (non-aggregated)") {

    test_generate_odb_properties("properties-1.odb", 1);

    bool aggregated = false;
    odc::api::Reader reader("properties-1.odb", aggregated);

    odc::api::Frame frame;
    size_t nframes = 0;
    std::map<std::string, std::string> seenProperties;

    while ((frame = reader.next())) {
        for (const auto& property : frame.properties()) {
            if (nframes == 0) {
                EXPECT(property.first == "foo" || property.first == "encoder");
                EXPECT(property.second == "bar" ||
                       property.second == std::string("odc version ") + odc::api::Settings::version());
            }
            else {
                EXPECT(property.first == "baz" || property.first == "encoder");
                EXPECT(property.second == "qux" ||
                       property.second == std::string("odc version ") + odc::api::Settings::version());
            }
            seenProperties[property.first] = property.second;
        }
        ++nframes;
    }

    EXPECT(nframes == 2);
    EXPECT(seenProperties.size() == 3);
}

CASE("Where the properties in the two frames are distinct (aggregated)") {

    ASSERT(test_check_file_exists("properties-1.odb"));

    bool aggregated = true;
    odc::api::Reader reader("properties-1.odb", aggregated);

    odc::api::Frame frame;
    size_t nframes = 0;
    std::map<std::string, std::string> seenProperties;

    while ((frame = reader.next())) {
        for (const auto& property : frame.properties()) {
            seenProperties[property.first] = property.second;
        }
        ++nframes;
    }

    EXPECT(nframes == 1);
    EXPECT(seenProperties.size() == 3);

    for (const auto& property : seenProperties) {
        EXPECT(property.first == "foo" || property.first == "baz" || property.first == "encoder");
        EXPECT(property.second == "bar" || property.second == "qux" ||
               property.second == std::string("odc version ") + odc::api::Settings::version());
    }
}

CASE("Where the properties in the two frames overlap with entries that are the same (non-aggregated)") {

    test_generate_odb_properties("properties-2.odb", 2);

    bool aggregated = false;
    odc::api::Reader reader("properties-2.odb", aggregated);

    odc::api::Frame frame;
    size_t nframes = 0;
    std::map<std::string, std::string> seenProperties;

    while ((frame = reader.next())) {
        for (const auto& property : frame.properties()) {
            if (nframes == 0) {
                EXPECT(property.first == "foo" || property.first == "baz" || property.first == "encoder");
                EXPECT(property.second == "bar" || property.second == "qux" ||
                       property.second == std::string("odc version ") + odc::api::Settings::version());
            }
            else {
                EXPECT(property.first == "foo" || property.first == "baz" || property.first == "encoder");
                EXPECT(property.second == "bar" || property.second == "qux" ||
                       property.second == std::string("odc version ") + odc::api::Settings::version());
            }
            seenProperties[property.first] = property.second;
        }
        ++nframes;
    }

    EXPECT(nframes == 2);
    EXPECT(seenProperties.size() == 3);
}

CASE("Where the properties in the two frames overlap with entries that are the same (aggregated)") {

    ASSERT(test_check_file_exists("properties-2.odb"));

    bool aggregated = true;
    odc::api::Reader reader("properties-2.odb", aggregated);

    odc::api::Frame frame;
    size_t nframes = 0;
    std::map<std::string, std::string> seenProperties;

    while ((frame = reader.next())) {
        for (const auto& property : frame.properties()) {
            seenProperties[property.first] = property.second;
        }
        ++nframes;
    }

    EXPECT(nframes == 1);
    EXPECT(seenProperties.size() == 3);

    for (const auto& property : seenProperties) {
        EXPECT(property.first == "foo" || property.first == "baz" || property.first == "encoder");
        EXPECT(property.second == "bar" || property.second == "qux" ||
               property.second == std::string("odc version ") + odc::api::Settings::version());
    }
}

CASE("Where the properties overlap with entries whose keys are the same, but the values different (non-aggregated)") {

    test_generate_odb_properties("properties-3.odb", 3);

    bool aggregated = false;
    odc::api::Reader reader("properties-3.odb", aggregated);

    odc::api::Frame frame;
    size_t nframes = 0;
    std::map<std::string, std::string> seenProperties;

    while ((frame = reader.next())) {
        for (const auto& property : frame.properties()) {
            if (nframes == 0) {
                EXPECT(property.first == "foo" || property.first == "encoder");
                EXPECT(property.second == "bar" ||
                       property.second == std::string("odc version ") + odc::api::Settings::version());
            }
            else {
                EXPECT(property.first == "foo" || property.first == "encoder");
                EXPECT(property.second == "baz" ||
                       property.second == std::string("odc version ") + odc::api::Settings::version());
            }
            seenProperties[property.first] = property.second;
        }
        ++nframes;
    }

    EXPECT(nframes == 2);
    EXPECT(seenProperties.size() == 2);
}

CASE("Where the properties overlap with entries whose keys are the same, but the values different (aggregated)") {

    ASSERT(test_check_file_exists("properties-3.odb"));

    bool aggregated = true;
    odc::api::Reader reader("properties-3.odb", aggregated);

    odc::api::Frame frame;
    size_t nframes = 0;
    std::map<std::string, std::string> seenProperties;

    while ((frame = reader.next())) {
        for (const auto& property : frame.properties()) {
            seenProperties[property.first] = property.second;
        }
        ++nframes;
    }

    EXPECT(nframes == 1);
    EXPECT(seenProperties.size() == 2);

    for (const auto& property : seenProperties) {
        EXPECT(property.first == "foo" || property.first == "encoder");
        EXPECT(property.second == "bar"  // value from the first frame will win!
               || property.second == std::string("odc version ") + odc::api::Settings::version());
    }
}

// ------------------------------------------------------------------------------------------------------

class TestVisitor : public odc::api::SpanVisitor {
public:

    TestVisitor(int frame, bool mustBeConstant) : frame_(frame), mustBeConstant_(mustBeConstant) {};

private:

    template <typename T>
    void test(const std::string& columnName, const std::set<T>& vals) {
        if (mustBeConstant_) {
            ASSERT(vals.size() == 1);
        }
        else {
            if (columnName == "obsvalue@body")
                ASSERT(vals.size() == 10);
            else
                ASSERT(vals.size() == 1);
        }

        std::stringstream val;
        val << *vals.begin();

        switch (frame_) {
            case 0:
                if (columnName == "expver") {
                    ASSERT(val.str() == "xxxx");
                }
                else if (columnName == "date@hdr") {
                    ASSERT(val.str() == "20210527");
                }
                else if (columnName == "obsvalue@body") {
                    int i = 0;
                    for (auto obsvalue : vals) {
                        std::stringstream obsvalue_stream;
                        obsvalue_stream << obsvalue;
                        std::stringstream expvalue_stream;
                        expvalue_stream << 12.3456 * i++;
                        ASSERT(obsvalue_stream.str() == expvalue_stream.str());
                    }
                }
                else if (columnName == "missing_value") {
                    std::stringstream expvalue_stream;
                    expvalue_stream << odc::api::Settings::doubleMissingValue();
                    ASSERT(val.str() == expvalue_stream.str());
                }
                break;
            case 1:
                if (columnName == "expver") {
                    ASSERT(val.str() == "xxxx");
                }
                else if (columnName == "date@hdr") {
                    ASSERT(val.str() == "20210528");
                }
                else if (columnName == "obsvalue@body") {
                    int i = 0;
                    for (auto obsvalue : vals) {
                        std::stringstream obsvalue_stream;
                        obsvalue_stream << obsvalue;
                        std::stringstream expvalue_stream;
                        expvalue_stream << 12.3456 * i++;
                        ASSERT(obsvalue_stream.str() == expvalue_stream.str());
                    }
                }
                break;
            case 2:
                if (columnName == "expver") {
                    ASSERT(val.str() == "xxxx");
                }
                else if (columnName == "date@hdr") {
                    ASSERT(val.str() == "20210529");
                }
                else if (columnName == "obsvalue@body") {
                    int i = 0;
                    for (auto obsvalue : vals) {
                        std::stringstream obsvalue_stream;
                        obsvalue_stream << obsvalue;
                        std::stringstream expvalue_stream;
                        expvalue_stream << 12.3456 * i++;
                        ASSERT(obsvalue_stream.str() == expvalue_stream.str());
                    }
                }
                break;
        }
    }

    void operator()(const std::string& columnName, const std::set<long>& vals) override { test(columnName, vals); }
    void operator()(const std::string& columnName, const std::set<double>& vals) override { test(columnName, vals); }
    void operator()(const std::string& columnName, const std::set<std::string>& vals) override {
        test(columnName, vals);
    }

    int frame_;
    bool mustBeConstant_;
};

CASE("Where Span interface is used with constant value constraint") {

    test_generate_odb_span("span-1.odb");

    // Define columns with constant values
    std::vector<std::string> cols{"expver", "date@hdr"};

    // Parse frames in non-aggregated mode
    bool aggregated = false;

    // Enforce the constant values constraint
    bool mustBeConstant = true;

    {
        odc::api::Reader reader("span-1.odb", aggregated);

        odc::api::Frame frame;
        odc::api::Span lastSpan;
        long offset = 0;
        long length = 0;
        bool first  = true;
        int i       = 0;

        // Iterate over frames
        while ((frame = reader.next())) {

            // Get values for the frame
            odc::api::Span span = frame.span(cols, mustBeConstant);

            // If the values are the same, just increase the length
            if (span == lastSpan || first) {
                length += span.length();

                // Remember the first set of values
                if (first)
                    std::swap(lastSpan, span);
            }

            // If the values differ, output the last set
            else {
                TestVisitor v(i, mustBeConstant);
                lastSpan.visit(v);

                EXPECT(offset == i++ * 453);
                EXPECT(length == 453);

                // Reset offset and length counters
                offset = span.offset();
                length = span.length();

                // Remember the current set of values
                std::swap(lastSpan, span);
            }

            first = false;
        }

        TestVisitor v(i, mustBeConstant);
        lastSpan.visit(v);

        EXPECT(offset == 906);
        EXPECT(length == 453);
    }

    // Add a non-constant value column to the list
    cols.emplace_back("obsvalue@body");

    {
        odc::api::Reader reader("span-1.odb", aggregated);

        odc::api::Frame frame;

        // Iterate over frames
        while ((frame = reader.next())) {

            // Try to get values for the frame
            try {
                odc::api::Span span = frame.span(cols, mustBeConstant);
            }

            // A user error is raised when constant value constraint is not met
            catch (eckit::Exception e) {
                EXPECT(std::string(e.what()) == "UserError: Non-constant columns required in span: [obsvalue@body]");
            }
        }
    }
}

CASE("Where Span interface is used without constant value constraint") {

    ASSERT(test_check_file_exists("span-1.odb"));

    // Define columns
    std::vector<std::string> cols{"expver", "date@hdr", "obsvalue@body"};

    // Parse frames in non-aggregated mode
    bool aggregated = false;

    // Do not enforce the constant values constraint
    bool mustBeConstant = false;

    odc::api::Reader reader("span-1.odb", aggregated);

    odc::api::Frame frame;
    odc::api::Span lastSpan;
    long offset = 0;
    long length = 0;
    bool first  = true;
    int i       = 0;

    // Iterate over frames
    while ((frame = reader.next())) {

        // Get values for the frame
        odc::api::Span span = frame.span(cols, mustBeConstant);

        // If the values are the same, just increase the length
        if (span == lastSpan || first) {
            length += span.length();

            // Remember the first set of values
            if (first)
                std::swap(lastSpan, span);
        }

        // If the values differ, output the last set
        else {
            TestVisitor v(i, mustBeConstant);
            lastSpan.visit(v);

            EXPECT(offset == i++ * 453);
            EXPECT(length == 453);

            // Reset offset and length counters
            offset = span.offset();
            length = span.length();

            // Remember the current set of values
            std::swap(lastSpan, span);
        }

        first = false;
    }

    TestVisitor v(i, mustBeConstant);
    lastSpan.visit(v);

    EXPECT(offset == 906);
    EXPECT(length == 453);
}

CASE("Where Span interface is used with a missing column") {

    ASSERT(test_check_file_exists("span-1.odb"));

    // Define columns and include a missing one
    std::vector<std::string> cols{"expver", "date@hdr", "foo@bar"};

    // Parse frames in non-aggregated mode
    bool aggregated = false;

    // Do not enforce the constant values constraint
    bool mustBeConstant = false;

    odc::api::Reader reader("span-1.odb", aggregated);

    odc::api::Frame frame;

    // Iterate over frames
    while ((frame = reader.next())) {

        // Try to get values for the frame
        try {
            odc::api::Span span = frame.span(cols, mustBeConstant);
        }

        // A user error is raised when specified column cannot be found
        catch (eckit::Exception e) {
            EXPECT(std::string(e.what()) == "UserError: Column 'foo@bar' not found.");
        }
    }
}

CASE("Where Span interface is used with no columns specified") {

    ASSERT(test_check_file_exists("span-1.odb"));

    // Define empty list of columns
    std::vector<std::string> cols{};

    // Parse frames in non-aggregated mode
    bool aggregated = false;

    // Do not enforce the constant values constraint
    bool mustBeConstant = false;

    odc::api::Reader reader("span-1.odb", aggregated);

    odc::api::Frame frame;
    odc::api::Span lastSpan;
    long offset = 0;
    long length = 0;
    bool first  = true;
    int i       = 0;

    // Iterate over frames
    while ((frame = reader.next())) {

        // Get values for the frame
        odc::api::Span span = frame.span(cols, mustBeConstant);

        // If the values are the same, just increase the length
        if (span == lastSpan || first) {
            length += span.length();

            // Remember the first set of values
            if (first)
                std::swap(lastSpan, span);
        }

        // If the values differ, output the last set
        else {
            TestVisitor v(i++, mustBeConstant);
            lastSpan.visit(v);

            EXPECT(offset == 0);
            EXPECT(length == 0);

            // Reset offset and length counters
            offset = span.offset();
            length = span.length();

            // Remember the current set of values
            std::swap(lastSpan, span);
        }

        first = false;
    }

    TestVisitor v(i, mustBeConstant);
    lastSpan.visit(v);

    EXPECT(offset == 0);
    EXPECT(length == 1359);
}

CASE("Where Span interface is used with all columns specified") {

    ASSERT(test_check_file_exists("span-1.odb"));

    // Define list of columns with all of them
    std::vector<std::string> cols{"expver", "date@hdr", "obsvalue@body", "missing_value"};

    // Parse frames in non-aggregated mode
    bool aggregated = false;

    // Do not enforce the constant values constraint
    bool mustBeConstant = false;

    odc::api::Reader reader("span-1.odb", aggregated);

    odc::api::Frame frame;
    odc::api::Span lastSpan;
    long offset = 0;
    long length = 0;
    bool first  = true;
    int i       = 0;

    // Iterate over frames
    while ((frame = reader.next())) {

        // Get values for the frame
        odc::api::Span span = frame.span(cols, mustBeConstant);

        // If the values are the same, just increase the length
        if (span == lastSpan || first) {
            length += span.length();

            // Remember the first set of values
            if (first)
                std::swap(lastSpan, span);
        }

        // If the values differ, output the last set
        else {
            TestVisitor v(i, mustBeConstant);
            lastSpan.visit(v);

            EXPECT(offset == 453 * i++);
            EXPECT(length == 453);

            // Reset offset and length counters
            offset = span.offset();
            length = span.length();

            // Remember the current set of values
            std::swap(lastSpan, span);
        }

        first = false;
    }

    TestVisitor v(i, mustBeConstant);
    lastSpan.visit(v);

    EXPECT(offset == 906);
    EXPECT(length == 453);
}

CASE("Where Span interface is used with missing values") {

    ASSERT(test_check_file_exists("span-1.odb"));

    // Add missing value column to the list
    std::vector<std::string> cols{"missing_value"};

    // Parse frames in non-aggregated mode
    bool aggregated = false;

    // Enforce the constant values constraint
    bool mustBeConstant = false;

    odc::api::Reader reader("span-1.odb", aggregated);

    odc::api::Frame frame;
    odc::api::Span lastSpan;
    long offset = 0;
    long length = 0;
    bool first  = true;
    int i       = 0;

    // Iterate over frames
    while ((frame = reader.next())) {

        // Get values for the frame
        odc::api::Span span = frame.span(cols, mustBeConstant);

        // If the values are the same, just increase the length
        if (span == lastSpan || first) {
            length += span.length();

            // Remember the first set of values
            if (first)
                std::swap(lastSpan, span);
        }

        // If the values differ, output the last set
        else {
            TestVisitor v(i++, mustBeConstant);
            lastSpan.visit(v);

            EXPECT(offset == 0);
            EXPECT(length == 0);

            // Reset offset and length counters
            offset = span.offset();
            length = span.length();

            // Remember the current set of values
            std::swap(lastSpan, span);
        }

        first = false;
    }

    TestVisitor v(i, mustBeConstant);
    lastSpan.visit(v);

    EXPECT(offset == 0);
    EXPECT(length == 1359);
}

CASE("Where Span interface is used to read values without decoding") {

    ASSERT(test_check_file_exists("span-1.odb"));

    // Add columns of all three types to the list
    std::vector<std::string> cols{"expver", "date@hdr", "obsvalue@body"};

    // Parse frames in aggregated mode
    bool aggregated = true;

    // Do not enforce the constant values constraint
    bool mustBeConstant = false;

    odc::api::Reader reader("span-1.odb", aggregated);

    odc::api::Frame frame;
    std::set<std::string> expver_vals;
    std::set<long> date_vals;
    std::set<double> obsvalue_vals;

    // Iterate over frames
    while ((frame = reader.next())) {

        // Get values for the frame
        odc::api::Span span = frame.span(cols, mustBeConstant);

        expver_vals   = span.getStringValues("expver");
        date_vals     = span.getIntegerValues("date@hdr");
        obsvalue_vals = span.getRealValues("obsvalue@body");
    }

    // Check string values
    for (const std::string val : expver_vals) {
        EXPECT(val == "xxxx");
    }

    std::vector<long> expdate_vals = {20210527, 20210528, 20210529};
    int i                          = 0;

    // Check integer values
    for (const long val : date_vals) {
        EXPECT(val == expdate_vals[i++]);
    }

    i = 0;

    // Check real values
    for (const double val : obsvalue_vals) {
        EXPECT(eckit::types::is_approximately_equal(val, 12.3456 * i++, 0.0001));
    }
}

// ------------------------------------------------------------------------------------------------------

CASE("Filter a subset of ODB-2 data") {

    eckit::FileHandle in("../2000010106-reduced.odb");
    in.openForRead();
    eckit::AutoClose close_in(in);

    const eckit::Length length;

    eckit::FileHandle out("2000010106-filtered.odb");
    out.openForWrite(length);
    eckit::AutoClose close_out(out);

    // Create a new file with a subset of data
    odc::api::filter("select expver, date, lat, lon, obsvalue where rownumber() <= 10", in, out);

    EXPECT(test_check_file_exists("2000010106-filtered.odb"));

    eckit::FileHandle fh("2000010106-filtered.odb");
    fh.openForRead();
    eckit::AutoClose close_fh(fh);

    // Access file via data handle reference, in order to test alternate constructor
    eckit::DataHandle* dh = fh.clone();

    bool aggregated = true;
    odc::api::Reader reader(dh, aggregated);

    size_t nframes        = 0;
    size_t totalRows      = 0;
    odc::api::Frame frame = reader.next();

    // Test second type of frame constructor via an existing copy
    odc::api::Frame test_frame(frame);

    EXPECT(test_frame.rowCount() == 10);

    // Access encoded data directly from a frame
    EXPECT(test_frame.encodedData().size() == 487);

    // Check if frame has expected columns.
    EXPECT(test_frame.columnCount() == 5);
    EXPECT(test_frame.hasColumn("expver"));
    EXPECT(test_frame.hasColumn("date@hdr"));
    EXPECT(test_frame.hasColumn("lat@hdr"));
    EXPECT(test_frame.hasColumn("lon@hdr"));
    EXPECT(test_frame.hasColumn("obsvalue@body"));

    // Check expected frame bounds
    EXPECT(int(test_frame.offset()) == 0);
    EXPECT(long(test_frame.length()) == 487);

    // Duplicate current frame via the SQL filter function
    odc::api::Frame sub_frame(test_frame.filter("select *"));

    // Check that frames are indeed identical
    EXPECT(sub_frame.rowCount() == 10);
    EXPECT(sub_frame.columnCount() == 5);
    EXPECT(sub_frame.hasColumn("expver"));
    EXPECT(sub_frame.hasColumn("date@hdr"));
    EXPECT(sub_frame.hasColumn("lat@hdr"));
    EXPECT(sub_frame.hasColumn("lon@hdr"));
    EXPECT(sub_frame.hasColumn("obsvalue@body"));
    EXPECT(int(sub_frame.offset()) == 0);
    EXPECT(long(sub_frame.length()) == 487);

    // Test creation of a frame via the assignment operator
    odc::api::Frame dummy_frame = frame;

    EXPECT(dummy_frame.rowCount() == 10);
}

// ------------------------------------------------------------------------------------------------------

// CASE("Decode an entire ODB file") {
//
//     odc::api::Odb o("../2000010106-reduced.odb");
//
//     size_t ntables = 0;
//
//     while (const auto& table = o.next()) {
//
//         DecodeTarget decoded;
//         table.get().decode();
//
//         EXPECT(decoded.rows() == table.get().rowCount());
//         EXPECT(decoded.columns() == 51);
//
//         ++ntables;
//     }
// }
//
//// ------------------------------------------------------------------------------------------------------
//
// CASE("Decode only some columns") {
//
//    odc::api::Odb o("../2000010106-reduced.odb");
//
//    size_t ntables = 0;
//
//    while (const auto& table = o.next()) {
//
//        DecodeTarget decoded;
//        decoded.addColumn("statid");
//        decoded.addColumn("expver");
//        decoded.addColumn("andate");
//        decoded.addColumn("obsvalue");
//
//        DecodeTarget decoded = table.get().decode();
//
//        EXPECT(decoded.rows() == table.get().rowCount());
//        EXPECT(decoded.columns() == 51);
//
//        ++ntables;
//    }
//}
// ------------------------------------------------------------------------------------------------------
//
// CASE("Decode an entire ODB file preallocated data structures") {
//
//    std::unique_ptr<odb_t> o(odc_open_for_read("../2000010106-reduced.odb"));
//
//    int ntables = odc_num_tables(o.get());
//    EXPECT(ntables == 5);
//
//    odb_decoded_t decoded;
//    odb_strided_data_t strided_data[51];
//
//    for (int i = 0; i < ntables; i++) {
//
//        std::unique_ptr<odb_table_t> table(odc_get_table(o.get(), i));
//
//        ASSERT(odc_table_num_columns(table.get()) == 51);
//
//        decoded.ncolumns = 51;
//        decoded.nrows = 10000;
//        decoded.columnData = strided_data;
//
//        ///   odc_table_decode(table.get(), &decoded);
//
//        ///EXPECT(decoded.nrows == odc_table_num_rows(table.get()));
//        ///EXPECT(decoded.ncolumns == 51);
//
//        ///eckit::Log::info() << "Decoded: ncolumns = " << decoded.ncolumns << std::endl;
//        ///eckit::Log::info() << "Decoded: nrows = " << decoded.nrows << std::endl;
//        ///eckit::Log::info() << "Decoded: data = " << decoded.columnData << std::endl;
//    }
// }
//
//// ------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
