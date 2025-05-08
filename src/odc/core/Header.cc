/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odc/core/Header.h"

#include "eckit/io/Buffer.h"
#include "eckit/io/DataHandle.h"
#include "eckit/log/Log.h"
#include "eckit/types/FixedString.h"
#include "eckit/utils/MD5.h"

#include "odc/LibOdc.h"
#include "odc/ODBAPISettings.h"
#include "odc/core/DataStream.h"
#include "odc/core/Exceptions.h"
#include "odc/core/MetaData.h"

using namespace eckit;

namespace odc {
namespace core {

//----------------------------------------------------------------------------------------------------------------------

Header::Header(MetaData& md, Properties& props) :
    md_(md), props_(props), dataSize_(0), rowsNumber_(0), byteOrder_(BYTE_ORDER_INDICATOR) {}

Header::~Header() {}

bool Header::readMagic(DataHandle& dh) {

    eckit::FixedString<5> magic;

    long bytesRead = dh.read(&magic, sizeof(magic));
    if (bytesRead == 0 || bytesRead == EOF)
        return false;
    if (bytesRead != sizeof(magic))
        throw ODBIncomplete(dh.title(), Here());
    if (magic != "\xff\xffODA")
        throw ODBInvalid(dh.title(), "Incorrect MAGIC", Here());
    return true;
}

template <typename ByteOrder>
void Header::load(DataHandle& dh) {

    // There must be at least 56 bytes available to read the basic header.

    constexpr size_t basic_header_size = 12 + 32 + 4;
    char basicBuffer[basic_header_size];

    if (dh.read(basicBuffer, sizeof(basicBuffer)) != sizeof(basicBuffer))
        throw ODBIncomplete(dh.title(), Here());

    DataStream<ByteOrder> ds1(basicBuffer, sizeof(basicBuffer));

    int32_t formatVersionMajor;
    ds1.read(formatVersionMajor);
    ASSERT("File format version not supported" && formatVersionMajor <= FORMAT_VERSION_NUMBER_MAJOR);

    int32_t formatVersionMinor;
    ds1.read(formatVersionMinor);
    ASSERT("File format version not supported" && formatVersionMinor <= FORMAT_VERSION_NUMBER_MINOR &&
           formatVersionMinor > 3);

    std::string headerDigest;
    ds1.read(headerDigest);

    int32_t headerSize;
    ds1.read(headerSize);

    // Read the remaining header data

    eckit::Buffer buffer(headerSize);
    if (dh.read(buffer, headerSize) != headerSize)
        throw ODBIncomplete(dh.title(), Here());

    // Calculate the MD5

    MD5 md5;
    md5.add(buffer.data(), buffer.size());
    std::string actualHeaderDigest = md5.digest();
    if (headerDigest != actualHeaderDigest)
        throw ODBInvalid(dh.title(), "Header digest incorrect", Here());

    DataStream<ByteOrder> ds2(buffer, buffer.size());

    // 0 means we don't know offset of next header.
    int64_t nextFrameOffset;
    ds2.read(nextFrameOffset);
    dataSize_ = nextFrameOffset;
    md_.dataSize(dataSize_);

    // Reserved, not used yet.
    int64_t prevFrameOffset;
    ds2.read(prevFrameOffset);
    ASSERT(prevFrameOffset == 0);

    // TODO: increase file format version

    int64_t numberOfRows;
    ds2.read(numberOfRows);
    rowsNumber_ = numberOfRows;
    md_.rowsNumber(rowsNumber_);

    LOG_DEBUG_LIB(LibOdc) << "Header::load: numberOfRows = " << numberOfRows << std::endl;

    // Flags -> ODAFlags
    Flags flags;
    ds2.read(flags);

    ds2.read(props_);

    md_.load(ds2);
}

void Header::loadAfterMagic(DataHandle& dh) {

    if (dh.read(&byteOrder_, sizeof(byteOrder_)) != sizeof(byteOrder_)) {
        throw ODBIncomplete(dh.title(), Here());
    }

    if (byteOrder_ != BYTE_ORDER_INDICATOR) {
        load<OtherByteOrder>(dh);
    }
    else {
        load<SameByteOrder>(dh);
    }
}

namespace {


template <typename ByteOrder>
std::pair<eckit::Buffer, size_t> serializeHeaderInternal(size_t dataSize, size_t rowsNumber,
                                                         const Properties& properties, const MetaData& columns) {

    // Serialise the variable size part of the header first. Use the configured buffer size
    // but allow expansion if needed.

    constexpr size_t initial_header_size = 9 + 8 + 4 + 32 + 4;

    eckit::Buffer buffer(ODBAPISettings::instance().headerBufferSize());
    bool serialised           = false;
    char* variableHeaderStart = 0;
    int32_t variableHeaderSize;

    while (!serialised) {
        try {
            variableHeaderStart = buffer + initial_header_size;
            DataStream<ByteOrder> ds(variableHeaderStart, buffer.size() - initial_header_size);

            ds.write(static_cast<int64_t>(dataSize));    // Reserved: nextFrameOffset
            ds.write(static_cast<int64_t>(0));           // Reserved: prevFrameOffset
            ds.write(static_cast<int64_t>(rowsNumber));  // num. rows

            Flags flags(0);  // n.b. flags unused
            ds.write(flags);

            ds.write(properties);

            columns.save(ds);

            serialised         = true;
            variableHeaderSize = ds.position();
        }
        catch (ODBEndOfDataStream& e) {
            buffer = eckit::Buffer(buffer.size() * 2);
        }
    }

    // Calculate MD5 of the variable portion of header data

    MD5 md5;
    md5.add(variableHeaderStart, variableHeaderSize);
    std::string headerDigest = md5.digest();

    // Now Serialise everything into the final buffer

    DataStream<ByteOrder> ds(buffer.data(), initial_header_size);

    // Header.
    ds.write(static_cast<uint16_t>(ODA_MAGIC_NUMBER));  // MAGIC

    ds.write('O');  // MAGIC
    ds.write('D');  // MAGIC
    ds.write('A');  // MAGIC

    ds.write(static_cast<int32_t>(BYTE_ORDER_INDICATOR));
    ds.write(static_cast<int32_t>(FORMAT_VERSION_NUMBER_MAJOR));
    ds.write(static_cast<int32_t>(FORMAT_VERSION_NUMBER_MINOR));

    ds.write(headerDigest);  // MD5

    ds.write(static_cast<int32_t>(variableHeaderSize));  // How much header data follows

    ASSERT(ds.position() == eckit::Offset(initial_header_size));

    return std::make_pair(std::move(buffer), variableHeaderSize + ds.position());
}

}  // namespace

std::pair<Buffer, size_t> Header::serializeHeader(size_t dataSize, size_t rowsNumber, const Properties& properties,
                                                  const MetaData& columns) {
    return serializeHeaderInternal<SameByteOrder>(dataSize, rowsNumber, properties, columns);
}

std::pair<Buffer, size_t> Header::serializeHeaderOtherByteOrder(size_t dataSize, size_t rowsNumber,
                                                                const Properties& properties, const MetaData& columns) {
    return serializeHeaderInternal<OtherByteOrder>(dataSize, rowsNumber, properties, columns);
}


//----------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace odc
