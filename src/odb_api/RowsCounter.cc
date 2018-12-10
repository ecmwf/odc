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
#include "odb_api/MetaData.h"
#include "odb_api/MetaDataReader.h"
#include "odb_api/MetaDataReaderIterator.h"
#include "odb_api/Reader.h"
#include "odb_api/RowsCounter.h"

using namespace eckit;

namespace odc {

unsigned long long RowsCounter::fastRowCount(const PathName &db)
{
	unsigned long long n = 0;

	typedef MetaDataReader<MetaDataReaderIterator> MDR;

	MDR mdReader(db);
	MDR::iterator it = mdReader.begin();
	MDR::iterator end = mdReader.end();
	for (; it != end; ++it)
		n += it->columns().rowsNumber();
	return n;
}

unsigned long long RowsCounter::rowCount(const PathName &db)
{
	odc::Reader oda(db);
	odc::Reader::iterator i = oda.begin();
	odc::Reader::iterator end = oda.end();

	unsigned long long n = 0;
	for ( ; i != end; ++i)
		++n;
	return n;
}

} // namespace odc 

