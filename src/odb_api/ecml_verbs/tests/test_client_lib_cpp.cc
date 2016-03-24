/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file test_client_lib.cc
///
/// This file contains example showing how to retrieve 
/// some ODB data from ODB Server using traditional ODB API iterator API.
///
///  Note: MarsClient lib must be linked to executable.
///
/// @author Piotr Kuchta, ECMWF, November 2015

#include <string>
#include <iostream>
#include <vector>

#include "odb_api/odb_api.h"

void test_retrieve_with_iterator_api()
{
    const std::string url ("local://retrieve,class=OD,"
                                           "date=20150218,"
                                           "time=1200,"
                                           "type=OFB,"
                                           "obsgroup=conv,"
                                           "reportype=16058,"
                                           "stream=oper,"
                                           "expver=qu12,"
                                           "odbPathNameSchema = '{date}/{time}/{reportype}.odb',",
                                           "database=localhost");
    odb::Select select("select varno,min(varno),max(varno);", url);

    unsigned long long rowNumber (0);
    for (odb::Select::iterator it (select.begin()),
                               end (select.end());
         it != end;
         ++it, ++rowNumber)
    {
        double r0 = (*it)[0], r1 = (*it)[1], r2 = (*it)[2];

        std::cout << r0 << ", " << r1 << ", " << r2 << std::endl;
    }

    eckit::Log::info() << "test_retrieve_with_iterator_api: retrieved " 
                << rowNumber  << " rows "
                << "from '" << url << "'" << std::endl;

    ASSERT(rowNumber == 1000);
}

int main(int argc, char** argv) 
{ 
    try {
        test_retrieve_with_iterator_api(); 
    } 
    catch (std::exception e) 
    {
        eckit::Log::info() << "test_retrieve_with_iterator_api: exception caught: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
