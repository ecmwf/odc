/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// This file contains examples of usage of public APIs.

#include <string>
#include <iostream>
#include <vector>

#include "eckit/filesystem/PathName.h"

#include "odc/tools/ImportTool.h"
#include "odc/Select.h"
#include "odc/Reader.h"
#include "odc/Writer.h"

#include "TestCase.h"

using namespace eckit;

namespace {

TEST(example_select_data_read_results)
{
    // Prepare input data
    const std::string data =
        R"(x:INTEGER,y:INTEGER,v:DOUBLE
        1,1,0.3
        1,1,0.2
        2,2,0.4
        2,2,0.1)";

    odc::tool::ImportTool::importText(data, std::string("example_select_data_read_results.odb"));

    odc::Select select("select x,min(v),max(v);", "example_select_data_read_results.odb");

    for (odc::Select::iterator it (select.begin()),
                               end (select.end());
         it != end;
         ++it)
    {
        double r0 = (*it)[0],
               r1 = (*it)[1],
               r2 = (*it)[2];

        std::cout << r0 << ", " << r1 << ", " << r2 << std::endl;
    }
}


TEST(example_read_data)
{
    // Prepare input data
    const std::string data = "x:INTEGER,y:INTEGER,v:DOUBLE\n" "1,1,0.3\n" "1,1,0.2\n" "2,2,0.4\n" "2,2,0.1\n";
    odc::tool::ImportTool::importText(data, std::string("example_read_data.odb"));

    odc::Reader o("example_read_data.odb");
    for (odc::Reader::iterator it (o.begin()),
                               end (o.end());
         it != end;
         ++it)
    {
        double r0 = (*it)[0],
               r1 = (*it)[1],
               r2 = (*it)[2];

        std::cout << r0 << ", " << r1 << ", " << r2 << std::endl;
    }
}

TEST(example_write_data)
{
    odc::core::MetaData metaData;
    metaData
        .addColumn("x", "INTEGER")
        .addColumn("y", "INTEGER")
        .addColumn("v", "DOUBLE");

    odc::Writer<> writer("example_write_data.odb");
    odc::Writer<>::iterator it (writer.begin());
    it->columns(metaData);
    it->writeHeader();

    for (size_t i (1); i <= 1000; ++i)
    {
        (*it)[0] = i;
        (*it)[1] = i*2;
        (*it)[2] = i*3;

        // Incrementing iterator moves coursor to the next row.
        ++it;
    }
}

/*
 * These have all been disabled as part of the elimination of ECML functionality
 * TODO: (SDS) Check if this really is to do with ecml
 *
 *
class ExampleCallback : public ecml::RequestHandler {
public:
    ExampleCallback(): ecml::RequestHandler("example_callback") {}
    virtual ecml::Values handle();
};

ecml::Values ExampleCallback::handle()
{
    // result_set is a variable SQL engine left in the environment for the callback
    std::string resultSetId (context.environment().lookup("result_set", "", context));
    std::cout << "ExampleCallback::handle: result_set: " << resultSetId << std::endl;
    if (! resultSetId.size())
        throw eckit::UserError("result_set not set");

    // callback may need some extra parameters. These can be set on the script level, see code of the example below.
    std::vector<std::string> extraParameters (context.environment().lookupList("extra_parameters", context));
    for (size_t i(0); i < extraParameters.size(); ++i)
        std::cout << "ExampleCallback::handle: extra_parameters[" << i << "]: " << extraParameters[i] << std::endl;

    odc::sql::ResultSet& resultSet (odc::sql::ResultSetStore::get(resultSetId));
    std::cout << "resultSet:" << resultSet << std::endl;
    std::vector<std::vector<double> >& rows (resultSet.rows());

    std::cout << "ExampleCallback:" << std::endl;
    for (size_t i(0); i < rows.size(); ++i)
    {
        std::vector<double>& row (rows[i]);
        for (size_t c(0); c < row.size(); ++c)
            std::cout << row[c] << ", ";
        std::cout << std::endl;
    }

    return new ecml::Cell("_list", "", 0, 0);
}

// Define a callback function by implementing and registering a class derived from ecml::RequestHandler
// Execute SQL SELECT statement using SQL verb, passing the new callback as a requests's parameter.
TEST(example_sql_select_callback)
{
    odc::tool::ImportTool::importText("x:INTEGER,y:INTEGER,v:DOUBLE\n"
                                       "1,1,0.3\n"
                                       "1,1,0.2\n"
                                       "2,2,0.4\n"
                                       "2,2,0.1\n",
                                       "example_sql_select_callback_input.odb");

    ecml::ExecutionContext context;
    odc::ODBModule odbModule;
    context.import(odbModule);

    ExampleCallback exampleCallback;
    context.registerHandler("example_callback", exampleCallback);
    
    context.execute(
    // This variable will be used by the calback
    "let, extra_parameters = 2015 / 2016 / 2017\n"

    "sql,"
    "  filter = 'select *',"
    "  source = 'example_sql_select_callback_input.odb',"
    "  target = 'example_sql_select_callback_output.odb',"
    "  callback = example_callback"
    );
}

// Run callback by registering it and executing as a verb in MARS script
TEST(example_sql_select_callback_invoked_as_a_request)
{
    odc::tool::ImportTool::importText("x:INTEGER,y:INTEGER,v:DOUBLE\n"
                                       "1,1,0.3\n"
                                       "1,1,0.2\n"
                                       "2,2,0.4\n"
                                       "2,2,0.1\n",
                                       "example_sql_select_callback_input.odb");

    ecml::ExecutionContext context;
    odc::ODBModule odbModule;
    context.import(odbModule);

    ExampleCallback exampleCallback;
    context.registerHandler("example_callback", exampleCallback);

    bool exceptionThrown (false);
    try {
        context.execute("example_callback, extra_parameters=1/2/3");
    } 
    catch (const eckit::UserError& e) {
        std::cout << " *** example_sql_select_callback: Exception thrown: " << e.what() << std::endl;
        exceptionThrown = true;
    }
    ASSERT(exceptionThrown);
}

// This example shows that a SQL callback can be a function defined in the MARS language.
// Our MARS function will call the example native function twice,
// each time with with different extra_parameters.
TEST(example_sql_select_and_a_mars_verb_as_a_callback)
{
    odc::tool::ImportTool::importText("x:INTEGER,y:INTEGER,v:DOUBLE\n"
                                       "1,1,0.3\n"
                                       "1,1,0.2\n"
                                       "2,2,0.4\n"
                                       "2,2,0.1\n",
                                       "example_sql_select_callback_input.odb");

    ecml::ExecutionContext context;
    odc::ODBModule odbModule;
    context.import(odbModule);

    ExampleCallback exampleCallback;
    context.registerHandler("example_callback", exampleCallback);
    
    context.execute(

    "function, of = result_set, \n"
    "          example_mars_language_callback = ("
    "            example_callback, result_set = (value,of=result_set), extra_parameters = 1 / 2 / 3 \n"
    "            example_callback, result_set = (value,of=result_set), extra_parameters = 1000 / 2000 / 3000 \n"
    "          ) \n"

    "sql,"
    "  filter = 'select *',"
    "  source = 'example_sql_select_callback_input.odb',"
    "  target = 'example_sql_select_callback_output.odb',"
    "  callback = example_mars_language_callback"
    );
}
*/

} // namespace

