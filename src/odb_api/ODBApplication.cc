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

#include "odb_api/ODBApplication.h"

#include "eckit/config/Resource.h"
#include "eckit/log/OStreamTarget.h"
#include "eckit/log/TimeStampTarget.h"
#include "eckit/log/Colour.h"
#include "eckit/log/ColouringTarget.h"

using namespace eckit;

namespace odb {
namespace tool {

ODBApplication::ODBApplication (int argc, char **argv) :
    Application(argc,argv),
    clp_(argc, argv) {
}

ODBApplication::~ODBApplication() {}

CommandLineParser& ODBApplication::commandLineParser() { return clp_; }

static LogTarget* create_dhs_log_target()
{
    return new OStreamTarget(std::cerr);
}

static LogTarget* create_info_log_target() {
    LogTarget* target = create_dhs_log_target();
    return new TimeStampTarget("(I)", target);
}

static LogTarget* create_warning_log_target() {
    LogTarget* target = create_dhs_log_target();
    return new TimeStampTarget("(W)", new ColouringTarget(target, &Colour::yellow));
}

static LogTarget* create_error_log_target() {
    LogTarget* target = create_dhs_log_target();
    return new TimeStampTarget("(E)", new ColouringTarget(target, &Colour::red));
}

static LogTarget* create_debug_log_target() {
    LogTarget* target = create_dhs_log_target();
    return new TimeStampTarget("(D)", target);
}

LogTarget* ODBApplication::createInfoLogTarget() const {
    return create_info_log_target();
}

LogTarget* ODBApplication::createWarningLogTarget() const {
    return create_warning_log_target();
}

LogTarget* ODBApplication::createErrorLogTarget() const {
    return create_error_log_target();
}

LogTarget* ODBApplication::createDebugLogTarget() const {
    return create_debug_log_target();
}


} // namespace tool 
} // namespace odb 

