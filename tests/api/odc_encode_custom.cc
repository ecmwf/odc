/**
 * To build this program, please make sure to reference linked libraries:
 *
 *     g++ -std=c++11 -leckit -lodccore -o odc-cpp-encode-custom odc_encode_custom.cc
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <iostream>

#include "eckit/io/FileHandle.h"
#include "eckit/runtime/Main.h"

#include "odc/api/Odb.h"

using namespace eckit;
using namespace odc::api;

// Bitfield constants
#define Ob00000001 1
#define Ob00001011 11
#define Ob01101011 107

void usage() {
    std::cerr << "Usage:\n    odc-cpp-encode-custom <odb2 output file>" << std::endl << std::endl;
}

void cycle_longs(long *list, int size, long *pool, int pool_size) {
    int index = 0;
    int i;

    for (i = 0; i < size; i++) {
        if (index == pool_size) index = 0;
        list[i] = pool[index];
        index++;
    }
}

void cycle_doubles(double *list, int size, double *pool, int pool_size) {
    int index = 0;
    int i;

    for (i = 0; i < size; i++) {
        if (index == pool_size) index = 0;
        list[i] = pool[index];
        index++;
    }
}

void create_scratch_data(size_t nrows, char data0[][8], int64_t data1[], char data2[][8], char data3[][16],
                         double data4[], int64_t data5[], double data6[], int64_t data7[]) {

    // Prepare the current date as an integer

    time_t rawtime;
    time(&rawtime);

    struct tm * timeinfo;
    timeinfo = localtime(&rawtime);

    int64_t date = 10000 * (timeinfo->tm_year + 1900) + 100 * (timeinfo->tm_mon + 1) + timeinfo->tm_mday;

    // Prepare the list of integer values, including the missing value

    long integer_pool[] = { 1234, 4321, Settings::integerMissingValue() };
    int integer_pool_size = sizeof(integer_pool)/sizeof(integer_pool[0]);

    long missing_integers[nrows];
    cycle_longs(missing_integers, nrows, integer_pool, integer_pool_size);

    // Prepare the list of double values, including the missing value

    double double_pool[] = { 12.34, 43.21, Settings::doubleMissingValue() };
    int double_pool_size = sizeof(double_pool)/sizeof(double_pool[0]);

    double missing_doubles[nrows];
    cycle_doubles(missing_doubles, nrows, double_pool, double_pool_size);

    // Prepare the list of bitfield values

    long bitfield_pool[] = { Ob00000001, Ob00001011, Ob01101011 };
    int bitfield_pool_size = sizeof(bitfield_pool)/sizeof(bitfield_pool[0]);

    long bitfield_values[nrows];
    cycle_longs(bitfield_values, nrows, bitfield_pool, bitfield_pool_size);

    // Fill in the passed data arrays with scratch values
    for (size_t i = 0; i < nrows; i++) {
        ASSERT(snprintf(data0[i], 8, "xxxx") == 4);  // expver
        data1[i] = date;  // date@hdr
        ASSERT(snprintf(data2[i], 7, "stat%02ld", i) == 6);  // statid@hdr
        ASSERT(snprintf(data3[i], 16, "0-12345-0-678%02ld", i) == 15);  // wigos@hdr
        data4[i] = 12.3456 * i;  // obsvalue@body
        data5[i] = missing_integers[i];  // integer_missing
        data6[i] = missing_doubles[i];  // double_missing
        data7[i] = bitfield_values[i];  // bitfield_column
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        usage();
        return 1;
    }

    // Get output path from command argument
    char* path = argv[1];

    // Initialise API
    Main::initialise(argc, argv);
    Settings::treatIntegersAsDoubles(false);

    // Define row count
    const size_t nrows = 20;

    // Allocate data array for each column
    char data0[nrows][8];
    int64_t data1[nrows];
    char data2[nrows][8];
    char data3[nrows][16];
    double data4[nrows];
    int64_t data5[nrows];
    double data6[nrows];
    int64_t data7[nrows];

    // Set up the allocated arrays with scratch data
    create_scratch_data(nrows, data0, data1, data2, data3, data4, data5, data6, data7);

    // Column `bitfield_column` is an integer with 4 bitfield values in it
    std::vector<ColumnInfo::Bit> bitfields = {
        // name, size, offset+=size(n-1)
        {"flag_a", 1, 0},
        {"flag_b", 2, 1},
        {"flag_c", 3, 3},
        {"flag_d", 1, 6},
    };

    // Define all column names, their types and sizes
    std::vector<ColumnInfo> columns = {
        ColumnInfo{std::string("expver"), ColumnType(STRING), 8, {}},
        ColumnInfo{std::string("date@hdr"), ColumnType(INTEGER), sizeof(int64_t), {}},
        ColumnInfo{std::string("statid@hdr"), ColumnType(STRING), 8, {}},
        ColumnInfo{std::string("wigos@hdr"), ColumnType(STRING), 16, {}},
        ColumnInfo{std::string("obsvalue@body"), ColumnType(REAL), sizeof(double), {}},
        ColumnInfo{std::string("integer_missing"), ColumnType(INTEGER), sizeof(int64_t), {}},
        ColumnInfo{std::string("double_missing"), ColumnType(REAL), sizeof(double), {}},
        ColumnInfo{std::string("bitfield_column"), ColumnType(BITFIELD), sizeof(int64_t), bitfields},
    };

    // Set a custom data layout and data array for each column
    std::vector<ConstStridedData> strides {
        // ptr, nrows, element_size, stride
        {data0, nrows, 8, 8},
        {data1, nrows, sizeof(int64_t), sizeof(int64_t)},
        {data2, nrows, 8, 8},
        {data3, nrows, 16, 16},
        {data4, nrows, sizeof(double), sizeof(double)},
        {data5, nrows, sizeof(int64_t), sizeof(int64_t)},
        {data6, nrows, sizeof(double), sizeof(double)},
        {data7, nrows, sizeof(int64_t), sizeof(int64_t)},
    };

    // Add some key/value metadata to the frame
    std::map<std::string, std::string> properties = {
        { "encoded_by", "odc_example" },
    };

    const Length length;

    FileHandle fh(path);
    fh.openForWrite(length);
    AutoClose closer(fh);

    // Encode ODB-2 into a data handle
    encode(fh, columns, strides, properties);

    std::cout << "Written " << nrows << " to " << path << std::endl;

    return 0;
}
