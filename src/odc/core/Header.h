/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Piotr Kuchta
/// @date April 2009
/// @author Simon Smart
/// @date January 2019

#ifndef odc_core_Header_H
#define odc_core_Header_H

#include <cstddef>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "eckit/io/Buffer.h"
#include "eckit/memory/NonCopyable.h"

namespace eckit {
class DataHandle;
}


namespace odc {
namespace core {

class MetaData;
using Properties = std::map<std::string, std::string>;
using Flags      = std::vector<double>;

//----------------------------------------------------------------------------------------------------------------------

const int32_t BYTE_ORDER_INDICATOR = 1;
const uint16_t ODA_MAGIC_NUMBER    = 0xffff;

const int32_t FORMAT_VERSION_NUMBER_MAJOR = 0;
const int32_t FORMAT_VERSION_NUMBER_MINOR = 5;

//----------------------------------------------------------------------------------------------------------------------

class Header : private eckit::NonCopyable {

public:  // methods

    Header(MetaData& md, Properties& props);
    ~Header();

    size_t dataSize() const { return dataSize_; }

    size_t rowsNumber() const { return rowsNumber_; }

    int32_t byteOrder() { return byteOrder_; }

    /// read Magic loads the MAGIC from the data handle. Returns 0 for end of stream,
    /// and throws an exception if the magic is incorrect.
    static bool readMagic(eckit::DataHandle& dh);

    void loadAfterMagic(eckit::DataHandle& dh);

    static std::pair<eckit::Buffer, size_t> serializeHeader(size_t dataSize, size_t rowsNumber,
                                                            const Properties& properties, const MetaData& columns);

    static std::pair<eckit::Buffer, size_t> serializeHeaderOtherByteOrder(size_t dataSize, size_t rowsNumber,
                                                                          const Properties& properties,
                                                                          const MetaData& columns);

private:  // members

    template <typename ByteOrder>
    void load(eckit::DataHandle& dh);

    MetaData& md_;
    Properties& props_;
    size_t dataSize_;
    size_t rowsNumber_;

    int32_t byteOrder_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace odc


#endif  // odc_core_Header_H
