/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

///
/// This file will be updated autmatically by the make.sms script
///

namespace odc {

class ODBAPIVersion {
public:

    static const char* version();
    static const char* gitsha1();
    static unsigned int formatVersionMajor();
    static unsigned int formatVersionMinor();

    static const char* installPrefix();
    static const char* buildDirectory();
};

}  // namespace odc
