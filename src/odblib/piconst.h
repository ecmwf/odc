/// \file piconst.h
/// ECMWF July 2010

#ifndef piconst_H
#define piconst_H

#include "math.h"

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
