/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file MDI.h
/// @author Piotr Kuchta - ECMWF Nov 13

#ifndef MDI_H
#define MDI_H

namespace odc {

class MDI {
public:

    static double realMDI() { return realMDI_; }
    static double integerMDI() { return integerMDI_; }

    // Historically this used 0 as the missing value. This seems ... wrong. Really wrong. No bits
    // set is not the same as a missing value (see ODB-493)
    static double bitfieldMDI() { return integerMDI(); }

    static void realMDI(double v) { realMDI_ = v; }
    static void integerMDI(double v) { integerMDI_ = v; }

private:

    static double realMDI_;
    static double integerMDI_;
};

}  // namespace odc

#endif
