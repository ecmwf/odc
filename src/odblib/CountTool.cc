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
#include "odblib/CountTool.h"
#include "odblib/MetaData.h"
#include "odblib/MetaDataReader.h"
#include "odblib/MetaDataReaderIterator.h"
#include "odblib/Reader.h"

using namespace eckit;

namespace odb {
namespace tool {

CountTool::CountTool (int argc, char *argv[]) : Tool(argc, argv) { }

unsigned long long CountTool::fastRowCount(const PathName &db)
{
	unsigned long long n = 0;

	typedef MetaDataReader<MetaDataReaderIterator> MDR;

	MDR mdReader(db);
	MDR::iterator it = mdReader.begin();
	MDR::iterator end = mdReader.end();
	for (; it != end; ++it)
	{
		MetaData &md = it->columns();
		n += md.rowsNumber();
	}
	return n;
}

unsigned long long CountTool::rowCount(const PathName &db)
{
	odb::Reader oda(db);
	odb::Reader::iterator i = oda.begin();
	odb::Reader::iterator end = oda.end();

	unsigned long long n = 0;
	for ( ; i != end; ++i)
		++n;
	return n;
}

void CountTool::run()
{
	if (parameters().size() < 2)
	{
		Log::error() << "Usage: ";
		usage(parameters(0), Log::error());
		Log::error() << std::endl;
		return;
	}

	//unsigned long long n = rowCount(parameters(1));
	unsigned long long n = fastRowCount(parameters(1));
	
	std::cout << n << std::endl;
}

} // namespace tool 
} // namespace odb 

