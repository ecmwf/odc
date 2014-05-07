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

namespace odb {

class MDI {
public:
    static double realMDI() { return realMDI_; }
    static double integerMDI() { return integerMDI_; }

    /// We always use 0 as MDI of Bitfield columns.
    static double bitfieldMDI() { return 0; }

    static void realMDI(double v) { realMDI_ = v; }
    static void integerMDI(double v) { integerMDI_ = v; }

private:
    static double realMDI_;
    static double integerMDI_;
};

} // namespace odb

#endif
