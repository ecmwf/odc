#include "Expressions.h"
#include "NumberExpression.h"
#include "StrStream.h"
#include "SQLType.h"
#include "SQLOutput.h"

namespace odb {
namespace sql {
namespace expression {

Expressions& Expressions::operator=(const Expressions& e)
{
	ExpressionsVector::operator=(e);
	return *this;
}

//Expressions * Expressions::clone() const
SQLExpression * Expressions::clone() const
{
	Expressions *r = new Expressions(this->size());
	for (size_t i = 0; i < this->size(); ++i)
		(*r)[i] = (*this)[i]->clone();

	return r;
}

void Expressions::release()
{
	for (size_t i = 0; i < this->size(); ++i)
		delete at(i);
}

void Expressions::print(ostream& o) const
{
	o << "[";
	for (size_t i = 0; i < size(); ++i)
	{
		at(i)->print(o);
		o << ",";
	}
	o << "]";
}

} // namespace expression
} // namespace sql
} // namespace odb

