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

    size_t i;

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
    CHECK_RETURN(odc_open_path(&reader, "../2000010106-reduced.odb"));
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
    EXPECT(ntables == 5);
    EXPECT(totalRows == 50000);
}

CASE("Check column details in an existing ODB file") {

    char example_column_names[][24] = {
        "expver@desc", "andate@desc", "antime@desc", "seqno@hdr", "obstype@hdr", "obschar@hdr", "subtype@hdr",
        "date@hdr", "time@hdr", "rdbflag@hdr", "status@hdr", "event1@hdr", "blacklist@hdr", "sortbox@hdr",
        "sitedep@hdr", "statid@hdr", "ident@hdr", "lat@hdr", "lon@hdr", "stalt@hdr", "modoro@hdr", "trlat@hdr",
        "trlon@hdr", "instspec@hdr", "event2@hdr", "anemoht@hdr", "baroht@hdr", "sensor@hdr", "numlev@hdr",
        "varno_presence@hdr", "varno@body", "vertco_type@body", "rdbflag@body", "anflag@body", "status@body",
        "event1@body", "blacklist@body", "entryno@body", "press@body", "press_rl@body", "obsvalue@body", "aux1@body",
        "event2@body", "ppcode@body", "level@body", "biascorr@body", "final_obs_error@errstat", "obs_error@errstat",
        "repres_error@errstat", "pers_error@errstat", "fg_error@errstat",
    };

    int example_column_types[] = {
        ODC_STRING, ODC_INTEGER, ODC_INTEGER, ODC_INTEGER, ODC_INTEGER, ODC_BITFIELD, ODC_INTEGER, ODC_INTEGER,
        ODC_INTEGER, ODC_BITFIELD, ODC_BITFIELD, ODC_BITFIELD, ODC_BITFIELD, ODC_INTEGER, ODC_INTEGER, ODC_STRING,
        ODC_INTEGER, ODC_REAL, ODC_REAL, ODC_REAL, ODC_REAL, ODC_REAL, ODC_REAL, ODC_INTEGER, ODC_INTEGER, ODC_REAL,
        ODC_REAL, ODC_INTEGER, ODC_INTEGER, ODC_BITFIELD, ODC_INTEGER, ODC_INTEGER, ODC_BITFIELD, ODC_BITFIELD,
        ODC_BITFIELD, ODC_BITFIELD, ODC_BITFIELD, ODC_INTEGER, ODC_REAL, ODC_REAL, ODC_REAL, ODC_REAL, ODC_INTEGER,
        ODC_INTEGER, ODC_BITFIELD, ODC_REAL, ODC_REAL, ODC_REAL, ODC_REAL, ODC_REAL, ODC_REAL,
    };

    char column_10_bitfield_names[][15] = {
        "lat_humon", "lat_qcsub", "lat_override", "lat_flag", "lat_hqc_flag", "lon_humon", "lon_qcsub", "lon_override",
        "lon_flag", "lon_hqc_flag", "date_humon", "date_qcsub", "date_override", "date_flag", "date_hqc_flag",
        "time_humon", "time_qcsub", "time_override", "time_flag", "time_hqc_flag", "stalt_humon", "stalt_qcsub",
        "stalt_override", "stalt_flag", "stalt_hqc_flag",
    };

    int column_10_bitfield_sizes[] = {
        1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1,
    };

    odc_reader_t* reader = nullptr;
    CHECK_RETURN(odc_open_path(&reader, "../2000010106-reduced.odb"));
    std::unique_ptr<odc_reader_t> reader_deleter(reader);

    odc_frame_t* frame = nullptr;
    CHECK_RETURN(odc_new_frame(&frame, reader));
    std::unique_ptr<odc_frame_t> frame_deleter(frame);

    // Get the first frame
    CHECK_RETURN(odc_next_frame(frame));

    int ncols;
    CHECK_RETURN(odc_frame_column_count(frame, &ncols));
    EXPECT(ncols == 51);

    int col;

    for (col = 0; col < ncols; ++col) {
        const char* name;
        int type;
        int element_size;
        int bitfield_count;

        CHECK_RETURN(odc_frame_column_attributes(frame, col, &name, &type, &element_size, &bitfield_count));

        EXPECT(strcmp(name, example_column_names[col]) == 0);
        EXPECT(type == example_column_types[col]);
        EXPECT(element_size == 8);

        if (type == ODC_BITFIELD) {
            EXPECT(bitfield_count > 0);
        }
        else {
            EXPECT(bitfield_count == 0);
        }

        // Test bitfields for column 10
        if (col == 9) {
            EXPECT(bitfield_count == 25);

            int bf;
            int expected_offset = 0;

            for (bf = 0; bf < bitfield_count; ++bf) {
                const char* bf_name;
                int bf_offset;
                int bf_size;

                CHECK_RETURN(odc_frame_bitfield_attributes(frame, col, bf, &bf_name, &bf_offset, &bf_size));

                EXPECT(strcmp(bf_name, column_10_bitfield_names[bf]) == 0);
                EXPECT(bf_size == column_10_bitfield_sizes[bf]);
                EXPECT(bf_offset == expected_offset);

                expected_offset = expected_offset + bf_size;
            }
        }
    }
}

CASE("Decode data in an existing ODB file") {

    CHECK_RETURN(odc_integer_behaviour(ODC_INTEGERS_AS_LONGS));

    odc_reader_t* reader = nullptr;
    CHECK_RETURN(odc_open_path(&reader, "../2000010106-reduced.odb"));
    std::unique_ptr<odc_reader_t> reader_deleter(reader);

    odc_frame_t* frame = nullptr;
    CHECK_RETURN(odc_new_frame(&frame, reader));
    std::unique_ptr<odc_frame_t> frame_deleter(frame);

    size_t ntables = 0;
    size_t totalRows = 0;

    // Get the first frame
    CHECK_RETURN(odc_next_frame(frame));

    // Read the second frame, because why not
    CHECK_RETURN(odc_next_frame(frame));

    odc_decoder_t* decoder;
    CHECK_RETURN(odc_new_decoder(&decoder));
    EXPECT(decoder);
    std::unique_ptr<odc_decoder_t> decoder_deleter(decoder);

    CHECK_RETURN(odc_decoder_defaults_from_frame(decoder, frame));

    long nrows;
    CHECK_RETURN(odc_decode(decoder, frame, &nrows));
    EXPECT(nrows == 10000);

    long nrows2;
    CHECK_RETURN(odc_decoder_row_count(decoder, &nrows2));
    EXPECT(nrows2 == 10000);

    int ncols;
    CHECK_RETURN(odc_decoder_column_count(decoder, &ncols));
    EXPECT(ncols == 51);

    const void* data;
    long row_stride;
    bool column_major;
    CHECK_RETURN(odc_decoder_data_array(decoder, &data, &row_stride, 0, &column_major));
    EXPECT(!column_major);
    EXPECT(data != nullptr);
    EXPECT(row_stride == 51 * sizeof(double));

    const int64_t expected_seqno[] = {
        (int64_t)6106691,
        (int64_t)6002665,
        (int64_t)6162889,
        (int64_t)6162885
    };

    const int64_t expected_obschar[] = {
        (int64_t)537918674,
        (int64_t)135265490,
        (int64_t)605027538,
        (int64_t)605027538,
    };

    const double expected_lat[] = {
        (double)0.370279,
        (double)0.369519,
        (double)0.367451,
        (double)0.360161,
    };

    const int width = 8;

    long missing_integer;
    double missing_double;

    CHECK_RETURN(odc_missing_integer(&missing_integer));
    CHECK_RETURN(odc_missing_double(&missing_double));

    int i;
    int row;

    for (i = 0; i < 4; ++i) {
        row = i * 765;

        // expver@desc (ODC_STRING, col 1)
        char* expver = &((char*)data)[width * 0 + width * row];
        expver[4] = '\0';
        EXPECT(strcmp(expver, "0018") == 0);

        // seqno@hdr (ODC_INTEGER, col 4)
        EXPECT(*(const int64_t*)&((const char*)data)[width * 3 + width * row] == expected_seqno[i]);

        // obschar@hdr (ODC_BITFIELD, col 6)
        EXPECT(*(const int64_t*)&((const char*)data)[width * 5 + width * row] == expected_obschar[i]);

        // sortbox@hdr (ODC_INTEGER, col 14, missing value!)
        EXPECT(*(const int64_t*)&((const char*)data)[width * 13 + 8 * row] == missing_integer);

        // lat@hdr (ODC_REAL, col 18)
        EXPECT(eckit::types::is_approximately_equal(*(const double*)&((const char*)data)[width * 17 + 8 * row],expected_lat[i], 0.000001));

        // repres_error@errstat (ODC_REAL, col 49, missing value!)
        EXPECT(*(const double*)&((const char*)data)[width * 48 + 8 * row] == missing_double);
    }
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
//    std::unique_ptr<odb_t> o(odc_open_path("../2000010106-reduced.odb"));
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
//    std::unique_ptr<odb_t> o(odc_open_path("../2000010106-reduced.odb"));
//
//    int ntables = odc_num_frames(o.get());
//    EXPECT(ntables == 5);
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
