/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

///
/// \file FakeODBIterator.cc
///
/// @author Piotr Kuchta, Feb 2009

#include <strings.h>

extern "C" {
#include "odbdump.h"
}

#include "eckit/parser/Tokenizer.h"
#include "eckit/utils/Translator.h"

#include "odb_api/odb_api.h"
#include "odb_api/tools/Tool.h"

#include "odb_api/migrator/FakeODBIterator.h"
#include "odb_api/migrator/ODBIterator.h"


using namespace eckit;

namespace odb {
namespace tool {

FakeODBIterator::ConstParameters FakeODBIterator::ConstParameters::instance_ = FakeODBIterator::ConstParameters();

Assignments::Assignments(const std::string& s)
{
    Tokenizer splitAssignments(",");
    std::vector<std::string> assignments;
    splitAssignments(s, assignments);
	
    Tokenizer splitEq("=");
	for (size_t i = 0; i < assignments.size(); ++i)
	{
		std::vector<std::string> assignment;
		splitEq(assignments[i], assignment);
		ASSERT(assignment.size() == 2);

		push_back(make_pair(assignment[0], assignment[1]));
	}
}

void FakeODBIterator::ConstParameters::add(const Assignments& ass)
{
	Log::debug() << "FakeODBIterator::ConstParameters::add(const Assignments& ass)" << std::endl;
	for (size_t i = 0; i < ass.size(); ++i)
	{
		const Assignment &as = ass[i];
		std::string columnName = as.first;
		std::string value = as.second;

		ASSERT(value.size() > 0);

		if (Tool::isInQuotes(value))
			addString(columnName, Tool::unQuote(value));
		else if (value.find('.') == std::string::npos)
			addInteger(columnName, Translator<std::string, long>()(value));
		else
			addReal(columnName, Translator<std::string, double>()(value));
	}
}

void FakeODBIterator::ConstParameters::addInteger(const std::string& name, long v)
{
	Log::info() << "FakeODBIterator::ConstParameters::addInteger: " << name << " = " << v << std::endl;
	push_back(ConstParameter(name, v, odb::INTEGER));
}

void FakeODBIterator::ConstParameters::addReal(const std::string& name, double v)
{
	Log::info() << "FakeODBIterator::ConstParameters::addReal: " << name << " = " << v << std::endl;
	push_back(ConstParameter(name, v, odb::REAL));
}

void FakeODBIterator::ConstParameters::addString(const std::string& name, std::string v)
{
	Log::info() << "FakeODBIterator::ConstParameters::addString: " << name << " = '" << v << "'" << std::endl;
	push_back(ConstParameter(name, Tool::cast_as_double(v), odb::STRING));
}

FakeODBIterator::FakeODBIterator(const PathName& db, const std::string& sql)
: iterator_(db, sql),
  columns_(0),
  data_(0),
  constParameters_(FakeODBIterator::ConstParameters::instance())
{}

FakeODBIterator::~FakeODBIterator()
{
	delete [] data_;
}

odb::MetaData& FakeODBIterator::columns() {
	if (columns_.size() == 0)
	{
		columns_ = iterator_.columns();

		size_t i = constParameters_.size();
		while (i-- > 0)
		{
			std::string name = constParameters_[i].name;
			odb::ColumnType type = constParameters_[i].type;

			Log::debug() << "FakeODBIterator::columns: i = " << i << ", name=" << name << std::endl;

			odb::Column* col = new odb::Column(columns_);
			col->name(name);
			col->type<DataStream<SameByteOrder, DataHandle> >(type, false);

			columns_.insert(columns_.begin(), col);
		}

	}
	return columns_;
}

double* FakeODBIterator::data()
{
	if (data_ == 0)
		data_ = new double[columns_.size()];

	double* trueData = iterator_.data();

	size_t count = constParameters_.size();
	memcpy(data_ + count, trueData, (columns_.size() - count) * sizeof(double));

	for (size_t i = 0; i < count; ++i)
		data_[i] = constParameters_[i].value;
	
	return data_;
}

bool FakeODBIterator::isNewDataset()
{
	return iterator_.isNewDataset();
}

bool FakeODBIterator::next()
{
	bool r = iterator_.next();
	noMore_ = !r;
	return r;
}

} // namespace tool 
} //namespace odb 

