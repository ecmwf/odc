/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/config/Resource.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "odblib/Comparator.h"
#include "odblib/Reader.h"
#include "odblib/StringTool.h"
#include "odblib/Tracer.h"

using namespace std;
using namespace eckit;

class ValuesDifferent : public Exception {
public:
	ValuesDifferent(const std::string& what) : Exception(what) {}
};


namespace odb {

Comparator::Comparator(bool checkMissingFlag)
: nRow_(0),
  checkMissingFlag_(checkMissingFlag),
  NaN_isOK_(Resource<bool>("$ODB_API_NAN_IS_OK", false))
{}


void Comparator::compare(const PathName& p1, const PathName& p2)
{
	std::vector<std::string> noExcludedColumnTypes;
	compare(p1, p2, noExcludedColumnTypes);
}

void Comparator::compare(const PathName& p1, const PathName& p2, const std::vector<std::string>& excludedColumnsTypes)
{
    Tracer t(Log::debug(), std::string("Comparator::compare: ") + p1 + ", " + p2);

	odb::Reader oda1(p1);
	odb::Reader oda2(p2);

	odb::Reader::iterator it1(oda1.begin());
	odb::Reader::iterator end1(oda1.end());
	odb::Reader::iterator it2(oda2.begin());
	odb::Reader::iterator end2(oda2.end());
	
	compare(it1, end1, it2, end2, p1, p2, excludedColumnsTypes);
}

void Comparator::compare(int nCols, const double *data1, const double *data2, const MetaData& md1, const MetaData& md2)
{
	for (int i=0; i < nCols; i++)
		try
		{
			const Column& column(*md1[i]);
			ColumnType type(column.type());
			switch (type)
			{
				case INTEGER:
				case BITFIELD:
				case STRING:
				case DOUBLE:
					if (! (same(data1[i], data2[i]) || (NaN_isOK_ && (isnan(data1[i]) && isnan(data2[i])))))
					{
						stringstream ss;
						ss << "Values different: " 
						<< " data1[" << i << "]==" << StringTool::valueAsString(data1[i], type)
						<< " data2[" << i << "]==" << StringTool::valueAsString(data2[i], type)  << std::endl;
						throw ValuesDifferent(ss.str());
					}
					break;
				case REAL:
					ASSERT(same(float(data1[i]), float(data2[i])) || (NaN_isOK_ && (isnan(data1[i]) && isnan(data2[i]))));
					break;
				case IGNORE:
				default:
					ASSERT(!"Unknown type");
					break;
			}
		} catch (Exception &e) {
			Log::info() << "While comparing rows number " << nRow_ << ", columns " << i
				<< " found different." << std::endl;
			Log::info() << " " << e.what() << std::endl;

			Log::info() << " data1[" << i << "] = " << std::scientific << data1[i] << std::endl;
			Log::info() << " data2[" << i << "] = " << std::scientific << data2[i] << std::endl;

			Log::info() << " md1[" << i << "] = " << *md1[i] << std::endl;
			Log::info() << " md2[" << i << "] = " << *md2[i] << std::endl;

			//TODO: make it an option to stop when an error found
			//throw;
		}
}


void Comparator::compare(const MetaData& metaData1, const MetaData& metaData2, const std::vector<std::string>& exColumnsTypes)
{
	ASSERT("Number of columns must be the same" && (metaData1.size() == metaData2.size()));

	std::set<std::string> excludedColumnsTypes(exColumnsTypes.begin(), exColumnsTypes.end());

	size_t size = metaData1.size();
	for (size_t i = 0; i < size; i++)
	{
		Column &column1 = *metaData1[i];
		Column &column2 = *metaData2[i];

		try {
			ASSERT(column1.name() == column2.name());

			if (excludedColumnsTypes.find(column1.name()) == excludedColumnsTypes.end())
			{
				ASSERT(column1.type() == column2.type());

				if (column1.type() == BITFIELD)
					if (! (column1.bitfieldDef() == column2.bitfieldDef()))
					{
						Log::error() << "Comparator::compare: bitfield definitions for column "
							<< i << " '" << column1.name() << "' differ." << std::endl;
						ASSERT(column1.bitfieldDef() == column2.bitfieldDef());
					}
			}
		
			if (checkMissingFlag_)	
			{
				if (column1.hasMissing() != column2.hasMissing())
				{
					Log::warning() << column1.name() << " : " 
						<< "column1.hasMissing()=" << column1.hasMissing() << ", " 
						<< "column2.hasMissing()=" << column2.hasMissing() << ", " 
						<< std::endl;
					ASSERT(column1.hasMissing() == column2.hasMissing());
				}

				if (column1.missingValue() != column2.missingValue())
				{
					Log::warning() << column1.name() << " : " 
						<< "column1.missingValue()=" << column1.missingValue() << ", " 
						<< "column2.missingValue()=" << column2.missingValue() << ", " 
						<< std::endl;
					ASSERT(column1.missingValue() == column2.missingValue());
				}
			}
			else
			{
				if (column1.hasMissing() != column2.hasMissing())
				{
					Log::warning() << column1.name() << " : " 
						<< "column1.hasMissing()=" << column1.hasMissing() << ", " 
						<< "column2.hasMissing()=" << column2.hasMissing() << ", " 
						<< std::endl;
				}
				if (column1.missingValue() != column2.missingValue())
				{
					Log::warning() << column1.name() << " : " 
						<< "column1.missingValue()=" << column1.missingValue() << ", " 
						<< "column2.missingValue()=" << column2.missingValue() << ", " 
						<< std::endl;
				}
			}

			if (column1.hasMissing() && column2.hasMissing())
				ASSERT(column1.missingValue() == column2.missingValue());
		} catch (...) {
			Log::info( Here() ) << "While comparing column " << i << ": "
				<< column1.name() << std::endl;
			throw;
		}
	}
}

} // namespace odb

