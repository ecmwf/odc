/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file UnitTest.h
///
/// @author Piotr Kuchta, ECMWF, June 2009

#include "odc/DispatchingWriter.h"
#include "odc/Reader.h"
#include "odc/StringTool.h"

#include "CountTool.h"
#include "TestCase.h"

using namespace std;
using namespace eckit;
using namespace odc;

/// UnitTest DispatchingWriter
///
static void test() {
    const string fileName = "2000010106-reduced.odb";

    odc::Reader oda(fileName);
    odc::Reader::iterator it        = oda.begin();
    const odc::Reader::iterator end = oda.end();
    ASSERT(it->columns().size() > 0);

    odc::DispatchingWriter writer("disp.{obstype}.{sensor}.odb");
    odc::DispatchingWriter::iterator wi = writer.begin();
    unsigned long long n1               = wi->pass1(it, end);
    wi->close();

    unsigned long long sum = 0;
    vector<PathName> files = (**wi).outputFiles();
    for (size_t i = 0; i < files.size(); ++i) {
        unsigned long long n = odc::tool::CountTool::rowCount(files[i]);
        Log::info() << i << ". " << files[i] << ": " << n << std::endl;
        sum += n;
    }

    ASSERT(n1 == sum);
    odc::StringTool::shell("ls -l disp.*.*.odb", Here());
}


static void setUp() {}
static void tearDown() {}

SIMPLE_TEST(DispatchingWriter)
