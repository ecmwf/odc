/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

//#include "eckit/eckit.h"

//#include "odblib/Codec.h"
//#include "odblib/Column.h"
//#include "odblib/DataStream.h"
//#include "odblib/DispatchingWriter.h"
//#include "odblib/IteratorProxy.h"
//#include "odblib/RowsIterator.h"
//#include "odblib/FunctionExpression.h"
//#include "odblib/SQLBitfield.h"
//#include "odblib/SQLDatabase.h"
//#include "odblib/SQLDistinctOutput.h"
//#include "odblib/SQLODAOutput.h"
//#include "odblib/SQLOrderOutput.h"
//#include "odblib/SQLOutput.h"
//#include "odblib/SQLSelectFactory.h"
//#include "odblib/SQLSession.h"
//#include "odblib/TemplateParameters.h"
//#include "odblib/Writer.h"
//#include "odblib/WriterDispatchingIterator.h"
//#include "odblib/ShiftedBitColumnExpression.h"
//#include "odblib/ImportTool.h"
//#include "odblib/DataTable.h"
//#include "odblib/SQLDataTable.h"

using namespace eckit;

template class ThreadSingleton<odb::sql::SQLSelectFactory>;
static ThreadSingleton<odb::sql::SQLSelectFactory> instance_;

namespace odb {
namespace sql {

SQLSelectFactory::SQLSelectFactory()
: implicitFromTableSource_(0),
  implicitFromTableSourceStream_(0),
  database_(0),
  config_(SQLOutputConfig::defaultConfig()),
  maxColumnShift_(0),
  minColumnShift_(0),
  csvDelimiter_(odb::tool::ImportTool::defaultDelimiter())
{}

SQLSelectFactory& SQLSelectFactory::instance()
{
	ASSERT( &instance_.instance() != 0 );
	return instance_.instance();
}


std::string SQLSelectFactory::index(const std::string& columnName, const SQLExpression* index)
{
	if (index == 0)
		return columnName;

	bool missing = false;
	std::string idx = Translator<int,std::string>()(int(index->eval(missing)));
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
	if (! pshift->isConstant()) throw eckit::UserError("Value of shift operator must be constant");
	bool missing = false;
	
	// Internally shift is an index in the cyclic buffer of old values, so the shift value is negative.
	int shift = - pshift->eval(missing);

	if (shift > maxColumnShift_) maxColumnShift_ = shift;
	if (shift < minColumnShift_) minColumnShift_ = shift;

	std::string expandedColumnName( index(columnName, vectorIndex) );
	return bitfieldName.size()
		? (shift == 0 ? new BitColumnExpression(expandedColumnName, bitfieldName, table)
					  : new ShiftedColumnExpression<BitColumnExpression>(expandedColumnName, bitfieldName, table, shift, -shift))
		: (shift == 0 ? new ColumnExpression(expandedColumnName + table, table)
					  : new ShiftedColumnExpression<ColumnExpression>(expandedColumnName + table, table, shift, -shift));
}

SQLExpression* SQLSelectFactory::reshift(SQLExpression* e)
{
    if (e == 0) return 0;
    SQLExpression* r = e;
    ShiftedColumnExpression<BitColumnExpression>* c1 = dynamic_cast<ShiftedColumnExpression<BitColumnExpression>*>(e);
    if (c1) {
        int newShift = c1->shift() - minColumnShift_;
        ASSERT(newShift >= 0);
        r = newShift > 0
            ? new ShiftedColumnExpression<BitColumnExpression>(*c1, newShift, c1->nominalShift())
            : (new BitColumnExpression(*c1))->nominalShift(c1->nominalShift());
        delete c1;
        return r;
    } 

    ShiftedColumnExpression<ColumnExpression>* c2 = dynamic_cast<ShiftedColumnExpression<ColumnExpression>*>(e);
    if (c2) {
        int newShift = c2->shift() - minColumnShift_ ;
        ASSERT(newShift >= 0);
        r = newShift > 0
            ? new ShiftedColumnExpression<ColumnExpression>(*c2, newShift, c2->nominalShift())
            : (new ColumnExpression(*c2))->nominalShift(c2->nominalShift());
        delete c2;
        return r;
    } 

    BitColumnExpression* c3 = dynamic_cast<BitColumnExpression*>(e);
    if(c3) {
        r = new ShiftedColumnExpression<BitColumnExpression>(*c3, -minColumnShift_, 0);
        delete c3;
        return r;
    }

    ColumnExpression* c4 = dynamic_cast<ColumnExpression*>(e);
    if(c4) {
        r = new ShiftedColumnExpression<ColumnExpression>(*c4, -minColumnShift_, 0);
        delete c4;
        return r;
    }
    
    odb::sql::expression::function::FunctionExpression* f = dynamic_cast<odb::sql::expression::function::FunctionExpression*>(e);
    if (f) {
        reshift(f->args());
        return r;
    }

    Log::info() << "SQLSelectFactory::reshift: SKIP " << *e << std::endl;
    return r;
}

void SQLSelectFactory::reshift(Expressions& select)
{
	ostream& L(Log::debug());
	L << "reshift: maxColumnShift_ = " << maxColumnShift_ << std::endl;
	L << "reshift: minColumnShift_ = " << minColumnShift_ << std::endl;
	for (size_t i = 0; i < select.size(); ++i)
		L << "reshift: <- select[" << i << "]=" << *select[i] << std::endl;

	for (size_t i = 0; i < select.size(); ++i)
        select[i] = reshift(select[i]);

	L << std::endl;
	for (size_t i = 0; i < select.size(); ++i)
		L << "reshift: -> select[" << i << "]=" << *select[i] << std::endl;
}

void SQLSelectFactory::resolveImplicitFrom(SQLSession& session, std::vector<SQLTable*>& from)
{
    Log::debug() << "No <from> clause" << std::endl;

    SQLTable* table = implicitFromTableSource_ ? session.openDataHandle(*implicitFromTableSource_)
        : implicitFromTableSourceStream_ ? session.openDataStream(*implicitFromTableSourceStream_, csvDelimiter_) 
        : database_ ? database_->table("defaultTable")
        : session.currentDatabase().dualTable();
    from.push_back(table);
}

SQLSelect* SQLSelectFactory::create (bool distinct,
	Expressions select_list,
	std::string into,
	std::vector<SQLTable*> from,
	SQLExpression *where,
	Expressions group_by,
	std::pair<Expressions,std::vector<bool> > order_by)
{
    std::ostream& L(Log::info());

	if (where) L << "SQLSelectFactory::create: where = " << *where << std::endl;

	SQLSelect* r = 0;
	SQLSession& session = SQLSession::current();

	if (from.size() == 0) resolveImplicitFrom(session, from);

	Expressions select;
	for (ColumnDefs::size_type i = 0; i < select_list.size(); ++i)
	{
		L << "expandStars: " << *select_list[i] << std::endl;
		select_list[i]->expandStars(from, select);
	}

	ASSERT(maxColumnShift_ >= 0);
	ASSERT(minColumnShift_ <= 0);
	if (minColumnShift_ < 0) 
    {
        L << endl << "SELECT_LIST before reshifting:" << select << std::endl;
		reshift(select);
        L << "SELECT_LIST after reshifting:" << select << endl << std::endl;

        if (where)
        {
            L << endl << "WHERE before reshifting:" << *where << std::endl;
            where = reshift(where);
            L << "WHERE after reshifting:" << *where << endl << std::endl;
        }

        reshift(order_by.first);
    }

	maxColumnShift_ = 0;
	minColumnShift_ = 0;

	if (group_by.size())
		Log::info() << "GROUP BY clause seen and ignored. Non aggregated values on select list will be used instead." << std::endl;

	TemplateParameters templateParameters;
    std::string outputFile = (config_.outputFormat == "odb") ? config_.outputFile : into;
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
	maxColumnShift_ = 0;
	return r;
}


} // namespace sql
} // namespace odb
