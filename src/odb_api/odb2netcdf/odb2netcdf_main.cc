#include <unistd.h>

#include <iostream>
#include <string>

#include <netcdfcpp.h>

#include "odb_api/odbcapi.h"
#include "odb_api/CommandLineParser.h"
#include "odb_api/ODBModule.h"

#include "Odb2NetCDF.h"
#include "Odb2NetcdfModule.h"

/// @author Anne Fouilloux

using namespace std;
using namespace eckit;
using namespace odc;
using namespace odc::tool;

static const string usage = "Usage: odb2netcdf -i [odb_filename|odb_filename_prefix] [-2d] -o netcdf_filename";

int main(int argc, char *argv[])
{
    odb_start_with_args(argc, argv);

    CommandLineParser args(argc, argv);
    args.registerOptionWithArgument("-i");
    args.registerOptionWithArgument("-o");

    bool twoD (args.optionIsSet("-2d"));
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

    cerr << usage << endl;

    return 1;
}

