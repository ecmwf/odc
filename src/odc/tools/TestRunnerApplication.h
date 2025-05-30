/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TestRunnerApplication.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#ifndef TestRunnerApplication_H
#define TestRunnerApplication_H

#include "odc/ODBApplication.h"

namespace odc {
namespace tool {
namespace test {

class TestRunnerApplication : public ODBApplication {
public:

    TestRunnerApplication(int argc, char** argv);
    virtual ~TestRunnerApplication();

    void run();
};

}  // namespace test
}  // namespace tool
}  // namespace odc

#endif
