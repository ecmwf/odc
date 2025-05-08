/**
 * To build this program, please make sure to reference linked libraries:
 *
 *     g++ -std=c++11 -leckit -lodccore -o odc-cpp-index odc_index.cc
 */

#include "eckit/runtime/Main.h"

#include "odc/api/Odb.h"

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace odc::api;

void usage() {
    std::cerr << "Usage:\n    odc-cpp-index <odb2 file>" << std::endl << std::endl;
}

class Printer : public SpanVisitor {
    template <typename T>
    void prnt(const std::string& columnName, const std::set<T>& vals) {
        ASSERT(vals.size() == 1);
        std::cout << columnName << "=" << *vals.begin() << " ";
    }
    void operator()(const std::string& columnName, const std::set<long>& vals) override { prnt(columnName, vals); }
    void operator()(const std::string& columnName, const std::set<double>& vals) override { prnt(columnName, vals); }
    void operator()(const std::string& columnName, const std::set<std::string>& vals) override {
        prnt(columnName, vals);
    }
};

void write_index(Span& span, long offset, long length) {
    std::cout << "Archival unit: offset=" << offset << " length=" << length << std::endl;
    std::cout << "  Key: ";

    // Dump the index values without decoding the frame data
    Printer p;
    span.visit(p);

    std::cout << std::endl;
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

    bool aggregated = false;

    // Open supplied path in non-aggregated mode
    Reader reader(path, aggregated);

    // Define which columns will be used as index keys
    std::vector<std::string> index_keys{"key1", "key2", "key3"};

    Frame frame;
    Span lastSpan;
    long offset = 0;
    long length = 0;
    bool first  = true;

    // Iterate over frames
    while ((frame = reader.next())) {

        // Enforce the constant values constraint
        bool mustBeConstant = true;

        // Get index values for the frame
        Span span = frame.span(index_keys, mustBeConstant);

        // If the index values are the same, just increase the length
        if (span == lastSpan || first) {
            length += span.length();

            // Remember the first set of index values
            if (first)
                std::swap(lastSpan, span);
        }

        // If the index values differ, output the last set
        else {
            write_index(lastSpan, offset, length);

            // Reset offset and length counters
            offset = span.offset();
            length = span.length();

            // Remember the current set of index values
            std::swap(lastSpan, span);
        }

        first = false;
    }

    // Output last set of index values
    write_index(lastSpan, offset, length);

    return 0;
}
