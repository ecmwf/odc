/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/eckit.h"
#include "odb_api/odblib/Dictionary.h"

namespace odb {
namespace sql {
namespace expression {

// TODO: a special type for Dictionary?
const type::SQLType* Dictionary::type() const { return &type::SQLType::lookup("real"); }

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

void Dictionary::print(std::ostream& o) const
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

