/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "SQLInt.h"
#include "SQLOutput.h"

#include <iomanip>

namespace odb {
namespace sql {
namespace type {

SQLInt::SQLInt(const string& name):
	SQLType(name)
{} 

SQLInt::~SQLInt() {}

size_t SQLInt::size() const { return sizeof(long); }

void SQLInt::output(SQLOutput& o, double d, bool missing) const
{
	o.outputInt(d, missing);
}

static SQLInt integer("integer");

/*
static SQLInt_YYYYMMDD yyyymmdd;
static SQLInt_HHMMSS   hhmmss;

static SQLInt pk1int("pk1int");
static SQLInt pk9int("pk9int");
static SQLInt linkoffset_t("linkoffset_t");
static SQLInt linklen_t("linklen_t");
*/

} // namespace type 
} // namespace sql
} // namespace odb
