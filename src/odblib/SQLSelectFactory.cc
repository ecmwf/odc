/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eclib/machine.h"

#include "odblib/Codec.h"
#include "odblib/Column.h"
#include "odblib/DataStream.h"
#include "odblib/DispatchingWriter.h"
#include "odblib/IteratorProxy.h"
#include "odblib/RowsIterator.h"
#include "odblib/SQLBitfield.h"
#include "odblib/SQLDatabase.h"
#include "odblib/SQLDistinctOutput.h"
#include "odblib/SQLODAOutput.h"
#include "odblib/SQLOrderOutput.h"
#include "odblib/SQLOutput.h"
#include "odblib/SQLSelectFactory.h"
#include "odblib/SQLSession.h"
#include "odblib/TemplateParameters.h"
#include "odblib/Writer.h"
#include "odblib/WriterDispatchingIterator.h"
#include "odblib/ShiftedBitColumnExpression.h"

template class ThreadSingleton<odb::sql::SQLSelectFactory>;
static ThreadSingleton<odb::sql::SQLSelectFactory> instance_;

namespace odb {
namespace sql {

SQLSelectFactory::SQLSelectFactory()
: implicitFromTableSource_(0),
  implicitFromTableSourceStream_(0),
  database_(0),
  config_(SQLOutputConfig::defaultConfig())
{}

SQLSelectFactory& SQLSelectFactory::instance()
{
	ASSERT( &instance_.instance() != 0 );
	return instance_.instance();
}


string SQLSelectFactory::index(const string& columnName, const SQLExpression* index)
{
	if (index == 0)
		return columnName;

	bool missing = false;
	string idx = Translator<int,string>()(int(index->eval(missing)));
	ASSERT(! missing);
	return columnName + "_" + idx;
}

SQLExpression* SQLSelectFactory::createColumn(
	const std::string& columnName,
	const std::string& bitfieldName,
	const SQLExpression* vectorIndex,
	const std::string& table,
	const SQLExpression* pshift)
{
	if (! pshift->isConstant()) throw UserError("Value of shift operator must be constant");
	bool missing = false;
	int shift = pshift->eval(missing);

	maxColumnShift_ = shift > maxColumnShift_ ? shift : maxColumnShift_;
	//if (shift > 0) throw UserError("Shift operator can only be negative");

	if (bitfieldName.size())
		return shift == 0 ? new BitColumnExpression(columnName, bitfieldName, table) 
						  : new ShiftedBitColumnExpression(columnName, bitfieldName, table, -shift);
	else
		return shift == 0 ? new ColumnExpression(index(columnName, vectorIndex) + table, table)
						  : new ShiftedColumnExpression<ColumnExpression>(index(columnName, vectorIndex) + table, table, -shift);
}

SQLSelect* SQLSelectFactory::create (bool distinct,
	Expressions select_list,
	string into,
	vector<SQLTable*> from,
	odb::sql::expression::SQLExpression *where,
	Expressions group_by,
	pair<Expressions,vector<bool> > order_by)
{   
	if (where) Log::info() << "SQLSelectFactory::create: where = " << *where << endl;

	SQLSelect* r = 0;
	SQLSession& session = SQLSession::current();

	if (from.size() == 0)
	{
		Log::debug() << "No <from> clause" << endl;

		SQLTable* table = implicitFromTableSource_ ? session.openDataHandle(*implicitFromTableSource_)
			: implicitFromTableSourceStream_ ? session.openDataStream(*implicitFromTableSourceStream_)
			: database_ ? database_->table("defaultTable") : 0;
		if (table == 0)
			throw UserError("No table specified");
		from.push_back(table);
	}

	Expressions select;
	for (ColumnDefs::size_type i = 0; i < select_list.size(); i++)
	{
		Log::debug() << "expandStars: " << *select_list[i] << endl;
		select_list[i]->expandStars(from, select);
	}

	if (group_by.size())
	{
		// TODO:
		Log::info() << "ORDER BY clause seen and ignored. Non aggregated values on select list will be used instead." << endl;
	}

	TemplateParameters templateParameters;
    string outputFile = (config_.outputFormat == "odb") ? config_.outputFile : into;
	TemplateParameters::parse(outputFile, templateParameters);
	if (templateParameters.size() == 0)
	{
		Writer<> dump(outputFile);
		SQLOutput *out = (outputFile == "") ? session.defaultOutput() : new SQLODAOutput<Writer<>::iterator>(dump.begin());

		if(distinct) { out = new SQLDistinctOutput(out); }
		if(order_by.first.size()) { out = new SQLOrderOutput(out, order_by); }
		r = new SQLSelect(select, from, where, out, config_);
	}
	else
	{
		// TODO? make the constant  (maxOpenFiles) passed to DispatchingWriter configurable
		DispatchingWriter dump(outputFile, order_by.first.size() ? 1 : 100);
		SQLOutput *out = (outputFile == "") ? session.defaultOutput() : new SQLODAOutput<DispatchingWriter::iterator>(dump.begin());

		if(distinct)        { out = new SQLDistinctOutput(out); }
		if(order_by.first.size()) { out = new SQLOrderOutput(out, order_by); }
		r = new SQLSelect(select, from, where, out, config_);
	}
	return r;
}

} // namespace sql
} // namespace odb
