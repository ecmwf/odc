/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odc/core/Codec.h"

#include "eckit/exception/Exceptions.h"

#include "odc/core/CodecFactory.h"

using namespace eckit;

namespace odc {
namespace core {

//----------------------------------------------------------------------------------------------------------------------

Codec::Codec(const std::string& name)
: name_(name),
  hasMissing_(false),
  missingValue_(odc::MDI::realMDI()),
  min_(missingValue_),
  max_(missingValue_)
{}

std::unique_ptr<Codec> Codec::clone()
{
    auto c = CodecFactory::instance().build<SameByteOrder>(name_);
    c->hasMissing_ = hasMissing_;
    c->missingValue_ = missingValue_;
    c->min_ = min_;
    c->max_ = max_;
    return c;
}

Codec::~Codec() {}

void Codec::setDataStream(GeneralDataStream& ds) {
    if (ds.isOther()) {
        setDataStream(ds.other());
    } else {
        setDataStream(ds.same());
    }
}

void Codec::setDataStream(DataStream<SameByteOrder>&) {
    throw eckit::SeriousBug("Mismatched byte order between DataStream and Codec", Here());
}

void Codec::setDataStream(DataStream<OtherByteOrder>&) {
    throw eckit::SeriousBug("Mismatched byte order between DataStream and Codec", Here());
}

void Codec::load(GeneralDataStream& ds) {
    if (ds.isOther()) {
        load(ds.other());
    } else {
        load(ds.same());
    }
}

void Codec::load(DataStream<SameByteOrder>&) {
    throw eckit::SeriousBug("Mismatched byte order between DataStream and Codec", Here());
}

void Codec::load(DataStream<OtherByteOrder>&) {
    throw eckit::SeriousBug("Mismatched byte order between DataStream and Codec", Here());
}

void Codec::save(GeneralDataStream& ds) {
    if (ds.isOther()) {
        save(ds.other());
    } else {
        save(ds.same());
    }
}

void Codec::save(DataStream<SameByteOrder>&) {
    throw eckit::SeriousBug("Mismatched byte order between DataStream and Codec", Here());
}

void Codec::save(DataStream<OtherByteOrder>&) {
    throw eckit::SeriousBug("Mismatched byte order between DataStream and Codec", Here());
}

void Codec::missingValue(double v)
{
    ASSERT("Cannot change missing value after encoding of column data started" && (min_ == missingValue_) && (max_ == missingValue_));
    min_ = max_ = missingValue_ = v;
}

void Codec::gatherStats(const double& v)
{
    if(v == missingValue_)
        hasMissing_ = 1;
    else
    {
        if(v < min_ || min_ == missingValue_)
            min_ = v;
        if(v > max_ || max_ == missingValue_)
            max_ = v;
    }
}

void Codec::print(std::ostream& s) const {
    s << name_
      << ", range=<" << std::fixed << min_ << "," << max_ << ">";

    if (hasMissing_) {
        s << ", missingValue=" << missingValue_;
    }
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace core
} // namespace odc

