/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odblib/SQLSelect.h"
#include "odblib/SQLSimpleOutput.h"
#include "odblib/Decoder.h"

namespace odb {
namespace sql {

SQLSimpleOutput::SQLSimpleOutput(ostream& out):
	out_(out),
	count_(0)
{
	out_ << fixed;
}

SQLSimpleOutput::~SQLSimpleOutput() {}

void SQLSimpleOutput::print(ostream& s) const
{
	s << "SQLSimpleOutput";
}

ostream& SQLSimpleOutput::format(ostream& o, size_t i) const
{
	o.width(columnWidths_[i]);
	return o << *columnAlignments_[i];
}

void SQLSimpleOutput::size(int) {}
void SQLSimpleOutput::reset() { count_ = 0; }
void SQLSimpleOutput::flush() { out_ << std::flush; }

bool SQLSimpleOutput::output(const expression::Expressions& results)
{
	size_t n = results.size();
    for(size_t i = 0; i < n; i++)
    {
        if(i) out_ << config_.fieldDelimiter;
		currentColumn_ = i;
        results[i]->output(*this);
    }
    out_ << endl;
	count_++;
	return true;
}

void SQLSimpleOutput::outputReal(double x, bool missing) const
{
	format(out_, currentColumn_);
	if (missing && !config_.doNotWriteNULL)
		out_ << "NULL";
	else
		out_ << x;
}

void SQLSimpleOutput::outputDouble(double x, bool missing) const
{
	format(out_, currentColumn_);
	if (missing && !config_.doNotWriteNULL)
		out_ << "NULL";
	else
		out_ << x;
}

void SQLSimpleOutput::outputInt(double x, bool missing) const
{
	format(out_, currentColumn_);
	if (missing && !config_.doNotWriteNULL)
		out_ << "NULL";
	else
		out_ << static_cast<long long>(x);
}

void SQLSimpleOutput::outputUnsignedInt(double x, bool missing) const
{
	format(out_, currentColumn_);
	if (missing && !config_.doNotWriteNULL)
		out_ << "NULL";
	else
		out_ << static_cast<unsigned long>(x);
}

void SQLSimpleOutput::outputString(double x, bool missing) const
{
	format(out_, currentColumn_);
	if (missing && !config_.doNotWriteNULL)
		out_ << "NULL";
	else
	{
		stringstream ss;
		ss << "'";
		char *p = reinterpret_cast<char*>(&x);
		for(size_t i = 0; i < sizeof(x); i++)
			if(p[i] != ' ' && isprint(p[i]))
				ss << p[i];
		ss << "'";

		out_ << ss.str();
	}
}

void SQLSimpleOutput::outputBitfield(double x, bool missing) const
{
	format(out_, currentColumn_);
	if (missing && !config_.doNotWriteNULL)
		out_ << "NULL";
	else
	{
		stringstream ss;
		Decoder::printBinary(ss, static_cast<unsigned long>(x));
		out_ << ss.str();
	}
}

void SQLSimpleOutput::prepare(SQLSelect& sql)
{
	if (config_.doNotWriteColumnNames)
		return;

	const expression::Expressions& columns(sql.output());
	for (size_t i = 0; i < columns.size(); i++)
	{
		string name = columns[i]->title();
		const type::SQLType* type = columns[i]->type();

		columnWidths_.push_back(type->width());
		columnAlignments_.push_back(type->format());

		if(i) out_ << config_.fieldDelimiter;

		format(out_, i);
		//stringstream ss;
		//ss << name;
		//ss << ":" << type->name();
		//out_ << ss.str();
		out_ << name;
		
	}
    out_ << endl;
}

void SQLSimpleOutput::cleanup(SQLSelect& sql) {}
unsigned long long SQLSimpleOutput::count() { return count_; }

} // namespace sql
} // namespace odb
