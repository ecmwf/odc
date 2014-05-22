/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/utils/Translator.h"
#include "eckit/thread/ThreadSingleton.h"
#include "odb_api/DispatchingWriter.h"
#include "odb_api/FunctionExpression.h"
#include "odb_api/ShiftedBitColumnExpression.h"
#include "odb_api/SQLDatabase.h"
#include "odb_api/SQLDistinctOutput.h"
#include "odb_api/SQLODAOutput.h"
#include "odb_api/SQLOrderOutput.h"
#include "odb_api/SQLSelectFactory.h"
#include "odb_api/SQLSelect.h"
#include "odb_api/SQLSession.h"
#include "odb_api/TemplateParameters.h"
#include "odb_api/Writer.h"

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
  csvDelimiter_(",")
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
    std::ostream& L(Log::debug());
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


SchemaAnalyzer& SQLSelectFactory::analyzer()
{ return SQLSession::current().currentDatabase().schemaAnalyzer(); }

MetaData SQLSelectFactory::columns(const std::string& tableName)
{
    const TableDef& tabledef ( analyzer().findTable(tableName) );
    const ColumnDefs& columnDefs ( tabledef.columns() );

#warning "TODO: Convert ColumnDefs (from tabledef) into MetaData and push it into the SQLODAOutput"

    ASSERT( false ); /// @todo this code must be fixed and return
}

SQLSelect* SQLSelectFactory::create (bool distinct,
	Expressions select_list,
	const std::string& into,
	std::vector<SQLTable*> from,
	SQLExpression *where,
	Expressions group_by,
    std::pair<Expressions,std::vector<bool> > order_by)
{
    //TODO: if(verbose_) {...}
    std::ostream& L(Log::debug());
    //std::ostream& L(Log::info());

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
        L << std::endl << "SELECT_LIST before reshifting:" << select << std::endl;
		reshift(select);
        L << "SELECT_LIST after reshifting:" << select << std::endl << std::endl;

        if (where)
        {
            L << std::endl << "WHERE before reshifting:" << *where << std::endl;
            where = reshift(where);
            L << "WHERE after reshifting:" << *where << std::endl << std::endl;
        }

        reshift(order_by.first);
    }

	maxColumnShift_ = 0;
	minColumnShift_ = 0;

	if (group_by.size())
		Log::info() << "GROUP BY clause seen and ignored. Non aggregated values on select list will be used instead." << std::endl;

    SQLOutput *out (createOutput(session, into, order_by.first.size()));

    if(distinct)             { out = new SQLDistinctOutput(out); }
    if(order_by.first.size()) { out = new SQLOrderOutput(out, order_by); }
    r = new SQLSelect(select, from, where, out, config_);

	maxColumnShift_ = 0;
	return r;
}

MetaData toODAColumns(const odb::sql::TableDef& tableDef)
{
    std::ostream& L(eckit::Log::info());

    L << "tableDef_ -> columns_" << std::endl;
    odb::sql::ColumnDefs columnDefs (tableDef.columns());
    MetaData md(0); //(columnDefs.size());
    for (size_t i(0); i < columnDefs.size(); ++i)
    {
        odb::sql::ColumnDef& c (columnDefs[i]);
        L << "   " << c.name() << ":" << c.type() << std::endl; //"(" << Column::columnTypeName(type) << ")" << std::endl;

        typedef DataStream<SameByteOrder, DataHandle> DS;
        ColumnType type (Column::type(c.type()));
        if (type == BITFIELD)
            md.addBitfield<DS>(c.name(), c.bitfieldDef());
        else
            md.addColumn<DS>(c.name(), c.type());

        ASSERT( &md[i]->coder() );
    }
    L << "toODAColumns ==> " << std::endl << md << std::endl;
    return md;
}

SQLOutput* SQLSelectFactory::createOutput (SQLSession& session, const std::string& into, size_t orderBySize)
{
    SQLOutput *r (NULL);

    TemplateParameters templateParameters;
    std::string outputFile = (config_.outputFormat() == "odb") ? config_.outputFile() : into;
    TemplateParameters::parse(outputFile, templateParameters);
	if (templateParameters.size())
	{
		// TODO? make the constant  (maxOpenFiles) passed to DispatchingWriter configurable
		DispatchingWriter writer(outputFile, orderBySize  ? 1 : 100);
        // TODO: use SQLSession::output
		r = (outputFile == "") ? session.defaultOutput() : new SQLODAOutput<DispatchingWriter::iterator>(writer.begin());
	} else {
        if (outputFile == "") r = session.defaultOutput();
        else {
            SchemaAnalyzer& a (session.currentDatabase().schemaAnalyzer());
            if (! a.tableKnown(outputFile)) 
                r = new SQLODAOutput<Writer<>::iterator>(Writer<>(outputFile).begin());
            else
            {
                Log::info() << "Table in the INTO clause known (" << outputFile << ")" << std::endl;
                const odb::sql::TableDef* tableDef (&a.findTable(outputFile));
                r = new SQLODAOutput<Writer<>::iterator>(Writer<>(outputFile).begin(), toODAColumns(*tableDef));
            } 
        }
    }
    return r;
}

} // namespace sql
} // namespace odb
