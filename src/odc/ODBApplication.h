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

#ifndef ODBApplication_H
#define ODBApplication_H

#include "eckit/runtime/Tool.h"
#include "odc/CommandLineParser.h"

namespace odc {
namespace tool {

class ODBApplication : public eckit::Tool {
public:

    ODBApplication(int argc, char** argv);
    virtual ~ODBApplication();

    CommandLineParser& commandLineParser();

private:

    virtual eckit::LogTarget* createInfoLogTarget() const;
    virtual eckit::LogTarget* createWarningLogTarget() const;
    virtual eckit::LogTarget* createErrorLogTarget() const;
    virtual eckit::LogTarget* createDebugLogTarget() const;

    CommandLineParser clp_;
};

}  // namespace tool
}  // namespace odc

#endif
