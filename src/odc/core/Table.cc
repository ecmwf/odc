/*
 * (C) Copyright 1996-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odc/core/Table.h"

#include <functional>
#include <bitset>

#include "eckit/io/AutoCloser.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/MemoryHandle.h"
#include "eckit/types/FixedString.h"

#include "odc/core/DecodeTarget.h"
#include "odc/MetaData.h"
#include "odc/Header.h"
#include "odc/Codec.h"

using namespace eckit;


namespace odc {
namespace core {

//----------------------------------------------------------------------------------------------------------------------

Table::Table(const ThreadSharedDataHandle& dh) :
    dh_(dh) {}

Offset Table::startPosition() const {
    return startPosition_;
}


Offset Table::nextPosition() const {
    return nextPosition_;
}

Length Table::encodedDataSize() const {
    return dataSize_;
}

size_t Table::numRows() const {
    return metadata_.rowsNumber();
}

size_t Table::numColumns() const {
    return metadata_.size();
}

int32_t Table::byteOrder() const {
    return byteOrder_;
}

const MetaData& Table::columns() const {
    return metadata_;
}

const Properties& Table::properties() const {
    return properties_;
}

Buffer Table::readEncodedData() {

    Buffer data(dataSize_);

    dh_.seek(dataPosition_);
    dh_.read(data, dataSize_);
    return data;
}

void Table::decode(DecodeTarget& target) {


    // Ensure there is sufficient space for decoding. Gives the target the
    // opportunity to allocate it if we desired.

//    target.ensureLength(numRows());

    // For now, we assume we are decoding everything!

    const MetaData& metadata(columns());
    std::vector<api::StridedData> facades(target.dataFacades()); // n.b. a copy

    size_t nrows = metadata.rowsNumber();
    size_t ncols = metadata.size();

    ASSERT(facades.size() == ncols);
    for (const auto& f : facades) ASSERT(f.nelem() == nrows);

    // Read the data in in bulk for this table

    const Buffer readBuffer(readEncodedData());
    MemoryHandle dh(readBuffer);
    AutoCloser<DataHandle> closer(dh);
    dh.openForRead();

    std::vector<std::reference_wrapper<codec::Codec>> decoders;
    decoders.reserve(ncols);
    for (auto& col : metadata) {
        decoders.push_back(col->coder());
        decoders.back().get().dataHandle(&dh);
    }
    // Do the decoding

    size_t lastStartCol = 0;
    size_t startCol = 0;
    std::vector<size_t> lastDecoded(ncols, 0);

    for (size_t rowCount = 0; rowCount < nrows; ++rowCount) {

        unsigned char marker[2];
        ASSERT(dh.read(&marker, sizeof(marker)) == 2);
        int startCol = (marker[0] * 256) + marker[1]; // Endian independant

        if (lastStartCol > startCol) {
            for (size_t col = startCol; col < lastStartCol; col++) {
                facades[col].fill(lastDecoded[col], rowCount-1);
            }
        }

        for (size_t col = startCol; col < ncols; col++) {
            decoders[col].get().decode(reinterpret_cast<double*>(facades[col][rowCount]));
            lastDecoded[col] = rowCount;
        }

        lastStartCol = startCol;
    }

    // And fill in any columns that are incomplete

    for (size_t col = 0; col < ncols; col++) {
        if (lastDecoded[col] < nrows-1) {
            facades[col].fill(lastDecoded[col], nrows-1);
        } else {
            break;
        }
    }
}


std::unique_ptr<Table> Table::readTable(odc::core::ThreadSharedDataHandle& dh) {

    Offset startPosition = dh.position();

    // Read the magic number. IF no more data, we are done

    FixedString<5> magic;
    long bytesRead = dh.read(&magic, sizeof(magic));

    if (bytesRead == 0) return 0;
    if (bytesRead != sizeof(magic)) throw ShortFile(dh.title(), Here());

    ASSERT(magic == "\xff\xffODA");

    // Load the header
    // TODO: Proxy class is silly. This could be done in a nicer way.

    struct ProxyClass {
        DataHandle& dataHandle() { return dh_; }
        MetaData& columns() { return md_; }
        DataHandle& dh_;
        MetaData& md_;
        Properties& properties_;
    };

    std::unique_ptr<Table> newTable(new Table(dh));

    MetaData md;
    Properties props;
    ProxyClass proxy{dh, newTable->metadata_, newTable->properties_};
    Header<ProxyClass> hdr(proxy);
    hdr.loadAfterMagic();

    newTable->startPosition_ = startPosition;
    newTable->dataPosition_ = dh.position();
    newTable->dataSize_ = hdr.dataSize();
    newTable->nextPosition_ = dh.position() + newTable->dataSize_;
    newTable->byteOrder_ = hdr.byteOrder();

    // Check that the ODB hasn't been truncated
    if (newTable->nextPosition_ > dh.estimate()) throw ShortFile(dh.title(), Here());

    return newTable;
}

//----------------------------------------------------------------------------------------------------------------------

}
}
