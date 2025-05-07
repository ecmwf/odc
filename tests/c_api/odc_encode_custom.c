/**
 * To build this program, please make sure to reference linked library:
 *
 *     gcc -lodccore -o odc-c-encode-custom odc_encode_custom.c
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "odc/api/odc.h"

// Bitfield constants
#define Ob00000001 1
#define Ob00001011 11
#define Ob01101011 107

#define CHECK_RESULT(x)                                                                           \
    do {                                                                                          \
        int rc = (x);                                                                             \
        if (rc != ODC_SUCCESS) {                                                                  \
            fprintf(stderr, "Error calling odc function \"%s\": %s\n", #x, odc_error_string(rc)); \
            exit(1);                                                                              \
        }                                                                                         \
    } while (false);

void usage() {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "    odc-c-encode-custom <odb2 output file>\n\n");
}

void cycle_longs(long* list, int size, long* pool, int pool_size) {
    int index = 0;
    int i;

    for (i = 0; i < size; i++) {
        if (index == pool_size)
            index = 0;
        list[i] = pool[index];
        index++;
    }
}

void cycle_doubles(double* list, int size, double* pool, int pool_size) {
    int index = 0;
    int i;

    for (i = 0; i < size; i++) {
        if (index == pool_size)
            index = 0;
        list[i] = pool[index];
        index++;
    }
}

void create_scratch_data(long nrows, char data0[][8], int64_t data1[], char data2[][8], char data3[][16],
                         double data4[], int64_t data5[], double data6[], int64_t data7[]) {

    // Prepare the current date as an integer

    time_t rawtime;
    time(&rawtime);

    struct tm* timeinfo;
    timeinfo = localtime(&rawtime);

    int64_t date = 10000 * (timeinfo->tm_year + 1900) + 100 * (timeinfo->tm_mon + 1) + timeinfo->tm_mday;

    // Prepare the list of integer values, including the missing value

    long missing_integer;
    CHECK_RESULT(odc_missing_integer(&missing_integer));

    long integer_pool[]   = {1234, 4321, missing_integer};
    int integer_pool_size = sizeof(integer_pool) / sizeof(integer_pool[0]);

    long missing_integers[nrows];
    cycle_longs(missing_integers, nrows, integer_pool, integer_pool_size);

    // Prepare the list of double values, including the missing value

    double missing_double;
    CHECK_RESULT(odc_missing_double(&missing_double));

    double double_pool[] = {12.34, 43.21, missing_double};
    int double_pool_size = sizeof(double_pool) / sizeof(double_pool[0]);

    double missing_doubles[nrows];
    cycle_doubles(missing_doubles, nrows, double_pool, double_pool_size);

    // Prepare the list of bitfield values

    long bitfield_pool[]   = {Ob00000001, Ob00001011, Ob01101011};
    int bitfield_pool_size = sizeof(bitfield_pool) / sizeof(bitfield_pool[0]);

    long bitfield_values[nrows];
    cycle_longs(bitfield_values, nrows, bitfield_pool, bitfield_pool_size);

    int i;

    // Fill in the passed data arrays with scratch values
    for (i = 0; i < nrows; i++) {
        snprintf(data0[i], 8, "xxxx");                   // expver
        data1[i] = date;                                 // date@hdr
        snprintf(data2[i], 7, "stat%02d", i);            // statid@hdr
        snprintf(data3[i], 16, "0-12345-0-678%02d", i);  // wigos@hdr
        data4[i] = 12.3456 * i;                          // obsvalue@body
        data5[i] = missing_integers[i];                  // integer_missing
        data6[i] = missing_doubles[i];                   // double_missing
        data7[i] = bitfield_values[i];                   // bitfield_column
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        usage();
        return 1;
    }

    // Get output path from command argument
    char* path = argv[1];

    // Initialise API and set treatment of integers as longs
    CHECK_RESULT(odc_initialise_api());
    CHECK_RESULT(odc_integer_behaviour(ODC_INTEGERS_AS_LONGS));

    // Define row count
    const long nrows = 20;

    // Allocate data array for each column
    char data0[nrows][8];
    int64_t data1[nrows];
    char data2[nrows][8];
    char data3[nrows][16];
    double data4[nrows];
    int64_t data5[nrows];
    double data6[nrows];
    int64_t data7[nrows];

    // Set up the allocated array with scratch data
    create_scratch_data(nrows, data0, data1, data2, data3, data4, data5, data6, data7);

    // Initialise encoder
    odc_encoder_t* encoder = NULL;
    CHECK_RESULT(odc_new_encoder(&encoder));

    // Set number of rows to allocate in the encoder
    CHECK_RESULT(odc_encoder_set_row_count(encoder, nrows));

    // Define all column names and their types
    CHECK_RESULT(odc_encoder_add_column(encoder, "expver", ODC_STRING));
    CHECK_RESULT(odc_encoder_add_column(encoder, "date@hdr", ODC_INTEGER));
    CHECK_RESULT(odc_encoder_add_column(encoder, "statid@hdr", ODC_STRING));
    CHECK_RESULT(odc_encoder_add_column(encoder, "wigos@hdr", ODC_STRING));
    CHECK_RESULT(odc_encoder_add_column(encoder, "obsvalue@body", ODC_REAL));
    CHECK_RESULT(odc_encoder_add_column(encoder, "integer_missing", ODC_INTEGER));
    CHECK_RESULT(odc_encoder_add_column(encoder, "double_missing", ODC_REAL));
    CHECK_RESULT(odc_encoder_add_column(encoder, "bitfield_column", ODC_BITFIELD));

    // Column `wigos@hdr` is a 16-byte string column
    CHECK_RESULT(odc_encoder_column_set_data_size(encoder, 3, 16));

    // Column `bitfield_column` is an integer with 4 bitfield values in it
    CHECK_RESULT(odc_encoder_column_add_bitfield(encoder, 7, "flag_a", 1));
    CHECK_RESULT(odc_encoder_column_add_bitfield(encoder, 7, "flag_b", 2));
    CHECK_RESULT(odc_encoder_column_add_bitfield(encoder, 7, "flag_c", 3));
    CHECK_RESULT(odc_encoder_column_add_bitfield(encoder, 7, "flag_d", 1));

    // Set a custom data layout and data array for each column
    CHECK_RESULT(odc_encoder_column_set_data_array(encoder, 0, 8, 8, data0));
    CHECK_RESULT(odc_encoder_column_set_data_array(encoder, 1, sizeof(int64_t), sizeof(int64_t), data1));
    CHECK_RESULT(odc_encoder_column_set_data_array(encoder, 2, 8, 8, data2));
    CHECK_RESULT(odc_encoder_column_set_data_array(encoder, 3, 16, 16, data3));
    CHECK_RESULT(odc_encoder_column_set_data_array(encoder, 4, sizeof(double), sizeof(double), data4));
    CHECK_RESULT(odc_encoder_column_set_data_array(encoder, 5, sizeof(int64_t), sizeof(int64_t), data5));
    CHECK_RESULT(odc_encoder_column_set_data_array(encoder, 6, sizeof(double), sizeof(double), data6));
    CHECK_RESULT(odc_encoder_column_set_data_array(encoder, 7, sizeof(int64_t), sizeof(int64_t), data7));

    const char* property_key   = "encoded_by";
    const char* property_value = "odc_example";

    // Add some key/value metadata to the frame
    CHECK_RESULT(odc_encoder_add_property(encoder, property_key, property_value));

    int file_descriptor = open(path, O_CREAT | O_TRUNC | O_WRONLY, 0666);
    long size;

    // Encode ODB-2 into an already open file descriptor
    CHECK_RESULT(odc_encode_to_file_descriptor(encoder, file_descriptor, &size));

    close(file_descriptor);

    // Deallocate memory used up by the encoder
    CHECK_RESULT(odc_free_encoder(encoder));

    fprintf(stdout, "Written %ld rows to %s\n", nrows, path);

    return 0;
}
