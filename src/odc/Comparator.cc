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
#include "odc/Comparator.h"
#include "odc/Reader.h"
#include "odc/StringTool.h"
#include "odc/Tracer.h"

#include <string.h>

#if __cplusplus >= 199711L
#define isnan(x) std::isnan(x)
#endif

using namespace std;
using namespace eckit;
using namespace odc::api;

class ValuesDifferent : public Exception {
public:
	ValuesDifferent(const std::string& what) : Exception(what) {}
};


namespace odc {

Comparator::Comparator(bool checkMissingFlag)
: nRow_(0),
  checkMissingFlag_(checkMissingFlag),
  NaN_isOK_(Resource<bool>("$odc_NAN_IS_OK", false))
{}


void Comparator::compare(const PathName& p1, const PathName& p2)
{
	std::vector<std::string> noExcludedColumnTypes;
    std::vector<std::string> noExcludedColumns;
    compare(p1, p2, noExcludedColumnTypes, noExcludedColumns);
}

void Comparator::compare(eckit::DataHandle& l, eckit::DataHandle& r)
{
	std::vector<std::string> noExcludedColumnTypes;
    std::vector<std::string> noExcludedColumns;
    odc::Reader oda1(l);
	odc::Reader oda2(r);

	odc::Reader::iterator it1(oda1.begin());
	odc::Reader::iterator end1(oda1.end());
	odc::Reader::iterator it2(oda2.begin());
	odc::Reader::iterator end2(oda2.end());
	
    compare(it1, end1, it2, end2, "left", "right", noExcludedColumnTypes, noExcludedColumns);
}

void Comparator::compare(const PathName& p1, const PathName& p2,
                         const std::vector<std::string>& excludedColumnsTypes,
                         const std::vector<std::string>& excludedColumns)
{
    Tracer t(Log::debug(), std::string("Comparator::compare: ") + p1 + ", " + p2);

	odc::Reader oda1(p1);
	odc::Reader oda2(p2);

	odc::Reader::iterator it1(oda1.begin());
	odc::Reader::iterator end1(oda1.end());
	odc::Reader::iterator it2(oda2.begin());
	odc::Reader::iterator end2(oda2.end());
	
    compare(it1, end1, it2, end2, p1, p2, excludedColumnsTypes, excludedColumns);
}

void Comparator::raiseNotEqual(const Column& column, double d1, double d2) {
    ColumnType type(column.type());
    stringstream ss;
    ss << "Values different in column " << column.name() << ": " 
        << StringTool::valueAsString(d1, type) << " is not equal " << StringTool::valueAsString(d2, type) << endl;
    throw ValuesDifferent(ss.str());
}

void Comparator::compare(int nCols,
                         const double *data1,
                         const double *data2,
                         const MetaData& md1,
                         const MetaData& md2) {

    std::vector<int> skipColsUnused;
    compare(nCols, data1, data2, md1, md2, skipColsUnused);
}

void Comparator::compare(int nCols,
                         const double *data1,
                         const double *data2,
                         const MetaData& md1,
                         const MetaData& md2,
                         const std::vector<int>& skipCols) {

    std::vector<int>::const_iterator nextSkipCol = skipCols.begin();

    unsigned long long numberOfDifferences (0);
    for (int i=0; i < nCols; i++) {

        // Skip the specified columns
        if (nextSkipCol != skipCols.end() && (*nextSkipCol) == i) {
            ++nextSkipCol;
            continue;
        }

        try {
            const Column& column(*md1[i]);
            ColumnType type(column.type());
            double d1 (data1[i]), d2 (data2[i]);

            switch (type)
            {
                case STRING:
                    if (strncmp(reinterpret_cast<const char*>(&d1), reinterpret_cast<const char*>(&d2), sizeof(double)))
                        raiseNotEqual(column, d1, d2);
                    break;
                case INTEGER:
                case BITFIELD:
                case DOUBLE:
                    if (! (same(d1, d2) || (NaN_isOK_ && (::isnan(d1) && ::isnan(d2)))))
                        raiseNotEqual(column, d1, d2);
                    break;
                case REAL:
                    if (! (same(float(d1), float(d2)) || (NaN_isOK_ && (::isnan(d1) && ::isnan(d2)))))
                        raiseNotEqual(column, d1, d2);
                    break;
                case IGNORE:
                default:
                    ASSERT(!"Unknown type");
                    break;
            }
        } catch (Exception &e) {
            ++numberOfDifferences; 
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

    if (numberOfDifferences)
    {
        stringstream ss;
        ss << "Files differ. "; // << numberOfDifferences << " difference" << ((numberOfDifferences == 1) ? "" : "s") << " found.";
        throw Exception(ss.str());
    }
}


void Comparator::compare(const MetaData& metaData1, const MetaData& metaData2,
                         const std::set<std::string>& excludedColumnsTypes,
                         const std::set<std::string>& excludedColumns,
                         std::vector<int>& skipCols) {

	ASSERT("Number of columns must be the same" && (metaData1.size() == metaData2.size()));

    // We keep track of which columns are skipped in this routine.
    skipCols.clear();

	size_t size = metaData1.size();
	for (size_t i = 0; i < size; i++)
	{
		Column &column1 = *metaData1[i];
		Column &column2 = *metaData2[i];

		try {
			ASSERT(column1.name() == column2.name());

            // If we are skipping a column, then we should check nothing for it.
            if (excludedColumns.find(column1.name()) != excludedColumns.end()) {
                skipCols.push_back(i);
                continue;
            }

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
            Log::info() << "While comparing column " << i << ": "
				<< column1.name() << std::endl;
			throw;
		}
	}
}

} // namespace odc

