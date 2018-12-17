/*
 * (C) Copyright 1996-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/types/FixedString.h"

#include "odc/core/Table.h"
#include "odc/MetaData.h"
#include "odc/Header.h"


namespace odc {
namespace core {

//----------------------------------------------------------------------------------------------------------------------

Table::Table() {}

Table::Table(const ThreadSharedDataHandle& dh, eckit::Offset startPosition, eckit::Offset nextPosition, MetaData&& md, Properties&& props) :
    dh_(dh),
    startPosition_(startPosition),
    nextPosition_(nextPosition),
    metadata_(std::move(md)),
    properties_(std::move(props)) {}

eckit::Offset Table::startPosition() const {
    return startPosition_;
}


eckit::Offset Table::nextPosition() const {
    return nextPosition_;
}

size_t Table::numRows() const {
    return metadata_.rowsNumber();
}


bool Table::readTable(odc::core::ThreadSharedDataHandle& dh, odc::core::Table& t) {

    t.startPosition_ = dh.position();

    // Read the magic number. IF no more data, we are done

    eckit::FixedString<5> magic;
    long bytesRead = dh.read(&magic, sizeof(magic));

    if (bytesRead != sizeof(magic)) return false;

    ASSERT(magic == "\xff\xffODA");

    // Load the header
    // TODO: Proxy class is silly. This could be done in a nicer way.

    struct ProxyClass {
        eckit::DataHandle& dataHandle() { return dh_; }
        MetaData& columns() { return md_; }
        eckit::DataHandle& dh_;
        MetaData& md_;
        Properties& properties_;
    };

    MetaData md;
    Properties props;
    ProxyClass proxy{dh, t.metadata_, t.properties_};
    Header<ProxyClass> hdr(proxy);
    hdr.loadAfterMagic();

    t.nextPosition_ = dh.position() + eckit::Length(hdr.dataSize());

    return true;
}

//----------------------------------------------------------------------------------------------------------------------

}
}
