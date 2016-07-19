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

#include "ecml/parser/Request.h"
#include "ecml/parser/RequestParser.h"
#include "ecml/parser/Cell.h"

#include "ecml/core/Environment.h"
#include "ecml/core/Interpreter.h"

#include "odb_api/Decoder.h"
#include "odb_api/Expressions.h"
#include "odb_api/SQLSelect.h"
#include "odb_api/MetaData.h"
#include "odb_api/SQLSelectFactory.h"
#include "odb_api/SQLCallbackOutput.h"
#include "odb_api/ecml_data/ResultSet.h"
#include "odb_api/ecml_data/ResultSetStore.h"

using namespace eckit;
using namespace ecml;
using namespace std;

namespace odb {
namespace sql {

SQLCallbackOutput::SQLCallbackOutput(ecml::ExecutionContext& context)
{
    ecml::Environment& e (context.environment());

    Request callback ( e.lookup("callback") );

    Log::info() << "createOutput: callback: " << callback << endl;

    vector<string> sources (e.lookupList("source", context)),
                   targets (e.lookupList("target", context));

    ASSERT(sources.size() == 1);
    ASSERT(targets.size() == 1);

    string source (sources[0]),
           target (targets[0]);

    Log::info() << "createOutput: source: " << source << endl;
    Log::info() << "createOutput: target: " << target << endl;
}

SQLCallbackOutput::~SQLCallbackOutput() {}

void SQLCallbackOutput::print(std::ostream& s) const
{
	s << "SQLCallbackOutput";
}

void SQLCallbackOutput::size(int) {}
void SQLCallbackOutput::reset() { count_ = 0; }

void SQLCallbackOutput::flush(ecml::ExecutionContext* context)
{
    Log::info() << "SQLCallbackOutput::flush: resultSet: " << resultSet_ << endl;

    ASSERT(context);

    string callback ( context->environment().lookup("callback", "", *context) );
    ASSERT(callback.size());

    stringstream ss;
    ss << resultSet_;

    const string id (ss.str());

    ResultSetStore::put(id, resultSet_);

    Values values (new Cell("_list", "", new Cell("", id, 0, 0), 0));
    Request frame (new Cell("_frame", "SQLCallbackOutput", 0, new Cell("", "result_set", values, 0)));
    context->pushEnvironmentFrame(frame);


    string xxx ( context->environment().lookup("result_set", "XXXXX", *context) );
    ASSERT(xxx == id);

    Log::info() << " -------- SQLCallbackOutput::flush: set result_set to " << xxx << endl;

    // callback is a name of the verb we want to call(symbol). 
    context->interpreter().eval(RequestParser::parse(callback), *context);

    Log::info() << " -------- SQLCallbackOutput::flush: executed callback " << callback << endl;

    ResultSetStore::remove(id);
}

bool SQLCallbackOutput::output(const expression::Expressions& results, ecml::ExecutionContext* context)
{
    ostream& L(Log::info());

    vector<bool> mask;
    vector<double> row;
    for(size_t i(0); i < results.size(); ++i)
    {
        bool missing (false);
        double v (results[i]->eval(missing));

        row.push_back(v);
        mask.push_back(missing);
    }

    resultSet_.append(row, mask);
    return true;
}

void SQLCallbackOutput::outputValue(double x, bool missing) { NOTIMP; }
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

void SQLCallbackOutput::cleanup(SQLSelect& sql)
{
    //SQLSelectFactory::instance().reset(); //?
}

unsigned long long SQLCallbackOutput::count() { return count_; }

} // namespace sql
} // namespace odb
