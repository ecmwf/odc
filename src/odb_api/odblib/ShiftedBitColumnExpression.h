/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File ShiftedBitColumnExpression.h
// Piotr Kuchta - ECMWF Dec 2012

#ifndef ShiftedBitColumnExpression_H
#define ShiftedBitColumnExpression_H

#include "odb_api/odblib/BitColumnExpression.h"
#include "odb_api/odblib/ShiftedColumnExpression.h"

namespace odb {
namespace sql {
namespace expression {

typedef ShiftedColumnExpression<BitColumnExpression> ShiftedBitColumnExpression;

} // namespace expression 
} // namespace sql
} // namespace odb 

#endif
