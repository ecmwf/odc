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
#include "odb_api/Decoder.h"
#include "odb_api/Expressions.h"
#include "odb_api/SQLSelect.h"
#include "odb_api/SQLCallbackOutput.h"

namespace odb {
namespace sql {

SQLCallbackOutput::SQLCallbackOutput(SQLRowCallback& callback)
: callback_(callback),
  count_(0),
  values_(0)
{}

SQLCallbackOutput::~SQLCallbackOutput() {}

void SQLCallbackOutput::print(std::ostream& s) const
{
	s << "SQLCallbackOutput";
}

void SQLCallbackOutput::size(int) {}
void SQLCallbackOutput::reset() { count_ = 0; }
void SQLCallbackOutput::flush() { /*TODO?*/ }

bool SQLCallbackOutput::output(const expression::Expressions& results)
{
    size_t n = results.size();
    values_.resize(n);
    for(currentColumn_ = 0; currentColumn_ < n; ++currentColumn_)
        results[currentColumn_]->output(*this);

    MetaData md(values_.size()); // TODO
    void *aux = 0; // TODO
    callback_(&values_[0], values_.size(), md, aux);
    count_++;
    return true;
}

void SQLCallbackOutput::outputValue(double x, bool missing)
{
    values_[currentColumn_] = x;
}

void SQLCallbackOutput::outputReal(double x, bool missing) { outputValue(x, missing); }
void SQLCallbackOutput::outputDouble(double x, bool missing) { outputValue(x, missing); }
void SQLCallbackOutput::outputInt(double x, bool missing) { outputValue(x, missing); }
void SQLCallbackOutput::outputUnsignedInt(double x, bool missing) { outputValue(x, missing); }
void SQLCallbackOutput::outputString(double x, bool missing) { outputValue(x, missing); }
void SQLCallbackOutput::outputBitfield(double x, bool missing) { outputValue(x, missing); }

void SQLCallbackOutput::prepare(SQLSelect& sql)
{
    /*
	const expression::Expressions& columns(sql.output());
	for (size_t i (0); i < columns.size(); i++)
	{
		std::string name (columns[i]->title());
		const type::SQLType* type (columns[i]->type());
	}
    */
}

void SQLCallbackOutput::cleanup(SQLSelect& sql) {}
unsigned long long SQLCallbackOutput::count() { return count_; }

} // namespace sql
} // namespace odb
