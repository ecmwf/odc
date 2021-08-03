/**
 * To build this program, please make sure to reference linked library:
 *
 *     gcc -lodccore -o odc-c-header odc_header.c
 */

#include <stdio.h>
#include <stdlib.h>

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
    fprintf(stderr, "    odc-c-header <odb2 file 1> [<odb2 file 2> ...]\n\n");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        usage();
        return 1;
    }

    // Initialise API
    CHECK_RESULT(odc_initialise_api());

    int argi;

    // Iterate over all supplied path arguments
    for (argi = 1; argi < argc; argi++) {
        char* path = argv[argi];

        odc_reader_t* reader = NULL;
        odc_frame_t* frame = NULL;

        // Open current path and initialise frame
        CHECK_RESULT(odc_open_path(&reader, path));
        CHECK_RESULT(odc_new_frame(&frame, reader));

        fprintf(stdout, "File: %s\n", path);

        int i = 0;
        int rc;

        // Iterate over all frames in the stream in non-aggregated mode, without decoding them
        while ((rc = odc_next_frame(frame)) == ODC_SUCCESS) {
            long row_count;
            int column_count;

            // Get row and column counts
            CHECK_RESULT(odc_frame_row_count(frame, &row_count));
            CHECK_RESULT(odc_frame_column_count(frame, &column_count));

            fprintf(stdout, "  Frame: %d, Row count: %ld, Column count: %d\n", ++i, row_count, column_count);

            int nproperties;

            // Get number of properties encoded in the frame
            CHECK_RESULT(odc_frame_properties_count(frame, &nproperties));

            const char* key;
            const char* value;

            int i;
            for (i = 0; i < nproperties; i++) {

                // Get property key and value by its index
                CHECK_RESULT(odc_frame_property_idx(frame, i, &key, &value));

                fprintf(stdout, "  Property: %s => %s\n", key, value);
            }

            int col;

            // Iterate over frame columns
            for (col = 0; col < column_count; ++col) {
                const char* name;
                int type;
                int element_size;
                int bitfield_count;

                // Get column information
                CHECK_RESULT(odc_frame_column_attributes(frame, col, &name, &type, &element_size, &bitfield_count));

                const char* type_name;

                // Lookup column type name
                CHECK_RESULT(odc_column_type_name(type, &type_name));

                fprintf(stdout, "    Column: %d, Name: %s, Type: %s, Size: %d\n", col + 1, name, type_name,
                        element_size);

                // Process bitfields only
                if (type == ODC_BITFIELD) {
                    int bf;

                    for (bf = 0; bf < bitfield_count; ++bf) {
                        const char* bf_name;
                        int bf_offset;
                        int bf_size;

                        // Get bitfield information
                        CHECK_RESULT(odc_frame_bitfield_attributes(frame, col, bf, &bf_name, &bf_offset, &bf_size));

                        fprintf(stdout, "      Bitfield: %d, Name: %s, Offset: %d, Nbits: %d\n", bf + 1, bf_name,
                                bf_offset, bf_size);
                    }
                }
            }

            fprintf(stdout, "\n");
        }

        // Unsuccessful end of frame iteration
        if (rc != ODC_ITERATION_COMPLETE) {
            fprintf(stderr, "Error: %s\n", odc_error_string(rc));
            return 1;
        }

        CHECK_RESULT(odc_free_frame(frame));

        // Deallocate reader instance
        CHECK_RESULT(odc_close(reader));
    }

    return 0;
}
