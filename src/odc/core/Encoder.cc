/*
 * (C) Copyright 1996-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odc/core/Encoder.h"

#include "odc/LibOdc.h"
#include "odc/codec/CodecOptimizer.h"
#include "odc/core/Header.h"

using namespace eckit;


namespace odc {
namespace core {

//----------------------------------------------------------------------------------------------------------------------

void encodeFrame(eckit::DataHandle& out, const std::vector<api::ColumnInfo>& columns,
                 const std::vector<api::ConstStridedData>& data, const std::map<std::string, std::string>& properties) {

    ASSERT(columns.size() == data.size());
    ASSERT(columns.size() > 0);
    MetaData md;

    size_t ncols = columns.size();
    size_t nrows = data[0].nelem();

    // Construct the default codecs

    md.setSize(ncols);
    for (size_t i = 0; i < ncols; ++i) {
        md[i]->name(columns[i].name);
        md[i]->type<SameByteOrder>(columns[i].type);
        ASSERT(columns[i].decodedSize % sizeof(double) == 0);
        md[i]->dataSizeDoubles(columns[i].decodedSize / sizeof(double));
        if (!columns[i].bitfield.empty()) {
            eckit::sql::BitfieldDef bf;
            for (const auto& bit : columns[i].bitfield) {
                bf.first.push_back(bit.name);
                bf.second.push_back(bit.size);
            }
            md[i]->bitfieldDef(bf);
        }
    }

    // Gather statistics over all the columns

    size_t maxRowSize = sizeof(uint16_t);  // all rows contain a marker

    for (size_t col = 0; col < ncols; ++col) {
        ASSERT(data[col].nelem() == nrows);
        Codec& coder(md[col]->coder());

        for (const char* d : data[col]) {
            coder.gatherStats(*reinterpret_cast<const double*>(d));
        }
        maxRowSize += data[col].dataSize();
    }

    // Optimise the codecs

    codec::CodecOptimizer().setOptimalCodecs<SameByteOrder>(md);

    // TODO: Sort the columns into the optimal order for encoding

    //    std::sort(md.begin(), md.end(), [](Column* a, Column* b) { ASSERT(false); });

    // TODO: Sort the data columns as well.
    //    ASSERT(false);

    // Encode the data
    const std::vector<api::ConstStridedData>& sortedData(data);
    std::vector<Codec*> coders;
    for (const auto& col : md)
        coders.push_back(&col->coder());

    Buffer encodedBuffer(maxRowSize * nrows);
    DataStream<SameByteOrder> encodedStream(encodedBuffer);

    for (size_t row = 0; row < nrows; row++) {
        size_t startCol = 0;

        if (row != 0) {
            for (; startCol < ncols; ++startCol) {
                if (sortedData[startCol].isNewValue(row))
                    break;
            }
        }

        // Write the marker
        uint8_t marker[2]{static_cast<uint8_t>((startCol / 256) % 256), static_cast<uint8_t>(startCol % 256)};
        encodedStream.writeBytes(marker, sizeof(marker));  // n.b. raw write

        // Write the updated values
        char* p = encodedStream.get();
        for (size_t col = startCol; col < ncols; col++) {
            p = coders[col]->encode(p, *reinterpret_cast<const double*>(sortedData[col].get(row)));
        }
        encodedStream.set(p);
    }

    // Encode the header

    Properties props{properties};
    props["encoder"]                        = std::string("odc version ") + LibOdc::instance().version();
    std::pair<Buffer, size_t> encodedHeader = Header::serializeHeader(encodedStream.position(), nrows, props, md);

    // And output the data

    ASSERT(out.write(encodedHeader.first, encodedHeader.second) == long(encodedHeader.second));
    ASSERT(out.write(encodedBuffer, encodedStream.position()) == encodedStream.position());
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace odc
