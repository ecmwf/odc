/**
 * To build this program, please make sure to reference linked libraries:
 *
 *     g++ -std=c++11 -leckit -lodccore -o odc-cpp-header odc_header.cc
 */

#include <iostream>

#include "eckit/runtime/Main.h"

#include "odc/api/Odb.h"

void usage() {
    std::cerr << "Usage:\n    odc-cpp-header <odb2 file 1> [<odb2 file 2> ...]" << std::endl << std::endl;
}

using namespace eckit;
using namespace odc::api;

int main(int argc, char** argv) {
    if (argc < 2) {
        usage();
        return 1;
    }

    // Initialise API
    Main::initialise(argc, argv);

    int argi;

    // Iterate over all supplied path arguments
    for (argi = 1; argi < argc; argi++) {
        char* path = argv[argi];

        bool aggregated = false;

        // Open supplied path in non-aggregated mode
        Reader reader(path, aggregated);

        std::cout << "File: " << path << std::endl;

        Frame frame;
        int i = 0;

        // Iterate over all frames in the stream, without decoding them
        while ((frame = reader.next())) {
            std::cout << "  Frame: " << ++i << ", Row count: " << frame.rowCount()
                      << ", Column count: " << frame.columnCount() << std::endl;

            for (const auto& property : frame.properties()) {
                std::cout << "  Property: " << property.first << " => " << property.second << std::endl;
            }

            int j = 0;

            // Iterate over frame columns
            for (const auto& column : frame.columnInfo()) {
                std::cout << "    Column: " << ++j << ", Name: " << column.name
                          << ", Type: " << columnTypeName(column.type) << ", Size: " << column.decodedSize << std::endl;

                // Process bitfields only
                if (column.type == BITFIELD) {
                    int k = 0;

                    for (auto const& bf : column.bitfield) {
                        std::cout << "      Bitfield: " << ++k << ", Name: " << bf.name << ", Offset: " << bf.offset
                                  << ", Nbits: " << bf.size << std::endl;
                    }
                }
            }

            std::cout << std::endl;
        }
    }

    return 0;
}
