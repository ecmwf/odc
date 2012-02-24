#include "machine.h"
#include "SQLBitfield.h"
#include "SQLODAOutput.h"
#include "DataStream.h"
#include "SQLSelectFactory.h"
#include "SQLSession.h"
#include "TemplateParameters.h"
#include "Codec.h"
#include "Column.h"
#include "IteratorProxy.h"
#include "Writer.h"
#include "RowsIterator.h"
#include "WriterDispatchingIterator.h"
#include "DispatchingWriter.h"
#include "SQLOutput.h"
#include "SQLDistinctOutput.h"
#include "SQLOrderOutput.h"
#include "SQLDatabase.h"

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
			: database_->table("defaultTable");
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
