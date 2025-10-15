/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file ODBApplication.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#include "eckit/config/Resource.h"
#include "eckit/log/Colour.h"
#include "eckit/log/ColouringTarget.h"
#include "eckit/log/OStreamTarget.h"

#include "odc/ODBApplication.h"
#include "odc/ODBTarget.h"

using namespace eckit;

namespace odc {
namespace tool {

ODBApplication::ODBApplication(int argc, char** argv) : Tool(argc, argv), clp_(argc, argv) {}

ODBApplication::~ODBApplication() {}

CommandLineParser& ODBApplication::commandLineParser() {
    return clp_;
}

static LogTarget* cerr_target() {
    return new OStreamTarget(std::cerr);
}

eckit::LogTarget* ODBApplication::createInfoLogTarget() const {
    return new ODBTarget("(I)", cerr_target());
}
eckit::LogTarget* ODBApplication::createDebugLogTarget() const {
    return new ODBTarget("(D)", cerr_target());
}
eckit::LogTarget* ODBApplication::createErrorLogTarget() const {
    return new ODBTarget("(E)", new ColouringTarget(cerr_target(), &Colour::red));
}
eckit::LogTarget* ODBApplication::createWarningLogTarget() const {
    return new ODBTarget("(W)", new ColouringTarget(cerr_target(), &Colour::yellow));
}


}  // namespace tool
}  // namespace odc
