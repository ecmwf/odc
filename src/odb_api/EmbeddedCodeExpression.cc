/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/exception/Exceptions.h"
#include "odb_api/ColumnExpression.h"
#include "odb_api/Expressions.h"
#include "odb_api/SQLTable.h"
#include "odb_api/EmbeddedCodeExpression.h"
#include "odb_api/StringTool.h"

using namespace eckit;

namespace odb {
namespace sql {
namespace expression {

const odb::sql::type::SQLType* EmbeddedCodeExpression::type() const { return &odb::sql::type::SQLType::lookup("string"); }

EmbeddedCodeExpression::EmbeddedCodeExpression(const std::string& text)
: text_(text)
{}

EmbeddedCodeExpression::EmbeddedCodeExpression(const EmbeddedCodeExpression& o)
: text_(o.text_)
{}

void EmbeddedCodeExpression::expandStars(const std::vector<SQLTable*>& tables, expression::Expressions& e)
{
    std::ostream& L(Log::debug());

	if (! StringTool::isColumnRegex(text_))
	{
        NOTIMP;
		//e.push_back(this);
		return;
	}

	unsigned int matched (0);
	for(std::vector<SQLTable*>::const_iterator j (tables.begin());  j != tables.end(); ++j)
	{
		SQLTable* table (*j);
		std::vector<std::string> names (table->columnNames());
		for(size_t i (0); i < names.size(); ++i)
		{
			const std::string& name (names[i]);
			if (! StringTool::matchEx(text_, name))
			{
				L << "EmbeddedCodeExpression::expandStars: skip '" << name << "'" << std::endl;
				continue;
			}
			
			L << "EmbeddedCodeExpression::expandStars: adding '" << name << "'" << std::endl;
			++matched;
			e.push_back(new ColumnExpression(name, table));
		}
	}
	if (! matched)
		throw eckit::UserError(std::string("No columns matching regex '") + text_ + "' found.");
	delete this;
}

SQLExpression* EmbeddedCodeExpression::clone() const { return new EmbeddedCodeExpression(text_); }

EmbeddedCodeExpression::~EmbeddedCodeExpression() {}

double EmbeddedCodeExpression::eval(bool& missing) const { NOTIMP; return 0 /*whatever*/; }

void EmbeddedCodeExpression::prepare(SQLSelect& sql) {}

void EmbeddedCodeExpression::cleanup(SQLSelect& sql) {}

void EmbeddedCodeExpression::output(std::ostream& s) const { s << text_; }

void EmbeddedCodeExpression::print(std::ostream& s) const { s << "'" << text_ << "'"; }

} // namespace expression
} // namespace sql
} // namespace odb 
