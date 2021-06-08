/**
 * To build this program, please make sure to reference linked library:
 *
 *     gcc -lodccore -o odc-c-ls odc_ls.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "odc/api/odc.h"

#define CHECK_RESULT(x) \
    do { \
        int rc = (x); \
        if (rc != ODC_SUCCESS) { \
            fprintf(stderr, "Error calling odc function \"%s\": %s\n", #x, odc_error_string(rc)); \
            exit(1); \
        } \
    } while (false); \

void usage() {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "    odc-c-ls <odb2 file>\n\n");
}

void write_header(odc_frame_t* frame, int ncols) {
    int col;

    for (col = 0; col < ncols; ++col) {
        const char* name;
        int type;
        int element_size;
        int bitfield_count;

        odc_frame_column_attributes(frame, col, &name, &type, &element_size, &bitfield_count);

        int padding = element_size - floor(log10(abs(col + 1))) + 2;

        fprintf(stdout, "%d. %-*s\t", col + 1, padding, name);
    }

    fprintf(stdout, "\n");
}

void write_bitfield(int64_t val, int nbits) {
    int bit;

    for (bit = nbits-1; bit >= 0; --bit) {
        fprintf(stdout, "%s", (val & (1 << bit)) ? "1" : "0");
    }
}

void write_data(odc_decoder_t* decoder, odc_frame_t* frame, long nrows, int ncols) {
    const void* data;
    long width;
    long height;
    bool columnMajor;
    CHECK_RESULT(odc_decoder_data_array(decoder, &data, &width, &height, &columnMajor));

    const char* name;
    int element_size;
    int bitfield_count;

    int column_types[ncols];
    int column_offsets[ncols];
    int column_sizes[ncols];
    int column_bf_sizes[ncols];

    long missing_integer;
    double missing_double;

    int current_offset = 0;
    int col;

    for (col = 0; col < ncols; ++col) {
        CHECK_RESULT(odc_frame_column_attributes(frame, col, &name, &column_types[col],
                                                 &column_sizes[col], &bitfield_count));
        column_offsets[col] = current_offset;
        current_offset += column_sizes[col];

        if (column_types[col] == ODC_BITFIELD) {
            int bf;

            for (bf = 0; bf < bitfield_count; ++bf) {
                const char* bf_name;
                int bf_offset;
                int bf_size;

                CHECK_RESULT(odc_frame_bitfield_attributes(frame, col, bf, &bf_name, &bf_offset, &bf_size));

                if (bf == 0) column_bf_sizes[col] = bf_size;
                else column_bf_sizes[col] += bf_size;
            }
        }
    }

    CHECK_RESULT(odc_missing_integer(&missing_integer));
    CHECK_RESULT(odc_missing_double(&missing_double));

    int row;

    for (row = 0; row < nrows; row++) {
        int col;

        for (col = 0; col < ncols; col++) {
            const void* value_p = &((const char*)data)[column_offsets[col] + (width * row)];

            switch (column_types[col]) {
                case ODC_INTEGER:
                    if (*(const int64_t*)value_p == missing_integer) {
                        fprintf(stdout, ".");
                    }
                    else {
                        fprintf(stdout, "%-*lld", column_sizes[col], *(const int64_t*)value_p);
                    }
                    break;
                case ODC_BITFIELD:
                    if (*(const int64_t*)value_p == 0) {
                        fprintf(stdout, ".");
                    }
                    else {
                        write_bitfield(*(const int64_t*)value_p, column_bf_sizes[col]);
                    }
                    break;
                case ODC_REAL:
                case ODC_DOUBLE:
                    if (*(const double*)value_p == missing_double) {
                        fprintf(stdout, ".");
                    }
                    else {
                        fprintf(stdout, "%-*f", column_sizes[col], *(const double*)value_p);
                    }
                    break;
                case ODC_STRING:
                    fprintf(stdout, "%-.*s", column_sizes[col], (const char*)value_p);
                    break;
                default:
                    fprintf(stdout, "<unknown>");
                    break;
            }
            fprintf(stdout, "\t");
        }
        fprintf(stdout, "\n");
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        usage();
        return 1;
    }

    char* path = argv[1];

    CHECK_RESULT(odc_initialise_api());  // initialising api
    CHECK_RESULT(odc_integer_behaviour(ODC_INTEGERS_AS_LONGS));  // change from default doubles

    odc_reader_t* reader = NULL;
    odc_frame_t* frame = NULL;

    CHECK_RESULT(odc_open_path(&reader, path));  // opening path
    CHECK_RESULT(odc_new_frame(&frame, reader));  // initialising frame

    int rc;
    long max_aggregated_rows = 1000000;

    while ((rc = odc_next_frame_aggregated(frame, max_aggregated_rows)) == ODC_SUCCESS) {
        int ncols;

        CHECK_RESULT(odc_frame_column_count(frame, &ncols));  // getting column count
        write_header(frame, ncols);

        long nrows;
        odc_decoder_t* decoder = NULL;

        CHECK_RESULT(odc_new_decoder(&decoder));  // initialising decoder
        CHECK_RESULT(odc_decoder_defaults_from_frame(decoder, frame));  // setting decoder structure
        CHECK_RESULT(odc_decode(decoder, frame, &nrows));  // decoding data
        write_data(decoder, frame, nrows, ncols);
        CHECK_RESULT(odc_free_decoder(decoder));  // cleaning up decoder
    }

    if (rc != ODC_ITERATION_COMPLETE) {
        fprintf(stderr, "Error: %s\n", odc_error_string(rc));  // unsuccessful end of frame iteration
        return 1;
    }

    CHECK_RESULT(odc_close(reader));  // closing reader

    return 0;
}
