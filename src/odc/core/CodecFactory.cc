/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odc/core/CodecFactory.h"

#include "eckit/exception/Exceptions.h"


namespace odc {
namespace core {

//----------------------------------------------------------------------------------------------------------------------

CodecFactory::CodecFactory() {}

CodecFactory::~CodecFactory() {}

CodecFactory& CodecFactory::instance() {
    static CodecFactory theInstance;
    return theInstance;
}

void CodecFactory::enregister(const std::string& name, CodecBuilderBase& builder) {

    std::lock_guard<std::mutex> lock(m_);

    ASSERT(builders_.find(name) == builders_.end());
    builders_.emplace(name, builder);
}

void CodecFactory::deregister(const std::string& name, CodecBuilderBase& builder) {

    std::lock_guard<std::mutex> lock(m_);

    auto it = builders_.find(name);
    ASSERT(it != builders_.end());
    builders_.erase(it);
}

CodecBuilderBase::CodecBuilderBase(const std::string& name) :
    name_(name) {
    CodecFactory::instance().enregister(name, *this);
}

CodecBuilderBase::~CodecBuilderBase() {
    CodecFactory::instance().deregister(name_, *this);
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace core
} // namespace odc
