/// @file   TestODBModule.cc
/// @author Piotr Kuchta

#include "odb_api/tools/TestCase.h"
#include "odb_api/tools/ToolFactory.h"
#include "odb_api/ODBModule.h"

#include "eckit/log/Log.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/ecml/core/ExecutionContext.h"

using namespace std;
using namespace odb;
using namespace eckit;

namespace {

struct ODBModule
{
    ODBModule()
    : odbModule(),
      context()
    {
        context.import(odbModule);
    }

    odb::ODBModule odbModule;
    eckit::ExecutionContext context;
};


TEST_FIXTURE(ODBModule, SQLSourceDoesNotExistThrows)
{
    bool exceptionThrown (false);
    try {
        context.execute(
        "sql,"
        " source = non_existing_file.odb,"
        " sql = 'select * where varno = 1',"
        " target = output.odb"
        );

    } catch (eckit::Exception& e) {
        exceptionThrown = true;
        string msg (e.what());
        Log::info() << "WHAT: " << msg << endl;
        CHECK(msg.find("non_existing_file.odb") != string::npos);
    }
    CHECK(exceptionThrown);
}

} // namespace
