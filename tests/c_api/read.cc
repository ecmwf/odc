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

// Specialise custom deletion for odb_t

#define CHECK_RETURN(x) EXPECT((x) == ODC_SUCCESS)

namespace std {
template <> struct default_delete<odc_reader_t> {
    void operator() (const odc_reader_t* reader) { CHECK_RETURN(odc_close(reader)); }
};

template <> struct default_delete<odc_frame_t> {
    void operator() (const odc_frame_t* frame) { CHECK_RETURN(odc_free_frame(frame)); }
};

template <> struct default_delete<odc_decoder_t> {
    void operator() (odc_decoder_t* t) { CHECK_RETURN(odc_free_decoder(t)); }
};
}

// ------------------------------------------------------------------------------------------------------

bool test_check_file_exists(std::string file_path) {
    std::ifstream file_stream(file_path);
    return file_stream.good();
}

void test_generate_odb(const std::string &path, int propertiesMode) {
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

        }

        // Encode the ODB-2 frame into a data handle
        encode(fh, columns, strides, properties);
    }

    ASSERT(test_check_file_exists(path));
}

// ------------------------------------------------------------------------------------------------------

CASE("Count lines in an existing ODB file") {

    odc_reader_t* reader = nullptr;
    CHECK_RETURN(odc_open_path(&reader, "../2000010106.odb"));
    std::unique_ptr<odc_reader_t> reader_deleter(reader);

    odc_frame_t* frame = nullptr;
    CHECK_RETURN(odc_new_frame(&frame, reader));
    std::unique_ptr<odc_frame_t> frame_deleter(frame);

    size_t ntables = 0;
    size_t totalRows = 0;

    int ierr;
    while ((ierr = odc_next_frame(frame)) == ODC_SUCCESS) {

        long nrows;
        CHECK_RETURN(odc_frame_row_count(frame, &nrows));
        totalRows += nrows;

        int ncols;
        CHECK_RETURN(odc_frame_column_count(frame, &ncols));
        EXPECT(ncols == 51);

        ++ntables;
    }

    EXPECT(ierr == ODC_ITERATION_COMPLETE);
    EXPECT(ntables == 333);
    EXPECT(totalRows == 3321753);
}

// ------------------------------------------------------------------------------------------------------

CASE("Where the properties in the two frames are distinct (non-aggregated)") {

    test_generate_odb("properties-1.odb", 1);

    odc_reader_t* reader = nullptr;
    CHECK_RETURN(odc_open_path(&reader, "properties-1.odb"));
    std::unique_ptr<odc_reader_t> reader_deleter(reader);

    odc_frame_t* frame = nullptr;
    CHECK_RETURN(odc_new_frame(&frame, reader));
    std::unique_ptr<odc_frame_t> frame_deleter(frame);

    int nproperties;
    const char* key;
    const char* value;

    const char* version;
    CHECK_RETURN(odc_version(&version));

    char odc_version_str[255] = "odc version ";
    strcat(odc_version_str, version);

    int nframes = 0;
    int rc;

    while ((rc = odc_next_frame(frame)) == ODC_SUCCESS) {
        CHECK_RETURN(odc_frame_properties_count(frame, &nproperties));
        EXPECT(nproperties == 2);

        int i;
        for (i = 0; i < 2; i++) {
            CHECK_RETURN(odc_frame_property_idx(frame, i, &key, &value));

            if (
                (i == 0 && nframes == 0)
                || (i == 1 && nframes == 1)
            ) {
                EXPECT(strcmp(key, "encoder") == 0);
                EXPECT(strcmp(value, odc_version_str) == 0);
            }
            else if (nframes == 0) {
                EXPECT(strcmp(key, "foo") == 0);
                EXPECT(strcmp(value, "bar") == 0);
            }
            else  {
                EXPECT(strcmp(key, "baz") == 0);
                EXPECT(strcmp(value, "qux") == 0);
            }
        }

        if (nframes == 0) {
            CHECK_RETURN(odc_frame_property(frame, "encoder", &value));
            EXPECT(strcmp(value, odc_version_str) == 0);

            CHECK_RETURN(odc_frame_property(frame, "foo", &value));
            EXPECT(strcmp(value, "bar") == 0);
        }
        else {
            CHECK_RETURN(odc_frame_property(frame, "encoder", &value));
            EXPECT(strcmp(value, odc_version_str) == 0);

            CHECK_RETURN(odc_frame_property(frame, "baz", &value));
            EXPECT(strcmp(value, "qux") == 0);
        }

        // Check for reading of non-existent properties
        CHECK_RETURN(odc_frame_property(frame, "non-existent", &value));
        EXPECT(value == NULL);

        ++nframes;
    }

    EXPECT(rc == ODC_ITERATION_COMPLETE);
    EXPECT(nframes == 2);
}

// ------------------------------------------------------------------------------------------------------

CASE("Where the properties in the two frames are distinct (aggregated)") {

    ASSERT(test_check_file_exists("properties-1.odb"));

    odc_reader_t* reader = nullptr;
    CHECK_RETURN(odc_open_path(&reader, "properties-1.odb"));
    std::unique_ptr<odc_reader_t> reader_deleter(reader);

    odc_frame_t* frame = nullptr;
    CHECK_RETURN(odc_new_frame(&frame, reader));
    std::unique_ptr<odc_frame_t> frame_deleter(frame);

    int nproperties;
    const char* key;
    const char* value;

    const char* version;
    CHECK_RETURN(odc_version(&version));

    char odc_version_str[255] = "odc version ";
    strcat(odc_version_str, version);

    long max_aggregated_rows = 1000000;
    int nframes = 0;
    int rc;

    while ((rc = odc_next_frame_aggregated(frame, max_aggregated_rows)) == ODC_SUCCESS) {
        CHECK_RETURN(odc_frame_properties_count(frame, &nproperties));
        EXPECT(nproperties == 3);

        int i;
        for (i = 0; i < 3; i++) {
            CHECK_RETURN(odc_frame_property_idx(frame, i, &key, &value));

            if (i == 0) {
                EXPECT(strcmp(key, "baz") == 0);
                EXPECT(strcmp(value, "qux") == 0);
            }
            else if (i == 1) {
                EXPECT(strcmp(key, "encoder") == 0);
                EXPECT(strcmp(value, odc_version_str) == 0);
            }
            else {
                EXPECT(strcmp(key, "foo") == 0);
                EXPECT(strcmp(value, "bar") == 0);
            }
        }

        CHECK_RETURN(odc_frame_property(frame, "encoder", &value));
        EXPECT(strcmp(value, odc_version_str) == 0);

        CHECK_RETURN(odc_frame_property(frame, "foo", &value));
        EXPECT(strcmp(value, "bar") == 0);

        CHECK_RETURN(odc_frame_property(frame, "baz", &value));
        EXPECT(strcmp(value, "qux") == 0);

        // Check for reading of non-existent properties
        CHECK_RETURN(odc_frame_property(frame, "non-existent", &value));
        EXPECT(value == NULL);

        ++nframes;
    }

    EXPECT(rc == ODC_ITERATION_COMPLETE);
    EXPECT(nframes == 1);
}

// ------------------------------------------------------------------------------------------------------

CASE("Where the properties in the two frames overlap with entries that are the same (non-aggregated)") {

    test_generate_odb("properties-2.odb", 2);

    odc_reader_t* reader = nullptr;
    CHECK_RETURN(odc_open_path(&reader, "properties-2.odb"));
    std::unique_ptr<odc_reader_t> reader_deleter(reader);

    odc_frame_t* frame = nullptr;
    CHECK_RETURN(odc_new_frame(&frame, reader));
    std::unique_ptr<odc_frame_t> frame_deleter(frame);

    int nproperties;
    const char* key;
    const char* value;

    const char* version;
    CHECK_RETURN(odc_version(&version));

    char odc_version_str[255] = "odc version ";
    strcat(odc_version_str, version);

    int nframes = 0;
    int rc;

    while ((rc = odc_next_frame(frame)) == ODC_SUCCESS) {
        CHECK_RETURN(odc_frame_properties_count(frame, &nproperties));
        EXPECT(nproperties == 3);

        int i;
        for (i = 0; i < 3; i++) {
            CHECK_RETURN(odc_frame_property_idx(frame, i, &key, &value));

            if (i == 0) {
                EXPECT(strcmp(key, "baz") == 0);
                EXPECT(strcmp(value, "qux") == 0);
            }
            else if (i == 1) {
                EXPECT(strcmp(key, "encoder") == 0);
                EXPECT(strcmp(value, odc_version_str) == 0);
            }
            else {
                EXPECT(strcmp(key, "foo") == 0);
                EXPECT(strcmp(value, "bar") == 0);
            }
        }

        CHECK_RETURN(odc_frame_property(frame, "encoder", &value));
        EXPECT(strcmp(value, odc_version_str) == 0);

        CHECK_RETURN(odc_frame_property(frame, "foo", &value));
        EXPECT(strcmp(value, "bar") == 0);

        CHECK_RETURN(odc_frame_property(frame, "baz", &value));
        EXPECT(strcmp(value, "qux") == 0);

        // Check for reading of non-existent properties
        CHECK_RETURN(odc_frame_property(frame, "non-existent", &value));
        EXPECT(value == NULL);

        ++nframes;
    }

    EXPECT(rc == ODC_ITERATION_COMPLETE);
    EXPECT(nframes == 2);
}

// ------------------------------------------------------------------------------------------------------

CASE("Where the properties in the two frames overlap with entries that are the same (aggregated)") {

    ASSERT(test_check_file_exists("properties-2.odb"));

    odc_reader_t* reader = nullptr;
    CHECK_RETURN(odc_open_path(&reader, "properties-2.odb"));
    std::unique_ptr<odc_reader_t> reader_deleter(reader);

    odc_frame_t* frame = nullptr;
    CHECK_RETURN(odc_new_frame(&frame, reader));
    std::unique_ptr<odc_frame_t> frame_deleter(frame);

    int nproperties;
    const char* key;
    const char* value;

    const char* version;
    CHECK_RETURN(odc_version(&version));

    char odc_version_str[255] = "odc version ";
    strcat(odc_version_str, version);

    long max_aggregated_rows = 1000000;
    int nframes = 0;
    int rc;

    while ((rc = odc_next_frame_aggregated(frame, max_aggregated_rows)) == ODC_SUCCESS) {
        CHECK_RETURN(odc_frame_properties_count(frame, &nproperties));
        EXPECT(nproperties == 3);

        int i;
        for (i = 0; i < 3; i++) {
            CHECK_RETURN(odc_frame_property_idx(frame, i, &key, &value));

            if (i == 0) {
                EXPECT(strcmp(key, "baz") == 0);
                EXPECT(strcmp(value, "qux") == 0);
            }
            else if (i == 1) {
                EXPECT(strcmp(key, "encoder") == 0);
                EXPECT(strcmp(value, odc_version_str) == 0);
            }
            else {
                EXPECT(strcmp(key, "foo") == 0);
                EXPECT(strcmp(value, "bar") == 0);
            }
        }

        CHECK_RETURN(odc_frame_property(frame, "encoder", &value));
        EXPECT(strcmp(value, odc_version_str) == 0);

        CHECK_RETURN(odc_frame_property(frame, "foo", &value));
        EXPECT(strcmp(value, "bar") == 0);

        CHECK_RETURN(odc_frame_property(frame, "baz", &value));
        EXPECT(strcmp(value, "qux") == 0);

        // Check for reading of non-existent properties
        CHECK_RETURN(odc_frame_property(frame, "non-existent", &value));
        EXPECT(value == NULL);

        ++nframes;
    }

    EXPECT(rc == ODC_ITERATION_COMPLETE);
    EXPECT(nframes == 1);
}

// ------------------------------------------------------------------------------------------------------

CASE("Where the properties overlap with entries whose keys are the same, but the values different (non-aggregated)") {

    test_generate_odb("properties-3.odb", 3);

    odc_reader_t* reader = nullptr;
    CHECK_RETURN(odc_open_path(&reader, "properties-3.odb"));
    std::unique_ptr<odc_reader_t> reader_deleter(reader);

    odc_frame_t* frame = nullptr;
    CHECK_RETURN(odc_new_frame(&frame, reader));
    std::unique_ptr<odc_frame_t> frame_deleter(frame);

    int nproperties;
    const char* key;
    const char* value;

    const char* version;
    CHECK_RETURN(odc_version(&version));

    char odc_version_str[255] = "odc version ";
    strcat(odc_version_str, version);

    int nframes = 0;
    int rc;

    while ((rc = odc_next_frame(frame)) == ODC_SUCCESS) {
        CHECK_RETURN(odc_frame_properties_count(frame, &nproperties));
        EXPECT(nproperties == 2);

        int i;
        for (i = 0; i < 2; i++) {
            CHECK_RETURN(odc_frame_property_idx(frame, i, &key, &value));

            if (i == 0) {
                EXPECT(strcmp(key, "encoder") == 0);
                EXPECT(strcmp(value, odc_version_str) == 0);
            }
            else if (nframes == 0) {
                EXPECT(strcmp(key, "foo") == 0);
                EXPECT(strcmp(value, "bar") == 0);
            }
            else  {
                EXPECT(strcmp(key, "foo") == 0);
                EXPECT(strcmp(value, "baz") == 0);
            }
        }

        if (nframes == 0) {
            CHECK_RETURN(odc_frame_property(frame, "encoder", &value));
            EXPECT(strcmp(value, odc_version_str) == 0);

            CHECK_RETURN(odc_frame_property(frame, "foo", &value));
            EXPECT(strcmp(value, "bar") == 0);
        }
        else {
            CHECK_RETURN(odc_frame_property(frame, "encoder", &value));
            EXPECT(strcmp(value, odc_version_str) == 0);

            CHECK_RETURN(odc_frame_property(frame, "foo", &value));
            EXPECT(strcmp(value, "baz") == 0);
        }

        // Check for reading of non-existent properties
        CHECK_RETURN(odc_frame_property(frame, "non-existent", &value));
        EXPECT(value == NULL);

        ++nframes;
    }

    EXPECT(rc == ODC_ITERATION_COMPLETE);
    EXPECT(nframes == 2);
}

// ------------------------------------------------------------------------------------------------------

CASE("Where the properties overlap with entries whose keys are the same, but the values different (aggregated)") {

    ASSERT(test_check_file_exists("properties-3.odb"));

    odc_reader_t* reader = nullptr;
    CHECK_RETURN(odc_open_path(&reader, "properties-3.odb"));
    std::unique_ptr<odc_reader_t> reader_deleter(reader);

    odc_frame_t* frame = nullptr;
    CHECK_RETURN(odc_new_frame(&frame, reader));
    std::unique_ptr<odc_frame_t> frame_deleter(frame);

    int nproperties;
    const char* key;
    const char* value;

    const char* version;
    CHECK_RETURN(odc_version(&version));

    char odc_version_str[255] = "odc version ";
    strcat(odc_version_str, version);

    long max_aggregated_rows = 1000000;
    int nframes = 0;
    int rc;

    while ((rc = odc_next_frame_aggregated(frame, max_aggregated_rows)) == ODC_SUCCESS) {
        CHECK_RETURN(odc_frame_properties_count(frame, &nproperties));
        EXPECT(nproperties == 2);

        int i;
        for (i = 0; i < 2; i++) {
            CHECK_RETURN(odc_frame_property_idx(frame, i, &key, &value));

            if (i == 0) {
                EXPECT(strcmp(key, "encoder") == 0);
                EXPECT(strcmp(value, odc_version_str) == 0);
            }
            else {
                EXPECT(strcmp(key, "foo") == 0);
                EXPECT(strcmp(value, "bar") == 0);  // value from the first frame will win!
            }
        }

        CHECK_RETURN(odc_frame_property(frame, "encoder", &value));
        EXPECT(strcmp(value, odc_version_str) == 0);

        CHECK_RETURN(odc_frame_property(frame, "foo", &value));
        EXPECT(strcmp(value, "bar") == 0);  // value from the first frame will win!

        // Check for reading of non-existent properties
        CHECK_RETURN(odc_frame_property(frame, "non-existent", &value));
        EXPECT(value == NULL);

        ++nframes;
    }

    EXPECT(rc == ODC_ITERATION_COMPLETE);
    EXPECT(nframes == 1);
}

// ------------------------------------------------------------------------------------------------------

//CASE("Decode an entire ODB file") {
//
//
//    std::unique_ptr<odb_t> o(odc_open_path("../2000010106.odb"));
//
//    size_t ntables = 0;
//
//    std::unique_ptr<odb_frame_t> table;
//    while (table.reset(odc_alloc_next_frame(o.get())), table) {
//
//        std::unique_ptr<const odb_decoded_t> decoded(odc_frame_decode_all(table.get()));
//        EXPECT(decoded->nrows == odc_frame_row_count(table.get()));
//        EXPECT(decoded->ncolumns == 51);
//
//        ++ntables;
//    }
//}
//
//// ------------------------------------------------------------------------------------------------------
//
//CASE("Decode an entire ODB file preallocated data structures") {
//
//    std::unique_ptr<odb_t> o(odc_open_path("../2000010106.odb"));
//
//    int ntables = odc_num_frames(o.get());
//    EXPECT(ntables == 333);
//
//    odb_decoded_t decoded;
//    odb_strided_data_t strided_data[51];
//
//    for (int i = 0; i < ntables; i++) {
//
//        std::unique_ptr<odb_frame_t> table(odc_get_frame(o.get(), i));
//
//        ASSERT(odc_frame_column_count(table.get()) == 51);
//
//        decoded.ncolumns = 51;
//        decoded.nrows = 10000;
//        decoded.columnData = strided_data;
//
//        ///   odc_frame_decode(table.get(), &decoded);
//
//        ///EXPECT(decoded.nrows == odc_frame_row_count(table.get()));
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
