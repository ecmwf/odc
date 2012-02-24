#include "Dictionary.h"
#include "NumberExpression.h"
#include "StrStream.h"
#include "SQLType.h"
#include "SQLOutput.h"

namespace odb {
namespace sql {
namespace expression {

Dictionary& Dictionary::operator=(const Dictionary& e)
{
	Map::operator=(e);
	return *this;
}

SQLExpression * Dictionary::clone() const
{
	Dictionary *r = new Dictionary;
	for (Dictionary::const_iterator it = begin(); it != end(); ++it)
		(*r)[it->first] = it->second->clone();

	return r;
}

void Dictionary::release()
{
	for (Dictionary::const_iterator it = begin(); it != end(); ++it)
		delete it->second;
}

void Dictionary::print(ostream& o) const
{
	o << "{";
	for (Dictionary::const_iterator it = begin(); it != end(); ++it)
	{
		o << it->first << " : ";
		it->second->print(o);
		o << ", ";
	}
	o << "}";
}

} // namespace expression
} // namespace sql
} // namespace odb

