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

// Specialise custom deletion for odc_reader_t

namespace std{
template <> struct default_delete<odc_encoder_t> {
    void operator() (odc_encoder_t* e) { odc_free_encoder(e); }
};

template <> struct default_delete<odc_reader_t> {
    void operator() (odc_reader_t* o) { odc_close(o); }
};

template <> struct default_delete<odc_frame_t> {
    void operator() (odc_frame_t* t) { odc_free_frame(t); }
};

template <> struct default_delete<odc_decoder_t> {
    void operator() (odc_decoder_t* t) { odc_free_decode_target(t); }
};
}

// ------------------------------------------------------------------------------------------------------

// TODO: Row-major
// TODO: Column major

CASE("Encode data in standard tabular form") {

    odc_integer_behaviour(ODC_INTEGERS_AS_DOUBLES);

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

    std::unique_ptr<odc_encoder_t> enc(odc_alloc_encoder());

    odc_encoder_set_row_count(enc.get(), nrows);
    odc_encoder_set_data_array(enc.get(), data, false);
    EXPECT(0 == odc_encoder_add_column(enc.get(), "col1", ODC_INTEGER));
    EXPECT(1 == odc_encoder_add_column(enc.get(), "col2", ODC_REAL));
    EXPECT(2 == odc_encoder_add_column(enc.get(), "col3", ODC_DOUBLE));
    EXPECT(3 == odc_encoder_add_column(enc.get(), "col4", ODC_DOUBLE));
    EXPECT(4 == odc_encoder_add_column(enc.get(), "col5", ODC_STRING));
    odc_encoder_column_set_size(enc.get(), 4, 2 * sizeof(double));
    EXPECT(5 == odc_encoder_add_column(enc.get(), "col6", ODC_REAL));
    EXPECT(6 == odc_encoder_add_column(enc.get(), "col7", ODC_BITFIELD));
    odc_encoder_column_add_bitfield_field(enc.get(), 6, "bits1", 2);
    odc_encoder_column_add_bitfield_field(enc.get(), 6, "bits2", 3);
    odc_encoder_column_add_bitfield_field(enc.get(), 6, "bits3", 1);

    // Do the encoding

    eckit::Buffer encoded(1024 * 1024);
    long sz = odc_encode_to_buffer(enc.get(), encoded, encoded.size());

    // Check that the table contains what we expect

    std::unique_ptr<odc_reader_t> o(odc_open_buffer(encoded, sz));
    std::unique_ptr<odc_frame_t> t(odc_alloc_next_frame(o.get()));

    EXPECT(t);
    EXPECT(odc_frame_column_count(t.get()) == ncols-1);
    EXPECT(odc_frame_row_count(t.get()) == nrows);

    EXPECT(::strcmp(odc_frame_column_name(t.get(), 0), "col1") == 0);
    EXPECT(::strcmp(odc_frame_column_name(t.get(), 1), "col2") == 0);
    EXPECT(::strcmp(odc_frame_column_name(t.get(), 2), "col3") == 0);
    EXPECT(::strcmp(odc_frame_column_name(t.get(), 3), "col4") == 0);
    EXPECT(::strcmp(odc_frame_column_name(t.get(), 4), "col5") == 0);
    EXPECT(::strcmp(odc_frame_column_name(t.get(), 5), "col6") == 0);
    EXPECT(::strcmp(odc_frame_column_name(t.get(), 6), "col7") == 0);

    EXPECT(odc_frame_column_type(t.get(), 0) == ODC_INTEGER);
    EXPECT(odc_frame_column_type(t.get(), 1) == ODC_REAL);
    EXPECT(odc_frame_column_type(t.get(), 2) == ODC_DOUBLE);
    EXPECT(odc_frame_column_type(t.get(), 3) == ODC_DOUBLE);
    EXPECT(odc_frame_column_type(t.get(), 4) == ODC_STRING);
    EXPECT(odc_frame_column_type(t.get(), 5) == ODC_REAL);
    EXPECT(odc_frame_column_type(t.get(), 6) == ODC_BITFIELD);

    EXPECT(odc_frame_column_data_size(t.get(), 0) == sizeof(double));
    EXPECT(odc_frame_column_data_size(t.get(), 1) == sizeof(double));
    EXPECT(odc_frame_column_data_size(t.get(), 2) == sizeof(double));
    EXPECT(odc_frame_column_data_size(t.get(), 3) == sizeof(double));
    EXPECT(odc_frame_column_data_size(t.get(), 4) == 2*sizeof(double));
    EXPECT(odc_frame_column_data_size(t.get(), 5) == sizeof(double));
    EXPECT(odc_frame_column_data_size(t.get(), 6) == sizeof(double));

    EXPECT(odc_frame_column_bitfield_count(t.get(), 0) == 0);
    EXPECT(odc_frame_column_bitfield_count(t.get(), 1) == 0);
    EXPECT(odc_frame_column_bitfield_count(t.get(), 2) == 0);
    EXPECT(odc_frame_column_bitfield_count(t.get(), 3) == 0);
    EXPECT(odc_frame_column_bitfield_count(t.get(), 4) == 0);
    EXPECT(odc_frame_column_bitfield_count(t.get(), 5) == 0);
    EXPECT(odc_frame_column_bitfield_count(t.get(), 6) == 3);

    EXPECT(::strcmp(odc_frame_column_bits_name(t.get(), 6, 0), "bits1") == 0);
    EXPECT(::strcmp(odc_frame_column_bits_name(t.get(), 6, 1), "bits2") == 0);
    EXPECT(::strcmp(odc_frame_column_bits_name(t.get(), 6, 2), "bits3") == 0);
    EXPECT(odc_frame_column_bits_size(t.get(), 6, 0) == 2);
    EXPECT(odc_frame_column_bits_size(t.get(), 6, 1) == 3);
    EXPECT(odc_frame_column_bits_size(t.get(), 6, 2) == 1);
    EXPECT(odc_frame_column_bits_offset(t.get(), 6, 0) == 0);
    EXPECT(odc_frame_column_bits_offset(t.get(), 6, 1) == 2);
    EXPECT(odc_frame_column_bits_offset(t.get(), 6, 2) == 5);

    // Test that the data is correctly encoded

    std::unique_ptr<odc_decode_target_t> dec(odc_alloc_decode_target());
    odc_frame_build_all_decode_target(t.get(), dec.get());
    odc_frame_decode(t.get(), dec.get(), 1);

    EXPECT(odc_decode_target_column_data(dec.get(), 0) == odc_decode_target_array_data(dec.get()));
    double vals1[] = {0, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000, 11000, 12000, 13000, 14000};
    double vals2[] = {3, 1003, 2003, 3003, 4003, 5003, 6003, 7003, 8003, 9003, 10003, 11003, 12003, 13003, 14003};
    double vals3[] = {6, 1006, 2006, 3006, 4006, 5006, 6006, 7006, 8006, 9006, 10006, 11006, 12006, 13006, 14006};
    double vals4[] = {9, 1009, 2009, 3009, 4009, 5009, 6009, 7009, 8009, 9009, 10009, 11009, 12009, 13009, 14009};
    double vals6[] = {18, 1018, 2018, 3018, 4018, 5018, 6018, 7018, 8018, 9018, 10018, 11018, 12018, 13018, 14018};
    double vals7[] = {21, 1021, 2021, 3021, 4021, 5021, 6021, 7021, 8021, 9021, 10021, 11021, 12021, 13021, 14021};
    EXPECT(::memcmp(vals1, odc_decode_target_column_data(dec.get(), 0), sizeof(vals1)) == 0);
    EXPECT(::memcmp(vals2, odc_decode_target_column_data(dec.get(), 1), sizeof(vals2)) == 0);
    EXPECT(::memcmp(vals3, odc_decode_target_column_data(dec.get(), 2), sizeof(vals3)) == 0);
    EXPECT(::memcmp(vals4, odc_decode_target_column_data(dec.get(), 3), sizeof(vals4)) == 0);
    EXPECT(::memcmp(vals6, odc_decode_target_column_data(dec.get(), 5), sizeof(vals6)) == 0);
    EXPECT(::memcmp(vals7, odc_decode_target_column_data(dec.get(), 6), sizeof(vals7)) == 0);
    const char* c5 = reinterpret_cast<const char*>(odc_decode_target_column_data(dec.get(), 4));
    for (size_t i = 0; i < ncols; ++i) {
        ASSERT(strncmp(c5+(2*sizeof(double)*i), "abcdefghijkl", 2*sizeof(double)) == 0);
    }

    EXPECT(odc_alloc_next_frame(o.get()) == 0);
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

    std::unique_ptr<odc_encoder_t> enc(odc_alloc_encoder());

    odc_encoder_set_row_count(enc.get(), nrows);
    EXPECT(0 == odc_encoder_add_column(enc.get(), "col1", ODC_INTEGER));
    EXPECT(1 == odc_encoder_add_column(enc.get(), "col2", ODC_BITFIELD));
    EXPECT(2 == odc_encoder_add_column(enc.get(), "col3", ODC_STRING));
    odc_encoder_column_set_size(enc.get(), 2, 3 * sizeof(double));
    EXPECT(3 == odc_encoder_add_column(enc.get(), "col4", ODC_DOUBLE));
    EXPECT(4 == odc_encoder_add_column(enc.get(), "col5", ODC_REAL));

    odc_encoder_column_set_data(enc.get(), 0, icol);
    odc_encoder_column_set_data(enc.get(), 1, bcol);
    odc_encoder_column_set_data(enc.get(), 2, scol);
    odc_encoder_column_set_data(enc.get(), 3, dcol);
    odc_encoder_column_set_data(enc.get(), 4, rcol);

    // Do the encoding

    eckit::Buffer encoded(1024 * 1024);
    long sz = odc_encode_to_buffer(enc.get(), encoded, encoded.size());

    // Check that we have encoded what we think we have encoded

    std::unique_ptr<odc_reader_t> o(odc_open_buffer(encoded, sz));
    std::unique_ptr<odc_frame_t> t(odc_alloc_next_frame(o.get()));

    EXPECT(t);
    EXPECT(odc_frame_column_count(t.get()) == ncols);
    EXPECT(odc_frame_row_count(t.get()) == nrows);

    std::unique_ptr<odc_decode_target_t> dec(odc_alloc_decode_target());
    odc_frame_build_all_decode_target(t.get(), dec.get());
    odc_frame_decode(t.get(), dec.get(), 1);

    EXPECT(odc_decode_target_column_data(dec.get(), 0) == odc_decode_target_array_data(dec.get()));
    EXPECT(::memcmp(icol, odc_decode_target_column_data(dec.get(), 0), sizeof(icol)) == 0);
    EXPECT(::memcmp(bcol, odc_decode_target_column_data(dec.get(), 1), sizeof(bcol)) == 0);
    EXPECT(::memcmp(dcol, odc_decode_target_column_data(dec.get(), 3), sizeof(dcol)) == 0);
    EXPECT(::memcmp(rcol, odc_decode_target_column_data(dec.get(), 4), sizeof(rcol)) == 0);

    EXPECT(odc_decode_target_column_size(dec.get(), 2) == 3*sizeof(double));
    const char* c2 = reinterpret_cast<const char*>(odc_decode_target_column_data(dec.get(), 2));
    for (size_t i = 0; i < ncols; ++i) {
        ASSERT(strncmp(c2+(3*sizeof(double)*i), "abcdefghhgfedcbazzzz", 3*sizeof(double)) == 0);
    }

    EXPECT(odc_alloc_next_frame(o.get()) == 0);
}

// ------------------------------------------------------------------------------------------------------

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

    std::unique_ptr<odc_encoder_t> enc(odc_alloc_encoder());

    odc_encoder_set_row_count(enc.get(), nrows);
    EXPECT(0 == odc_encoder_add_column(enc.get(), "col1", ODC_INTEGER));
    EXPECT(1 == odc_encoder_add_column(enc.get(), "col2", ODC_BITFIELD));
    EXPECT(2 == odc_encoder_add_column(enc.get(), "col3", ODC_STRING));
    odc_encoder_column_set_size(enc.get(), 2, sizeof(scol[0]));
    EXPECT(3 == odc_encoder_add_column(enc.get(), "col4", ODC_DOUBLE));
    EXPECT(4 == odc_encoder_add_column(enc.get(), "col5", ODC_REAL));

    odc_encoder_column_set_data(enc.get(), 0, icol);
    odc_encoder_column_set_data(enc.get(), 1, bcol);
    odc_encoder_column_set_data(enc.get(), 2, scol);
    odc_encoder_column_set_data(enc.get(), 3, dcol);
    odc_encoder_column_set_data(enc.get(), 4, rcol);

    odc_encoder_column_set_stride(enc.get(), 0, 2 * sizeof(icol[0]));
    odc_encoder_column_set_stride(enc.get(), 1, 2 * sizeof(bcol[0]));
    odc_encoder_column_set_stride(enc.get(), 2, 2 * sizeof(scol[0]));
    odc_encoder_column_set_stride(enc.get(), 3, 2 * sizeof(dcol[0]));
    odc_encoder_column_set_stride(enc.get(), 4, 2 * sizeof(rcol[0]));

    // Do the encoding

    eckit::Buffer encoded(1024 * 1024);
    long sz = odc_encode_to_buffer(enc.get(), encoded, encoded.size());

    // Check that we have encoded what we think we have encoded

    std::unique_ptr<odc_reader_t> o(odc_open_buffer(encoded, sz));
    std::unique_ptr<odc_frame_t> t(odc_alloc_next_frame(o.get()));

    EXPECT(t);
    EXPECT(odc_frame_column_count(t.get()) == ncols);
    EXPECT(odc_frame_row_count(t.get()) == nrows);

    std::unique_ptr<odc_decode_target_t> dec(odc_alloc_decode_target());
    odc_frame_build_all_decode_target(t.get(), dec.get());
    odc_frame_decode(t.get(), dec.get(), 1);

    EXPECT(odc_decode_target_column_data(dec.get(), 0) == odc_decode_target_array_data(dec.get()));
    for (size_t row = 0; row < nrows; ++row) {
        EXPECT(icol[2*row] == *(reinterpret_cast<const int64_t*>(odc_decode_target_column_data(dec.get(), 0)) + row));
        EXPECT(bcol[2*row] == *(reinterpret_cast<const int64_t*>(odc_decode_target_column_data(dec.get(), 1)) + row));
        EXPECT(dcol[2*row] == *(reinterpret_cast<const double*>(odc_decode_target_column_data(dec.get(), 3)) + row));
        EXPECT(rcol[2*row] == *(reinterpret_cast<const double*>(odc_decode_target_column_data(dec.get(), 4)) + row));
        const char* c2 = reinterpret_cast<const char*>(odc_decode_target_column_data(dec.get(), 2));
        EXPECT(strncmp(c2 + (row * sizeof(scol[0])), "abcdefghhgfedcbazzzz", sizeof(scol[0])) == 0);
    }

    EXPECT(odc_alloc_next_frame(o.get()) == 0);

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

    std::unique_ptr<odc_encoder_t> enc(odc_alloc_encoder());

    odc_encoder_set_row_count(enc.get(), nrows);
    EXPECT(0 == odc_encoder_add_column(enc.get(), "col1", ODC_INTEGER));
    EXPECT(1 == odc_encoder_add_column(enc.get(), "col2", ODC_BITFIELD));
    EXPECT(2 == odc_encoder_add_column(enc.get(), "col3", ODC_STRING));
    odc_encoder_column_set_size(enc.get(), 2, 3 * sizeof(double));
    EXPECT(3 == odc_encoder_add_column(enc.get(), "col4", ODC_DOUBLE));
    EXPECT(4 == odc_encoder_add_column(enc.get(), "col5", ODC_REAL));

    odc_encoder_column_set_data(enc.get(), 0, icol);
    odc_encoder_column_set_data(enc.get(), 1, bcol);
    odc_encoder_column_set_data(enc.get(), 2, scol);
    odc_encoder_column_set_data(enc.get(), 3, dcol);
    odc_encoder_column_set_data(enc.get(), 4, rcol);

    // Set reduced number of lines per frame

    odc_encoder_set_rows_per_frame(enc.get(), 5);

    // Do the encoding

    eckit::Buffer encoded(1024 * 1024);
    long sz = odc_encode_to_buffer(enc.get(), encoded, encoded.size());

    int fd = open("/home/simon/test.odb", O_CREAT|O_WRONLY|O_TRUNC);
    write(fd, encoded, sz);
    close(fd);

    // Check that we have encoded what we think we have encoded

    std::unique_ptr<odc_reader_t> o(odc_open_buffer(encoded, sz));

    std::unique_ptr<odc_frame_t> t(odc_alloc_next_frame(o.get()));

    EXPECT(t);
    EXPECT(odc_frame_column_count(t.get()) == ncols);
    EXPECT(odc_frame_row_count(t.get()) == 5);  // n.b. != nrows

    std::unique_ptr<odc_decode_target_t> dec(odc_alloc_decode_target());
    odc_frame_build_all_decode_target(t.get(), dec.get());
    odc_frame_decode(t.get(), dec.get(), 1);

    EXPECT(odc_decode_target_column_data(dec.get(), 0) == odc_decode_target_array_data(dec.get()));
    EXPECT(::memcmp(icol, odc_decode_target_column_data(dec.get(), 0), 5 * sizeof(icol[0])) == 0);
    EXPECT(::memcmp(bcol, odc_decode_target_column_data(dec.get(), 1), 5 * sizeof(bcol[0])) == 0);
    EXPECT(::memcmp(dcol, odc_decode_target_column_data(dec.get(), 3), 5 * sizeof(dcol[0])) == 0);
    EXPECT(::memcmp(rcol, odc_decode_target_column_data(dec.get(), 4), 5 * sizeof(rcol[0])) == 0);

    EXPECT(odc_decode_target_column_size(dec.get(), 2) == 3*sizeof(double));
    const char* c2 = reinterpret_cast<const char*>(odc_decode_target_column_data(dec.get(), 2));
    for (size_t i = 0; i < 5; ++i) {
        ASSERT(strncmp(c2+(3*sizeof(double)*i), "abcdefghhgfedcbazzzz", 3*sizeof(double)) == 0);
    }

    // And there is a second set of data

    t.reset(odc_alloc_next_frame(o.get()));
    EXPECT(t);
    EXPECT(odc_frame_column_count(t.get()) == ncols);
    EXPECT(odc_frame_row_count(t.get()) == 5);  // n.b. != nrows

    dec.reset(odc_alloc_decode_target());
    odc_frame_build_all_decode_target(t.get(), dec.get());
    odc_frame_decode(t.get(), dec.get(), 1);

    EXPECT(odc_decode_target_column_data(dec.get(), 0) == odc_decode_target_array_data(dec.get()));
    EXPECT(::memcmp(&icol[5], odc_decode_target_column_data(dec.get(), 0), 5 * sizeof(icol[0])) == 0);
    EXPECT(::memcmp(&bcol[5], odc_decode_target_column_data(dec.get(), 1), 5 * sizeof(bcol[0])) == 0);
    EXPECT(::memcmp(&dcol[5], odc_decode_target_column_data(dec.get(), 3), 5 * sizeof(dcol[0])) == 0);
    EXPECT(::memcmp(&rcol[5], odc_decode_target_column_data(dec.get(), 4), 5 * sizeof(rcol[0])) == 0);

    EXPECT(odc_decode_target_column_size(dec.get(), 2) == 3*sizeof(double));
    c2 = reinterpret_cast<const char*>(odc_decode_target_column_data(dec.get(), 2));
    for (size_t i = 0; i < 5; ++i) {
        ASSERT(strncmp(c2+(3*sizeof(double)*i), "abcdefghhgfedcbazzzz", 3*sizeof(double)) == 0);
    }

    // Until done.

    EXPECT(odc_alloc_next_frame(o.get()) == 0);

}

// ------------------------------------------------------------------------------------------------------

CASE("Encode to a file descriptor") {

    // Do some trivial encoding

    odc_integer_behaviour(ODC_INTEGERS_AS_LONGS);

    const int nrows = 10;
    long icol[nrows] = {1111, 2222, 3333, 4444, 5555, 6666, 7777, 8888, 9999, 0};

    std::unique_ptr<odc_encoder_t> enc(odc_alloc_encoder());
    odc_encoder_set_row_count(enc.get(), nrows);
    EXPECT(0 == odc_encoder_add_column(enc.get(), "col1", ODC_INTEGER));
    odc_encoder_column_set_data(enc.get(), 0, icol);

    // Do the encoding

    eckit::TmpFile tf;
    int fd = ::open(tf.asString().c_str(), O_CREAT|O_WRONLY);
    ASSERT(fd != -1);
    long sz;
    try {
        sz = odc_encode_to_file_descriptor(enc.get(), fd);
    } catch(...) {
        ::close(fd);
        throw;
    }
    EXPECT(sz > 0);
    EXPECT(sz == tf.size());
    ::close(fd);

    // Check that we have encoded what we think we have encoded

    std::unique_ptr<odc_reader_t> o(odc_open_path(tf.asString().c_str()));
    std::unique_ptr<odc_frame_t> t(odc_alloc_next_frame(o.get()));

    EXPECT(t);
    EXPECT(odc_frame_column_count(t.get()) == 1);
    EXPECT(odc_frame_row_count(t.get()) == nrows);

    std::unique_ptr<odc_decode_target_t> dec(odc_alloc_decode_target());
    odc_frame_build_all_decode_target(t.get(), dec.get());
    odc_frame_decode(t.get(), dec.get(), 1);

    EXPECT(odc_decode_target_column_data(dec.get(), 0) == odc_decode_target_array_data(dec.get()));
    EXPECT(::memcmp(icol, odc_decode_target_column_data(dec.get(), 0), sizeof(icol)) == 0);

    EXPECT(odc_alloc_next_frame(o.get()) == 0);
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

    std::unique_ptr<odc_encoder_t> enc(odc_alloc_encoder());
    odc_encoder_set_row_count(enc.get(), nrows);
    EXPECT(0 == odc_encoder_add_column(enc.get(), "col1", ODC_INTEGER));
    odc_encoder_column_set_data(enc.get(), 0, icol);

    // Do the encoding

    eckit::Buffer encoded(1024 * 1024);
    custom_buffer_t handle = { (char*)encoded, 0, encoded.size() };

    long sz = odc_encode_to_stream(enc.get(), &handle, (write_fn)&custom_buffer_write);
    EXPECT(sz > 0);
    EXPECT(size_t(sz) == handle.pos);

    // Check that we have encoded what we think we have encoded

    std::unique_ptr<odc_reader_t> o(odc_open_buffer(encoded, sz));
    std::unique_ptr<odc_frame_t> t(odc_alloc_next_frame(o.get()));

    EXPECT(t);
    EXPECT(odc_frame_column_count(t.get()) == 1);
    EXPECT(odc_frame_row_count(t.get()) == nrows);

    std::unique_ptr<odc_decode_target_t> dec(odc_alloc_decode_target());
    odc_frame_build_all_decode_target(t.get(), dec.get());
    odc_frame_decode(t.get(), dec.get(), 1);

    EXPECT(odc_decode_target_column_data(dec.get(), 0) == odc_decode_target_array_data(dec.get()));
    EXPECT(::memcmp(icol, odc_decode_target_column_data(dec.get(), 0), sizeof(icol)) == 0);

    EXPECT(odc_alloc_next_frame(o.get()) == 0);
}

// ------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
