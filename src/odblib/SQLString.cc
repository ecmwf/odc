/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "SQLString.h"
#include "SQLOutput.h"

#include <ctype.h>

namespace odb {
namespace sql {
namespace type {

SQLString::SQLString(const string& name):
	SQLType(name)
{} 

SQLString::~SQLString() {}

size_t SQLString::size() const { return sizeof(double); }

void SQLString::output(SQLOutput& o, double d, bool missing) const
{
	o.outputString(d, missing);
}

static SQLString type("string");

} // namespace type 
} // namespace sql
} // namespace odb
