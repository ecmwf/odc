/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file OrderByExpressions.h
/// Piotr Kuchta - ECMWF Nov 11

#ifndef OrderByExpressions_H
#define OrderByExpressions_H

#include "eclib/machine.h"
#include <vector>

#include "odblib/Expressions.h"

namespace odb {
namespace sql {
namespace expression {

class OrderByExpressions : public Expressions
{
public:
	OrderByExpressions(const OrderByExpressions& o)
	: Expressions(o), ascending_(o.ascending_)
	{}

	OrderByExpressions(const std::vector<bool>& ascending) : ascending_(ascending) {}

	bool operator<(const OrderByExpressions&) const;


private:
	const std::vector<bool>& ascending_;
};

} // namespace expression
} // namespace sql
} // namespace odb

#endif
