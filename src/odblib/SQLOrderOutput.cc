/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odblib/SQLOrderOutput.h"
#include "odblib/SQLExpressionEvaluated.h"
#include "odblib/OrderByExpressions.h"

namespace odb {
namespace sql {

SQLOrderOutput::SQLOrderOutput(SQLOutput* output, const pair<Expressions,vector<bool> >& by)
: output_(output),
  by_(by)
{
	Log::info() << *this << endl;
}

SQLOrderOutput::~SQLOrderOutput() {}

void SQLOrderOutput::print(ostream& s) const
{
	s << "SQLOrderOutput[" << *output_ << " ORDER BY ";
	for(size_t i = 0; i < by_.first.size(); i++)
		s << *(by_.first[i]) << (by_.second[i] ? " ASC " : " DESC ") << ", ";
	s << "]";
}

void SQLOrderOutput::size(int count) { output_->size(count); }

unsigned long long SQLOrderOutput::count() { return output_->count(); }

void SQLOrderOutput::reset() { output_->reset(); }

void SQLOrderOutput::flush()
{
	for (SortedResults::iterator it = sortedResults_.begin(); it != sortedResults_.end(); ++it)
	{
		vector<Expressions>& rows = it->second;
		for (size_t i = 0; i < rows.size(); ++i)
			output_->output(rows[i]);
	}
	output_->flush();
}

bool SQLOrderOutput::output(const Expressions& results)
{
	OrderByExpressions byValues(by_.second);
	for (size_t i = 0; i < by_.first.size(); ++i)
		byValues.push_back(new SQLExpressionEvaluated(*by_.first[i]));

	Expressions resultValues;
	for (size_t i = 0; i < results.size(); ++i)
		resultValues.push_back(new SQLExpressionEvaluated(*results[i]));

	vector<Expressions>& otherResultsWithTheSameKey = sortedResults_[byValues];
	otherResultsWithTheSameKey.push_back(resultValues);
	
	return false;
}

void SQLOrderOutput::prepare(SQLSelect& sql)
{
	output_->prepare(sql);
	for(Expressions::iterator j = by_.first.begin(); j != by_.first.end() ; ++j)
		(*j)->prepare(sql);
}

void SQLOrderOutput::cleanup(SQLSelect& sql)
{
	output_->cleanup(sql);
	for(Expressions::iterator j = by_.first.begin(); j != by_.first.end() ; ++j)
		(*j)->cleanup(sql);
}

const SQLOutputConfig& SQLOrderOutput::config() { return output_->config(); }
void SQLOrderOutput::config(SQLOutputConfig& cfg) { output_->config(cfg); }


} // namespace sql
} // namespace odb
