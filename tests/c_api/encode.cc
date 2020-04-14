/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <memory>
#include <cstring>

// TODO: unneeded
#include <fcntl.h>
#include <unistd.h>

#include "eckit/filesystem/TmpFile.h"
#include "eckit/io/Buffer.h"
#include "eckit/testing/Test.h"

#include "odc/api/odc.h"

using namespace eckit::testing;

// Specialise custom deletion

#define CHECK_RETURN(x) EXPECT((x) == ODC_SUCCESS)

namespace std{
template <> struct default_delete<odc_encoder_t> {
    void operator() (odc_encoder_t* e) { CHECK_RETURN(odc_free_encoder(e)); }
};

template <> struct default_delete<odc_reader_t> {
    void operator() (odc_reader_t* o) { CHECK_RETURN(odc_close(o)); }
};

template <> struct default_delete<odc_frame_t> {
    void operator() (odc_frame_t* t) { CHECK_RETURN(odc_free_frame(t)); }
};

template <> struct default_delete<odc_decoder_t> {
    void operator() (odc_decoder_t* t) { CHECK_RETURN(odc_free_decoder(t)); }
};
}

// ------------------------------------------------------------------------------------------------------


// TODO: Row-major
// TODO: Column major

CASE("Encode data in standard tabular form") {

    CHECK_RETURN(odc_integer_behaviour(ODC_INTEGERS_AS_DOUBLES));

    const int nrows = 15;
    const int ncols = 8;
    double data[nrows][ncols];

    // Construct some data to encode

    for (int row = 0; row < nrows; ++row) {
        for (int col = 0; col < 4; ++col) {
            data[row][col] = (1000 * row) + (3 * col);
        }
        data[row][4] = 0;
        data[row][5] = 0;
        ::strncpy(reinterpret_cast<char*>(&data[row][4]), "abcdefghijkl", 2*sizeof(double));
        for (int col = 6; col < ncols; ++col) {
            data[row][col] = (1000 * row) + (3 * col);
        }
    }

    // Configure the encoder to encode said data

    odc_encoder_t* enc = nullptr;
    CHECK_RETURN(odc_new_encoder(&enc));
    std::unique_ptr<odc_encoder_t> enc_deleter(enc);

    bool columnMajor = false;
    CHECK_RETURN(odc_encoder_set_row_count(enc, nrows));
    CHECK_RETURN(odc_encoder_set_data_array(enc, data, ncols * sizeof(double), nrows, columnMajor));
    CHECK_RETURN(odc_encoder_add_column(enc, "col1", ODC_INTEGER));
    CHECK_RETURN(odc_encoder_add_column(enc, "col2", ODC_REAL));
    CHECK_RETURN(odc_encoder_add_column(enc, "col3", ODC_DOUBLE));
    CHECK_RETURN(odc_encoder_add_column(enc, "col4", ODC_DOUBLE));
    CHECK_RETURN(odc_encoder_add_column(enc, "col5", ODC_STRING));
    int elementSize = 2 * sizeof(double);
    CHECK_RETURN(odc_encoder_column_set_data_array(enc, 4, elementSize, 0, 0));
    CHECK_RETURN(odc_encoder_add_column(enc, "col6", ODC_REAL));
    CHECK_RETURN(odc_encoder_add_column(enc, "col7", ODC_BITFIELD));
    CHECK_RETURN(odc_encoder_column_add_bitfield(enc, 6, "bits1", 2));
    CHECK_RETURN(odc_encoder_column_add_bitfield(enc, 6, "bits2", 3));
    CHECK_RETURN(odc_encoder_column_add_bitfield(enc, 6, "bits3", 1));

    // Do the encoding

    eckit::Buffer encoded(1024 * 1024);
    long sz;
    CHECK_RETURN(odc_encode_to_buffer(enc, encoded.data(), encoded.size(), &sz));

    // Check that the table contains what we expect

    odc_reader_t* reader = nullptr;
    CHECK_RETURN(odc_open_buffer(&reader, encoded.data(), sz));
    EXPECT(reader);
    std::unique_ptr<odc_reader_t> reader_deleter(reader);

    odc_frame_t* frame = nullptr;
    CHECK_RETURN(odc_new_frame(&frame, reader));
    EXPECT(frame);
    std::unique_ptr<odc_frame_t> frame_deleter(frame);

    CHECK_RETURN(odc_next_frame(frame));

    int column_count;
    CHECK_RETURN(odc_frame_column_count(frame, &column_count));
    EXPECT(column_count == ncols-1);

    long row_count;
    CHECK_RETURN(odc_frame_row_count(frame, &row_count));
    EXPECT(row_count == nrows);

    const char* column_names[] = {"col1", "col2", "col3", "col4", "col5", "col6", "col7"};
    int column_types[] = {ODC_INTEGER, ODC_REAL, ODC_DOUBLE, ODC_DOUBLE, ODC_STRING, ODC_REAL, ODC_BITFIELD};
    const char* bitfield_names[] = {"bits1", "bits2", "bits3"};
    int bitfield_sizes[] = {2, 3, 1};
    int bitfield_offsets[] = {0, 2, 5};

    for (int col = 0; col < 7; ++col) {

        const char* name;
        int type;
        int elementSize;
        int bitfieldCount;
        CHECK_RETURN(odc_frame_column_attributes(frame, col, &name, &type, &elementSize, &bitfieldCount));
        EXPECT(name);
        EXPECT(::strcmp(name, column_names[col]) == 0);
        EXPECT(type == column_types[col]);
        EXPECT(elementSize == (col == 4 ? 2 : 1) * int(sizeof(double)));
        EXPECT(bitfieldCount == (col == 6 ? 3 : 0));

        if (col == 6) {
            for (int bf = 0; bf < 3; ++bf) {
                const char* bf_name;
                int bfSize;
                int bfOffset;
                CHECK_RETURN(odc_frame_bitfield_attributes(frame, col, bf, &bf_name, &bfOffset, &bfSize));
                EXPECT(bf_name);
                EXPECT(::strcmp(bf_name, bitfield_names[bf]) == 0);
                EXPECT(bfSize == bitfield_sizes[bf]);
                EXPECT(bfOffset == bitfield_offsets[bf]);
            }
        }
    }

    // Test that the data is correctly encoded

    odc_decoder_t* decoder;
    CHECK_RETURN(odc_new_decoder(&decoder));
    EXPECT(decoder);
    std::unique_ptr<odc_decoder_t> decoder_deleter(decoder);

    CHECK_RETURN(odc_decoder_defaults_from_frame(decoder, frame));

    long rows_decoded;
    CHECK_RETURN(odc_decode(decoder, frame, &rows_decoded));
    EXPECT(rows_decoded == nrows);

    const void* p;
    CHECK_RETURN(odc_decoder_column_data_array(decoder, 0, 0, 0, &p));
    const void* pdata;
    long row_stride;
    bool decodeColumnMajor;
    CHECK_RETURN(odc_decoder_data_array(decoder, &pdata, &row_stride, 0, &decodeColumnMajor));
    EXPECT(!decodeColumnMajor);
    EXPECT(p != nullptr);
    EXPECT(pdata != nullptr);
    EXPECT(p == pdata);
    EXPECT(row_stride == 8*sizeof(double));

    double vals1[] = {0, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000, 11000, 12000, 13000, 14000};
    double vals2[] = {3, 1003, 2003, 3003, 4003, 5003, 6003, 7003, 8003, 9003, 10003, 11003, 12003, 13003, 14003};
    double vals3[] = {6, 1006, 2006, 3006, 4006, 5006, 6006, 7006, 8006, 9006, 10006, 11006, 12006, 13006, 14006};
    double vals4[] = {9, 1009, 2009, 3009, 4009, 5009, 6009, 7009, 8009, 9009, 10009, 11009, 12009, 13009, 14009};
    double vals6[] = {18, 1018, 2018, 3018, 4018, 5018, 6018, 7018, 8018, 9018, 10018, 11018, 12018, 13018, 14018};
    double vals7[] = {21, 1021, 2021, 3021, 4021, 5021, 6021, 7021, 8021, 9021, 10021, 11021, 12021, 13021, 14021};

    const double (*row_data)[8] = reinterpret_cast<const double (*)[8]>(pdata);

    for (size_t row = 0; row < nrows; ++row) {
        EXPECT(vals1[row] == row_data[row][0]);
        EXPECT(vals2[row] == row_data[row][1]);
        EXPECT(vals3[row] == row_data[row][2]);
        EXPECT(vals4[row] == row_data[row][3]);
        EXPECT(::strncmp("abcdefghijkl", reinterpret_cast<const char*>(&row_data[row][4]), 2*sizeof(double)) == 0);
        EXPECT(vals6[row] == row_data[row][6]);
        EXPECT(vals7[row] == row_data[row][7]);
    }

    EXPECT(odc_next_frame(frame) == ODC_ITERATION_COMPLETE);
}


CASE("Encode from columnar data") {

    odc_integer_behaviour(ODC_INTEGERS_AS_LONGS);

    const int nrows = 10;
    const int ncols = 5;

    // Construct some source data

    long icol[nrows] = {1111, 2222, 3333, 4444, 5555, 6666, 7777, 8888, 9999, 0};
    long bcol[nrows] = {1101, 2202, 3303, 4404,  5505, 6606, 7707, 8808, 9909, 0};
    char scol[nrows][3 * sizeof(double)] = {0};
    double dcol[nrows] = {1131, 2232, 3333, 4434, 5535, 6636, 7737, 8838, 9939, 0};
    double rcol[nrows] = {1141, 2242, 3343, 4444, 5545, 6646, 7747, 8848, 9949, 0};
    for (size_t i = 0; i < nrows; ++i) {
        ::strncpy(&scol[i][0], "abcdefghhgfedcbazzzz", 3*sizeof(double));
    }

    // Configure the encoder to encode said data

    odc_encoder_t* enc = nullptr;
    CHECK_RETURN(odc_new_encoder(&enc));
    std::unique_ptr<odc_encoder_t> enc_deleter(enc);

    CHECK_RETURN(odc_encoder_set_row_count(enc, nrows));
    CHECK_RETURN(odc_encoder_add_column(enc, "col1", ODC_INTEGER));
    CHECK_RETURN(odc_encoder_add_column(enc, "col2", ODC_BITFIELD));
    CHECK_RETURN(odc_encoder_add_column(enc, "col3", ODC_STRING));
    CHECK_RETURN(odc_encoder_add_column(enc, "col4", ODC_DOUBLE));
    CHECK_RETURN(odc_encoder_add_column(enc, "col5", ODC_REAL));

    int elementSizeCol2 = 3 * sizeof(double);
    CHECK_RETURN(odc_encoder_column_set_data_array(enc, 0, 0, 0, icol));
    CHECK_RETURN(odc_encoder_column_set_data_array(enc, 1, 0, 0, bcol));
    CHECK_RETURN(odc_encoder_column_set_data_array(enc, 2, elementSizeCol2, 0, scol));
    CHECK_RETURN(odc_encoder_column_set_data_array(enc, 3, 0, 0, dcol));
    CHECK_RETURN(odc_encoder_column_set_data_array(enc, 4, 0, 0, rcol));

    // Do the encoding

    eckit::Buffer encoded(1024 * 1024);
    long sz;
    CHECK_RETURN(odc_encode_to_buffer(enc, encoded.data(), encoded.size(), &sz));

    // Check that we have encoded what we think we have encoded

    odc_reader_t* reader = nullptr;
    CHECK_RETURN(odc_open_buffer(&reader, encoded.data(), sz));
    EXPECT(reader);
    std::unique_ptr<odc_reader_t> reader_deleter(reader);

    odc_frame_t* frame = nullptr;
    CHECK_RETURN(odc_new_frame(&frame, reader));
    EXPECT(frame);
    std::unique_ptr<odc_frame_t> frame_deleter(frame);

    CHECK_RETURN(odc_next_frame(frame));

    int column_count;
    CHECK_RETURN(odc_frame_column_count(frame, &column_count));
    EXPECT(column_count == ncols);

    long row_count;
    CHECK_RETURN(odc_frame_row_count(frame, &row_count));
    EXPECT(row_count == nrows);

    // Test that the data is correctly encoded

    odc_decoder_t* decoder;
    CHECK_RETURN(odc_new_decoder(&decoder));
    EXPECT(decoder);
    std::unique_ptr<odc_decoder_t> decoder_deleter(decoder);

    CHECK_RETURN(odc_decoder_defaults_from_frame(decoder, frame));

    long rows_decoded;
    CHECK_RETURN(odc_decode(decoder, frame, &rows_decoded));
    EXPECT(rows_decoded == nrows);

    const void* pdata;
    long row_stride;
    bool decodeColumnMajor;
    CHECK_RETURN(odc_decoder_data_array(decoder, &pdata, &row_stride, 0, &decodeColumnMajor));
    EXPECT(!decodeColumnMajor);
    EXPECT(pdata != nullptr);
    EXPECT(row_stride == 7*sizeof(double));

    const double (*row_data)[7] = reinterpret_cast<const double (*)[7]>(pdata);

    for (size_t row = 0; row < nrows; ++row) {
        EXPECT(reinterpret_cast<const long&>(row_data[row][0]) == icol[row]);
        EXPECT(reinterpret_cast<const long&>(row_data[row][1]) == bcol[row]);
        EXPECT(::strncmp("abcdefghhgfedcbazzzz", reinterpret_cast<const char*>(&row_data[row][2]), 3*sizeof(double)) == 0);
        EXPECT(row_data[row][5] == dcol[row]);
        EXPECT(row_data[row][6] == rcol[row]);
    }

    EXPECT(odc_next_frame(frame) == ODC_ITERATION_COMPLETE);
}

//// ------------------------------------------------------------------------------------------------------

CASE("Encode data with custom stride") {

    odc_integer_behaviour(ODC_INTEGERS_AS_LONGS);

    const int nrows = 5;
    const int ncols = 5;

    // Construct some source data

    long icol[2*nrows] = {1111, 2222, 3333, 4444, 5555, 6666, 7777, 8888, 9999, 0};
    long bcol[2*nrows] = {1101, 2202, 3303, 4404,  5505, 6606, 7707, 8808, 9909, 0};
    char scol[2*nrows][3 * sizeof(double)] = {0};
    double dcol[2*nrows] = {1131, 2232, 3333, 4434, 5535, 6636, 7737, 8838, 9939, 0};
    double rcol[2*nrows] = {1141, 2242, 3343, 4444, 5545, 6646, 7747, 8848, 9949, 0};
    for (size_t i = 0; i < 2*nrows; ++i) {
        ::strncpy(&scol[i][0], "abcdefghhgfedcbazzzz", 3*sizeof(double));
    }

    // Configure the encoder to encode said data

    odc_encoder_t* enc = nullptr;
    CHECK_RETURN(odc_new_encoder(&enc));
    std::unique_ptr<odc_encoder_t> enc_deleter(enc);

    CHECK_RETURN(odc_encoder_set_row_count(enc, nrows));
    CHECK_RETURN(odc_encoder_add_column(enc, "col1", ODC_INTEGER));
    CHECK_RETURN(odc_encoder_add_column(enc, "col2", ODC_BITFIELD));
    CHECK_RETURN(odc_encoder_add_column(enc, "col3", ODC_STRING));
    CHECK_RETURN(odc_encoder_add_column(enc, "col4", ODC_DOUBLE));
    CHECK_RETURN(odc_encoder_add_column(enc, "col5", ODC_REAL));

    CHECK_RETURN(odc_encoder_column_set_data_array(enc, 0, 0, 2*sizeof(icol[0]), icol));
    CHECK_RETURN(odc_encoder_column_set_data_array(enc, 1, 0, 2*sizeof(bcol[0]), bcol));
    CHECK_RETURN(odc_encoder_column_set_data_array(enc, 2, sizeof(scol[0]), 2*sizeof(scol[0]), scol));
    CHECK_RETURN(odc_encoder_column_set_data_array(enc, 3, 0, 2*sizeof(dcol[0]), dcol));
    CHECK_RETURN(odc_encoder_column_set_data_array(enc, 4, 0, 2*sizeof(rcol[0]), rcol));

    // Do the encoding

    eckit::Buffer encoded(1024 * 1024);
    long sz;
    CHECK_RETURN(odc_encode_to_buffer(enc, encoded.data(), encoded.size(), &sz));

    // Check that we have encoded what we think we have encoded

    odc_reader_t* reader = nullptr;
    CHECK_RETURN(odc_open_buffer(&reader, encoded.data(), sz));
    EXPECT(reader);
    std::unique_ptr<odc_reader_t> reader_deleter(reader);

    odc_frame_t* frame = nullptr;
    CHECK_RETURN(odc_new_frame(&frame, reader));
    EXPECT(frame);
    std::unique_ptr<odc_frame_t> frame_deleter(frame);

    CHECK_RETURN(odc_next_frame(frame));

    int column_count;
    CHECK_RETURN(odc_frame_column_count(frame, &column_count));
    EXPECT(column_count == ncols);

    long row_count;
    CHECK_RETURN(odc_frame_row_count(frame, &row_count));
    EXPECT(row_count == nrows);

    // Test that the data is correctly encoded

    odc_decoder_t* decoder;
    CHECK_RETURN(odc_new_decoder(&decoder));
    EXPECT(decoder);
    std::unique_ptr<odc_decoder_t> decoder_deleter(decoder);

    CHECK_RETURN(odc_decoder_defaults_from_frame(decoder, frame));

    long rows_decoded;
    CHECK_RETURN(odc_decode(decoder, frame, &rows_decoded));
    EXPECT(rows_decoded == nrows);

    const void* pdata;
    long row_stride;
    bool decodeColumnMajor;
    CHECK_RETURN(odc_decoder_data_array(decoder, &pdata, &row_stride, 0, &decodeColumnMajor));
    EXPECT(!decodeColumnMajor);
    EXPECT(pdata != nullptr);
    EXPECT(row_stride == 7*sizeof(double));

    const double (*row_data)[7] = reinterpret_cast<const double (*)[7]>(pdata);

    for (size_t row = 0; row < nrows; ++row) {
        EXPECT(reinterpret_cast<const long&>(row_data[row][0]) == icol[2*row]);
        EXPECT(reinterpret_cast<const long&>(row_data[row][1]) == bcol[2*row]);
        EXPECT(::strncmp("abcdefghhgfedcbazzzz", reinterpret_cast<const char*>(&row_data[row][2]), 3*sizeof(double)) == 0);
        EXPECT(row_data[row][5] == dcol[2*row]);
        EXPECT(row_data[row][6] == rcol[2*row]);
    }

    EXPECT(odc_next_frame(frame) == ODC_ITERATION_COMPLETE);
}

// ------------------------------------------------------------------------------------------------------

CASE("Encode with more rows that fit inside a table") {

    odc_integer_behaviour(ODC_INTEGERS_AS_LONGS);

    const int nrows = 10;
    const int ncols = 5;

    // Construct some source data

    long icol[nrows] = {1111, 2222, 3333, 4444, 5555, 6666, 7777, 8888, 9999, 0};
    long bcol[nrows] = {1101, 2202, 3303, 4404,  5505, 6606, 7707, 8808, 9909, 0};
    char scol[nrows][3 * sizeof(double)] = {0};
    double dcol[nrows] = {1131, 2232, 3333, 4434, 5535, 6636, 7737, 8838, 9939, 0};
    double rcol[nrows] = {1141, 2242, 3343, 4444, 5545, 6646, 7747, 8848, 9949, 0};
    for (size_t i = 0; i < nrows; ++i) {
        ::strncpy(&scol[i][0], "abcdefghhgfedcbazzzz", 3*sizeof(double));
    }

    // Configure the encoder to encode said data

    odc_encoder_t* enc = nullptr;
    CHECK_RETURN(odc_new_encoder(&enc));
    std::unique_ptr<odc_encoder_t> enc_deleter(enc);

    CHECK_RETURN(odc_encoder_set_row_count(enc, nrows));
    CHECK_RETURN(odc_encoder_add_column(enc, "col1", ODC_INTEGER));
    CHECK_RETURN(odc_encoder_add_column(enc, "col2", ODC_BITFIELD));
    CHECK_RETURN(odc_encoder_add_column(enc, "col3", ODC_STRING));
    CHECK_RETURN(odc_encoder_add_column(enc, "col4", ODC_DOUBLE));
    CHECK_RETURN(odc_encoder_add_column(enc, "col5", ODC_REAL));

    CHECK_RETURN(odc_encoder_column_set_data_array(enc, 0, 0, 0, icol));
    CHECK_RETURN(odc_encoder_column_set_data_array(enc, 1, 0, 0, bcol));
    CHECK_RETURN(odc_encoder_column_set_data_array(enc, 2, sizeof(scol[0]), 0, scol));
    CHECK_RETURN(odc_encoder_column_set_data_array(enc, 3, 0, 0, dcol));
    CHECK_RETURN(odc_encoder_column_set_data_array(enc, 4, 0, 0, rcol));

    // Set reduced number of lines per frame

    int maxPerFrame = 5;
    CHECK_RETURN(odc_encoder_set_rows_per_frame(enc, maxPerFrame));

    // Do the encoding

    eckit::Buffer encoded(1024 * 1024);
    long sz;
    CHECK_RETURN(odc_encode_to_buffer(enc, encoded.data(), encoded.size(), &sz));

    // Check that we have encoded what we think we have encoded

    odc_reader_t* reader = nullptr;
    CHECK_RETURN(odc_open_buffer(&reader, encoded.data(), sz));
    EXPECT(reader);
    std::unique_ptr<odc_reader_t> reader_deleter(reader);

    odc_frame_t* frame = nullptr;
    CHECK_RETURN(odc_new_frame(&frame, reader));
    EXPECT(frame);
    std::unique_ptr<odc_frame_t> frame_deleter(frame);

    // We now expect two frames

    for (int frame_idx = 0; frame_idx < 2; ++frame_idx) {

        CHECK_RETURN(odc_next_frame(frame));
        int row_offset = (frame_idx * maxPerFrame);

        int column_count;
        CHECK_RETURN(odc_frame_column_count(frame, &column_count));
        EXPECT(column_count == ncols);

        long row_count;
        CHECK_RETURN(odc_frame_row_count(frame, &row_count));
        EXPECT(row_count == (frame_idx ? nrows-maxPerFrame : maxPerFrame));

        // Test that the data is correctly encoded

        odc_decoder_t* decoder;
        CHECK_RETURN(odc_new_decoder(&decoder));
        EXPECT(decoder);
        std::unique_ptr<odc_decoder_t> decoder_deleter(decoder);

        CHECK_RETURN(odc_decoder_defaults_from_frame(decoder, frame));

        long rows_decoded;
        CHECK_RETURN(odc_decode(decoder, frame, &rows_decoded));
        EXPECT(rows_decoded == row_count);

        const void* pdata;
        long row_stride;
        bool decodeColumnMajor;
        CHECK_RETURN(odc_decoder_data_array(decoder, &pdata, &row_stride, 0, &decodeColumnMajor));
        EXPECT(!decodeColumnMajor);
        EXPECT(pdata != nullptr);
        EXPECT(row_stride == 7*sizeof(double));

        const double (*row_data)[7] = reinterpret_cast<const double (*)[7]>(pdata);

        for (int row = 0; row < row_count; ++row) {
            EXPECT(reinterpret_cast<const long&>(row_data[row][0]) == icol[row+row_offset]);
            EXPECT(reinterpret_cast<const long&>(row_data[row][1]) == bcol[row+row_offset]);
            EXPECT(::strncmp("abcdefghhgfedcbazzzz", reinterpret_cast<const char*>(&row_data[row][2]), 3*sizeof(double)) == 0);
            EXPECT(row_data[row][5] == dcol[row+row_offset]);
            EXPECT(row_data[row][6] == rcol[row+row_offset]);
        }
    }

    EXPECT(odc_next_frame(frame) == ODC_ITERATION_COMPLETE);
}

// ------------------------------------------------------------------------------------------------------

CASE("Encode to a file descriptor") {

    // Do some trivial encoding

    odc_integer_behaviour(ODC_INTEGERS_AS_LONGS);

    const int nrows = 10;
    long icol[nrows] = {1111, 2222, 3333, 4444, 5555, 6666, 7777, 8888, 9999, 0};

    // Configure the encoder to encode said data

    odc_encoder_t* enc = nullptr;
    CHECK_RETURN(odc_new_encoder(&enc));
    std::unique_ptr<odc_encoder_t> enc_deleter(enc);

    CHECK_RETURN(odc_encoder_set_row_count(enc, nrows));
    CHECK_RETURN(odc_encoder_add_column(enc, "col1", ODC_INTEGER));
    CHECK_RETURN(odc_encoder_column_set_data_array(enc, 0, 0, 0, icol));

    // Do the encoding

    eckit::TmpFile tf;
    int fd = ::open(tf.asString().c_str(), O_CREAT|O_WRONLY, 0666);
    ASSERT(fd != -1);
    long sz;
    try {
        CHECK_RETURN(odc_encode_to_file_descriptor(enc, fd, &sz));
    } catch(...) {
        ::close(fd);
        throw;
    }
    EXPECT(sz > 0);
    EXPECT(sz == tf.size());
    ::close(fd);

    // Check that we have encoded what we think we have encoded

    odc_reader_t* reader = nullptr;
    CHECK_RETURN(odc_open_path(&reader, tf.asString().c_str()));
    EXPECT(reader);
    std::unique_ptr<odc_reader_t> reader_deleter(reader);

    odc_frame_t* frame = nullptr;
    CHECK_RETURN(odc_new_frame(&frame, reader));
    EXPECT(frame);
    std::unique_ptr<odc_frame_t> frame_deleter(frame);

    CHECK_RETURN(odc_next_frame(frame));

    int column_count;
    CHECK_RETURN(odc_frame_column_count(frame, &column_count));
    EXPECT(column_count == 1);

    long row_count;
    CHECK_RETURN(odc_frame_row_count(frame, &row_count));
    EXPECT(row_count == nrows);

    // Test that the data is correctly encoded

    odc_decoder_t* decoder;
    CHECK_RETURN(odc_new_decoder(&decoder));
    EXPECT(decoder);
    std::unique_ptr<odc_decoder_t> decoder_deleter(decoder);

    CHECK_RETURN(odc_decoder_defaults_from_frame(decoder, frame));

    long rows_decoded;
    CHECK_RETURN(odc_decode(decoder, frame, &rows_decoded));
    EXPECT(rows_decoded == nrows);

    const void* pdata;
    CHECK_RETURN(odc_decoder_data_array(decoder, &pdata, 0, 0, 0));
    EXPECT(::memcmp(icol, pdata, sizeof(icol)) == 0);

    EXPECT(odc_next_frame(frame) == ODC_ITERATION_COMPLETE);
}

// ------------------------------------------------------------------------------------------------------

struct custom_buffer_t {
    char* data;
    size_t pos;
    size_t size;
};

typedef long (*write_fn)(void* handle, const void* buffer, long length);
long custom_buffer_write(custom_buffer_t* handle, const void* buffer, long length) {
    ASSERT(handle);
    ASSERT(length + handle->pos <= handle->size);
    ::memcpy(handle->data + handle->pos, buffer, length);
    handle->pos += length;
    return length;
}

CASE("Encode to a custom output stream") {

    // Do some trivial encoding

    odc_integer_behaviour(ODC_INTEGERS_AS_LONGS);

    const int nrows = 10;
    long icol[nrows] = {1111, 2222, 3333, 4444, 5555, 6666, 7777, 8888, 9999, 0};

    // Configure the encoder to encode said data

    odc_encoder_t* enc = nullptr;
    CHECK_RETURN(odc_new_encoder(&enc));
    std::unique_ptr<odc_encoder_t> enc_deleter(enc);

    CHECK_RETURN(odc_encoder_set_row_count(enc, nrows));
    CHECK_RETURN(odc_encoder_add_column(enc, "col1", ODC_INTEGER));
    CHECK_RETURN(odc_encoder_column_set_data_array(enc, 0, 0, 0, icol));

    // Do the encoding

    eckit::Buffer encoded(1024 * 1024);
    custom_buffer_t handle = { (char*)encoded, 0, encoded.size() };

    long sz;
    CHECK_RETURN(odc_encode_to_stream(enc, &handle, (write_fn)&custom_buffer_write, &sz));
    EXPECT(sz > 0);
    EXPECT(size_t(sz) == handle.pos);

    // Check that we have encoded what we think we have encoded

    odc_reader_t* reader = nullptr;
    CHECK_RETURN(odc_open_buffer(&reader, encoded.data(), sz));
    EXPECT(reader);
    std::unique_ptr<odc_reader_t> reader_deleter(reader);

    odc_frame_t* frame = nullptr;
    CHECK_RETURN(odc_new_frame(&frame, reader));
    EXPECT(frame);
    std::unique_ptr<odc_frame_t> frame_deleter(frame);

    CHECK_RETURN(odc_next_frame(frame));

    int column_count;
    CHECK_RETURN(odc_frame_column_count(frame, &column_count));
    EXPECT(column_count == 1);

    long row_count;
    CHECK_RETURN(odc_frame_row_count(frame, &row_count));
    EXPECT(row_count == nrows);

    // Test that the data is correctly encoded

    odc_decoder_t* decoder;
    CHECK_RETURN(odc_new_decoder(&decoder));
    EXPECT(decoder);
    std::unique_ptr<odc_decoder_t> decoder_deleter(decoder);

    CHECK_RETURN(odc_decoder_defaults_from_frame(decoder, frame));

    long rows_decoded;
    CHECK_RETURN(odc_decode(decoder, frame, &rows_decoded));
    EXPECT(rows_decoded == nrows);

    const void* pdata;
    CHECK_RETURN(odc_decoder_data_array(decoder, &pdata, 0, 0, 0));
    EXPECT(::memcmp(icol, pdata, sizeof(icol)) == 0);

    EXPECT(odc_next_frame(frame) == ODC_ITERATION_COMPLETE);
}

// ------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
