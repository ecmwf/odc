#include <unistd.h>

#include <iostream>
#include <string>

#include <netcdfcpp.h>

#include "odb_api/odbcapi.h"
#include "odb_api/CommandLineParser.h"
#include "odb_api/ODBModule.h"

#include "ecml/core/ExecutionContext.h"

#include "Odb2NetCDF.h"
#include "Odb2NetcdfModule.h"

/// @author Anne Fouilloux

using namespace std;
using namespace eckit;
using namespace odb;
using namespace odb::tool;

static const string usage = "Usage: odb2netcdf -i [odb_filename|odb_filename_prefix] [-2d] -o netcdf_filename";

int runScripts(const vector<string>& params)
{
    ecml::ExecutionContext context;
    ODBModule odbModule;
    Odb2NetcdfModule odb2NetcdfModule;
    context.import(odbModule);
    context.import(odb2NetcdfModule);

    for (size_t i(0); i < params.size(); ++i)
    {
        context.executeScriptFile(params[i]);
    }
    return 0;
}

int main(int argc, char *argv[])
{
    odb_start();

    CommandLineParser args(argc, argv);
    args.registerOptionWithArgument("-i");
    args.registerOptionWithArgument("-o");

    bool twoD (args.optionIsSet("-2d"));
    bool ecml (args.optionIsSet("-ecml"));
    string input (args.optionArgument<string>("-i", "")),
           output (args.optionArgument<string>("-o", ""));

    if (input.size() && output.size())
    {
        if (twoD)
        {
            Odb2NetCDF_2D converter (input, output);
            converter.convert();
        }
        else
        {
            Odb2NetCDF_1D converter (input, output);
            converter.convert();
        }
        return 0;
    }

    if (ecml)
    {
        std::vector<std::string> params(args.parameters());
        params.erase(params.begin());
        return runScripts(params);
    }

    cerr << usage << endl;

    return 1;
}

