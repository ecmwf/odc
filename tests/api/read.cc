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

#include "odc/api/odc.h"
#include "odc/api/Odb.h"

using namespace eckit::testing;

// ------------------------------------------------------------------------------------------------------

bool test_check_file_exists(std::string file_path) {
    std::ifstream file_stream(file_path);
    return file_stream.good();
}

void test_generate_odb_properties(const std::string &path, int propertiesMode = 0) {
    odc::api::Settings::treatIntegersAsDoubles(false);

    // Define row count
    const size_t nrows = 10;

    // Allocate data array for each column
    char data0[nrows][8];
    int64_t data1[nrows];
    double data2[nrows];

    int i;

    // Set up the allocated arrays with scratch data
    for (i = 0; i < nrows; i++) {
        snprintf(data0[i], 8, "xxxx");  // expver
        data1[i] = 20210527;  // date@hdr
        data2[i] = 12.3456 * i;  // obsvalue@body
    }

    // Define all column names, their types and sizes
    std::vector<odc::api::ColumnInfo> columns = {
        {std::string("expver"), odc::api::ColumnType(odc::api::STRING), 8},
        {std::string("date@hdr"), odc::api::ColumnType(odc::api::INTEGER), sizeof(int64_t)},
        {std::string("obsvalue@body"), odc::api::ColumnType(odc::api::REAL), sizeof(double)},
    };

    // Set a custom data layout and data array for each column
    std::vector<odc::api::ConstStridedData> strides {
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
                if (i == 0) properties["foo"] = "bar";
                else {
                    properties = {};  // reset
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
                if (i == 0) properties["foo"] = "bar";
                else properties["foo"] = "baz";
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

void test_generate_odb_span(const std::string &path) {
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
        snprintf(data0_0[i], 8, "xxxx");  // expver
        data0_1[i] = 20210527;  // date@hdr
        data0_2[i] = 12.3456 * i;  // obsvalue@body
        data0_3[i] = odc::api::Settings::doubleMissingValue(); // missing_value

        snprintf(data1_0[i], 8, "xxxx");  // expver
        data1_1[i] = 20210528;  // date@hdr
        data1_2[i] = 12.3456 * i;  // obsvalue@body
        data1_3[i] = odc::api::Settings::doubleMissingValue(); // missing_value

        snprintf(data2_0[i], 8, "xxxx");  // expver
        data2_1[i] = 20210529;  // date@hdr
        data2_2[i] = 12.3456 * i;  // obsvalue@body
        data2_3[i] = odc::api::Settings::doubleMissingValue(); // missing_value
    }

    // Define all column names, their types and sizes
    std::vector<odc::api::ColumnInfo> columns = {
        {std::string("expver"), odc::api::ColumnType(odc::api::STRING), 8},
        {std::string("date@hdr"), odc::api::ColumnType(odc::api::INTEGER), sizeof(int64_t)},
        {std::string("obsvalue@body"), odc::api::ColumnType(odc::api::REAL), sizeof(double)},
        {std::string("missing_value"), odc::api::ColumnType(odc::api::REAL), sizeof(double)},
    };

    // Set a custom data layout and data array for each column and frame
    std::vector<odc::api::ConstStridedData> strides0 {
        // ptr, nrows, element_size, stride
        {data0_0, nrows, 8, 8},
        {data0_1, nrows, sizeof(int64_t), sizeof(int64_t)},
        {data0_2, nrows, sizeof(double), sizeof(double)},
        {data0_3, nrows, sizeof(double), sizeof(double)},
    };

    std::vector<odc::api::ConstStridedData> strides1 {
        // ptr, nrows, element_size, stride
        {data1_0, nrows, 8, 8},
        {data1_1, nrows, sizeof(int64_t), sizeof(int64_t)},
        {data1_2, nrows, sizeof(double), sizeof(double)},
        {data1_3, nrows, sizeof(double), sizeof(double)},
    };

    std::vector<odc::api::ConstStridedData> strides2 {
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
    odc::api::Reader reader("../2000010106.odb", aggregated);

    size_t nframes = 0;
    size_t totalRows = 0;

    odc::api::Frame frame;

    while ((frame = reader.next())) {
        totalRows += frame.rowCount();
        EXPECT(frame.columnCount() == 51);
        ++nframes;
    }

    EXPECT(nframes == 333);
    EXPECT(totalRows == 3321753);
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
                EXPECT(
                    property.first == "foo"
                    || property.first == "encoder"
                );
                EXPECT(
                    property.second == "bar"
                    || property.second == std::string("odc version ") + odc::api::Settings::version()
                );
            }
            else {
                EXPECT(
                    property.first == "baz"
                    || property.first == "encoder"
                );
                EXPECT(
                    property.second == "qux"
                    || property.second == std::string("odc version ") + odc::api::Settings::version()
                );
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
        EXPECT(
            property.first == "foo"
            || property.first == "baz"
            || property.first == "encoder"
        );
        EXPECT(
            property.second == "bar"
            || property.second == "qux"
            || property.second == std::string("odc version ") + odc::api::Settings::version()
        );
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
                EXPECT(
                    property.first == "foo"
                    || property.first == "baz"
                    || property.first == "encoder"
                );
                EXPECT(
                    property.second == "bar"
                    || property.second == "qux"
                    || property.second == std::string("odc version ") + odc::api::Settings::version()
                );
            }
            else {
                EXPECT(
                    property.first == "foo"
                    || property.first == "baz"
                    || property.first == "encoder"
                );
                EXPECT(
                    property.second == "bar"
                    || property.second == "qux"
                    || property.second == std::string("odc version ") + odc::api::Settings::version()
                );
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
        EXPECT(
            property.first == "foo"
            || property.first == "baz"
            || property.first == "encoder"
        );
        EXPECT(
            property.second == "bar"
            || property.second == "qux"
            || property.second == std::string("odc version ") + odc::api::Settings::version()
        );
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
                EXPECT(
                    property.first == "foo"
                    || property.first == "encoder"
                );
                EXPECT(
                    property.second == "bar"
                    || property.second == std::string("odc version ") + odc::api::Settings::version()
                );
            }
            else {
                EXPECT(
                    property.first == "foo"
                    || property.first == "encoder"
                );
                EXPECT(
                    property.second == "baz"
                    || property.second == std::string("odc version ") + odc::api::Settings::version()
                );
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
        EXPECT(
            property.first == "foo"
            || property.first == "encoder"
        );
        EXPECT(
            property.second == "bar"  // value from the first frame will win!
            || property.second == std::string("odc version ") + odc::api::Settings::version()
        );
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
            if (columnName == "obsvalue@body") ASSERT(vals.size() == 10);
            else ASSERT(vals.size() == 1);
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
    void operator()(const std::string& columnName, const std::set<std::string>& vals) override { test(columnName, vals); }

    int frame_;
    bool mustBeConstant_;
};

CASE("Where Span interface is used with constant value constraint") {

    test_generate_odb_span("span-1.odb");

    // Define columns with constant values
    std::vector<std::string> cols {"expver", "date@hdr"};

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
        bool first = true;
        int i = 0;

        // Iterate over frames
        while ((frame = reader.next())) {

            // Get values for the frame
            odc::api::Span span = frame.span(cols, mustBeConstant);

            // If the values are the same, just increase the length
            if (span == lastSpan || first) {
                length += span.length();

                // Remember the first set of values
                if (first) std::swap(lastSpan, span);
            }

            // If the values differ, output the last set
            else {
                TestVisitor v(i, mustBeConstant);
                lastSpan.visit(v);

                ASSERT(offset == i++ * 453);
                ASSERT(length == 453);

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

        ASSERT(offset == 906);
        ASSERT(length == 453);
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
            catch(eckit::Exception e) {
                ASSERT(std::string(e.what()) == "UserError: Non-constant columns required in span: [obsvalue@body]");
            }
        }
    }
}

CASE("Where Span interface is used without constant value constraint") {

    ASSERT(test_check_file_exists("span-1.odb"));

    // Define columns
    std::vector<std::string> cols {"expver", "date@hdr", "obsvalue@body"};

    // Parse frames in non-aggregated mode
    bool aggregated = false;

    // Do not enforce the constant values constraint
    bool mustBeConstant = false;

    odc::api::Reader reader("span-1.odb", aggregated);

    odc::api::Frame frame;
    odc::api::Span lastSpan;
    long offset = 0;
    long length = 0;
    bool first = true;
    int i = 0;

    // Iterate over frames
    while ((frame = reader.next())) {

        // Get values for the frame
        odc::api::Span span = frame.span(cols, mustBeConstant);

        // If the values are the same, just increase the length
        if (span == lastSpan || first) {
            length += span.length();

            // Remember the first set of values
            if (first) std::swap(lastSpan, span);
        }

        // If the values differ, output the last set
        else {
            TestVisitor v(i, mustBeConstant);
            lastSpan.visit(v);

            ASSERT(offset == i++ * 453);
            ASSERT(length == 453);

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

    ASSERT(offset == 906);
    ASSERT(length == 453);
}

CASE("Where Span interface is used with a missing column") {

    ASSERT(test_check_file_exists("span-1.odb"));

    // Define columns and include a missing one
    std::vector<std::string> cols {"expver", "date@hdr", "foo@bar"};

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
        catch(eckit::Exception e) {
            ASSERT(std::string(e.what()) == "UserError: Column 'foo@bar' not found.");
        }
    }
}

CASE("Where Span interface is used with no columns specified") {

    ASSERT(test_check_file_exists("span-1.odb"));

    // Define empty list of columns
    std::vector<std::string> cols {};

    // Parse frames in non-aggregated mode
    bool aggregated = false;

    // Do not enforce the constant values constraint
    bool mustBeConstant = false;

    odc::api::Reader reader("span-1.odb", aggregated);

    odc::api::Frame frame;
    odc::api::Span lastSpan;
    long offset = 0;
    long length = 0;
    bool first = true;
    int i = 0;

    // Iterate over frames
    while ((frame = reader.next())) {

        // Get values for the frame
        odc::api::Span span = frame.span(cols, mustBeConstant);

        // If the values are the same, just increase the length
        if (span == lastSpan || first) {
            length += span.length();

            // Remember the first set of values
            if (first) std::swap(lastSpan, span);
        }

        // If the values differ, output the last set
        else {
            TestVisitor v(i++, mustBeConstant);
            lastSpan.visit(v);

            ASSERT(offset == 0);
            ASSERT(length == 0);

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

    ASSERT(offset == 0);
    ASSERT(length == 1359);
}

CASE("Where Span interface is used with all columns specified") {

    ASSERT(test_check_file_exists("span-1.odb"));

    // Define list of columns with all of them
    std::vector<std::string> cols {"expver", "date@hdr", "obsvalue@body", "missing_value"};

    // Parse frames in non-aggregated mode
    bool aggregated = false;

    // Do not enforce the constant values constraint
    bool mustBeConstant = false;

    odc::api::Reader reader("span-1.odb", aggregated);

    odc::api::Frame frame;
    odc::api::Span lastSpan;
    long offset = 0;
    long length = 0;
    bool first = true;
    int i = 0;

    // Iterate over frames
    while ((frame = reader.next())) {

        // Get values for the frame
        odc::api::Span span = frame.span(cols, mustBeConstant);

        // If the values are the same, just increase the length
        if (span == lastSpan || first) {
            length += span.length();

            // Remember the first set of values
            if (first) std::swap(lastSpan, span);
        }

        // If the values differ, output the last set
        else {
            TestVisitor v(i, mustBeConstant);
            lastSpan.visit(v);

            ASSERT(offset == 453 * i++);
            ASSERT(length == 453);

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

    ASSERT(offset == 906);
    ASSERT(length == 453);
}

CASE("Where Span interface is used with missing values") {

    ASSERT(test_check_file_exists("span-1.odb"));

    // Add missing value column to the list
    std::vector<std::string> cols {"missing_value"};

    // Parse frames in non-aggregated mode
    bool aggregated = false;

    // Enforce the constant values constraint
    bool mustBeConstant = false;

    odc::api::Reader reader("span-1.odb", aggregated);

    odc::api::Frame frame;
    odc::api::Span lastSpan;
    long offset = 0;
    long length = 0;
    bool first = true;
    int i = 0;

    // Iterate over frames
    while ((frame = reader.next())) {

        // Get values for the frame
        odc::api::Span span = frame.span(cols, mustBeConstant);

        // If the values are the same, just increase the length
        if (span == lastSpan || first) {
            length += span.length();

            // Remember the first set of values
            if (first) std::swap(lastSpan, span);
        }

        // If the values differ, output the last set
        else {
            TestVisitor v(i++, mustBeConstant);
            lastSpan.visit(v);

            ASSERT(offset == 0);
            ASSERT(length == 0);

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

    ASSERT(offset == 0);
    ASSERT(length == 1359);
}

// ------------------------------------------------------------------------------------------------------

//CASE("Decode an entire ODB file") {
//
//    odc::api::Odb o("../2000010106.odb");
//
//    size_t ntables = 0;
//
//    while (const auto& table = o.next()) {
//
//        DecodeTarget decoded;
//        table.get().decode();
//
//        EXPECT(decoded.rows() == table.get().rowCount());
//        EXPECT(decoded.columns() == 51);
//
//        ++ntables;
//    }
//}
//
//// ------------------------------------------------------------------------------------------------------
//
//CASE("Decode only some columns") {
//
//    odc::api::Odb o("../2000010106.odb");
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
//    std::unique_ptr<odb_t> o(odc_open_for_read("../2000010106.odb"));
//
//    int ntables = odc_num_tables(o.get());
//    EXPECT(ntables == 333);
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
