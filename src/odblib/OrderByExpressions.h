/// \file OrderByExpressions.h
/// Piotr Kuchta - ECMWF Nov 11

#ifndef OrderByExpressions_H
#define OrderByExpressions_H

#include <machine.h>
#include <vector>

#include "Expressions.h"

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
