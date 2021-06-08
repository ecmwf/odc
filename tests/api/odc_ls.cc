/**
 * To build this program, please make sure to reference linked libraries:
 *
 *     g++ -std=c++11 -leckit -lodccore -o odc-cpp-ls odc_ls.cc
 */

#include <iostream>
#include <iomanip>
#include <cstdint>
#include <vector>
#include <numeric>
#include <math.h>

#include "eckit/runtime/Main.h"

#include "odc/api/Odb.h"

void usage() {
    std::cerr << "Usage:\n    odc-cpp-ls <odb2 file>" << std::endl << std::endl;
}

void write_header(int i, odc::api::ColumnInfo col) {
    int padding = col.decodedSize - floor(log10(abs(i + 1))) + 2;
    std::cout << (i + 1) << ". " << std::left << std::setw(padding) << col.name << "\t";
}

void write_data(size_t nrows, size_t ncols, std::vector<odc::api::ColumnInfo> columnInfo,
                std::vector<odc::api::StridedData> strides, std::vector<int> nbits) {
    long integer_missing = odc::api::Settings::integerMissingValue();
    double double_missing = odc::api::Settings::doubleMissingValue();

    size_t row;

    for (row = 0; row < nrows; ++row) {

        size_t col;

        for (col = 0; col < ncols; ++col) {
            switch (columnInfo[col].type) {
                case odc::api::INTEGER: {
                    int64_t val = *reinterpret_cast<const int64_t*>(strides[col][row]);
                    if (val == integer_missing) {
                        std::cout << ".";
                    }
                    else {
                        std::cout << std::left << std::setw(columnInfo[col].decodedSize) << val;
                    }
                    break;
                }
                case odc::api::BITFIELD: {
                    int64_t val = *reinterpret_cast<const int64_t*>(strides[col][row]);
                    for (int bit = nbits[col]-1; bit >= 0; --bit) {
                        std::cout << ((val & (1 << bit)) ? "1" : "0");
                    }
                    break;
                }
                case odc::api::REAL:
                case odc::api::DOUBLE: {
                    double val = *reinterpret_cast<const double*>(strides[col][row]);
                    if (val == double_missing) {
                        std::cout << ".";
                    } else {
                        std::cout << std::left << std::setw(columnInfo[col].decodedSize) << val;
                    }
                    break;
                }
                    std::cout << *reinterpret_cast<const int64_t*>(strides[col][row]);
                    break;
                case odc::api::STRING:
                    std::cout << std::left << std::setw(columnInfo[col].decodedSize) <<
                        std::string(strides[col][row],
                                    ::strnlen(strides[col][row], columnInfo[col].decodedSize));
                    break;
                default:
                    ASSERT(false);
            };
            std::cout << "\t";
        }
        std::cout << std::endl;
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        usage();
        return 1;
    }

    char* path = argv[1];

    eckit::Main::initialise(argc, argv);

    // Initialise library

    odc::api::Settings::treatIntegersAsDoubles(false);

    // Open the specified ODB file

    odc::api::Reader reader(path);

    // Iterate through the frames

    odc::api::Frame frame;
    while ((frame = reader.next())) {

        // Properties of this frame

        size_t ncols = frame.columnCount();
        size_t nrows = frame.rowCount();
        const auto& columnInfo = frame.columnInfo();

        // Print headers & determine storage requirements

        size_t row_size = 0;
        size_t i;

        for (i = 0; i < frame.columnCount(); ++i) {
            const auto& col(columnInfo[i]);

            write_header(i, col);

            row_size += col.decodedSize;
        }

        std::cout << std::endl;

        // Allocate storage required

        size_t storage_size = row_size * nrows;
        std::vector<char> buffer(storage_size);

        // Decoder prerequisites

        std::vector<std::string> columns;
        std::vector<odc::api::StridedData> strides;
        std::vector<int> nbits;

        char* ptr = &buffer[0];
        for (const auto& col : columnInfo) {
            columns.push_back(col.name);
            strides.emplace_back(odc::api::StridedData{ptr, nrows, col.decodedSize, col.decodedSize});
            ptr += nrows * col.decodedSize;

            // Bits for formatting bitfields
            if (col.type == odc::api::BITFIELD) {
                nbits.push_back(
                    std::accumulate(col.bitfield.begin(), col.bitfield.end(), 0,
                                    [](int l, const odc::api::ColumnInfo::Bit& r) {
                                        return l + r.size;
                                    }));
            }
            else {
                nbits.push_back(0);
            }
        }

        ASSERT(ptr == (&buffer[0] + storage_size));

        // Decode the data

        int nthreads = 4;
        odc::api::Decoder decoder(columns, strides);
        decoder.decode(frame, nthreads);

        // Iterate through the decoded data, and print it

        write_data(nrows, ncols, columnInfo, strides, nbits);
    }

    return 0;
}

