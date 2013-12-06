/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

//#include "eckit/log/Log.h"
#include <eckit/eckit.h>
#include "odblib/SQLCountStar.h"
//#include "odblib/SQLAST.h"
//#include "odblib/SQLBitfield.h"
//#include "odblib/SchemaAnalyzer.h"
//#include "odblib/SQLDatabase.h"
//#include "odblib/SQLTable.h"

namespace odb {
namespace sql {

SQLCountStar::SQLCountStar(const std::string& table):
	table_(table)
{} 

SQLCountStar::~SQLCountStar() {}

void SQLCountStar::print(std::ostream& s) const
{
	s << "SELECT COUNT(*) FROM " << table_;
}

unsigned long long SQLCountStar::execute(SQLDatabase& db)
{
	unsigned long long n = db.table(table_)->noRows();	
	std::cout << n << std::endl;	
	return n;
}

} // namespace sql
} // namespace odb
