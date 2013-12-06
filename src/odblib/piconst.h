/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file piconst.h
/// ECMWF July 2010

#ifndef piconst_H
#define piconst_H

#include <cmath>

namespace piconst {
//  const double  pi                = ((double)3.14159265358979323844e0);
  const double  pi                = atan2(1.0,1.0)*4;
  const double  half_pi           = (pi / 2);
  const double  two_pi            = (2 * pi);
  const double  four_pi           = (4 * pi);
  const double  pi_over_180       = (pi/180);
  const double  recip_pi_over_180 = (180/pi);
  const double  sphere_area       = four_pi; /* Actually: 4 * pi * R^2 */
}
#endif
