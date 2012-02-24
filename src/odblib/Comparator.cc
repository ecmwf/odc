#include "oda.h"
#include "Comparator.h"
#include <cmath>
#include "Resource.h"
#include "Tracer.h"

#define SRC __FILE__, __LINE__


namespace odb {

Comparator::Comparator(bool checkMissingFlag)
: nRow_(0),
  checkMissingFlag_(checkMissingFlag),
  NaN_isOK_(Resource<bool>("$ODB_API_NAN_IS_OK", false))
{}

void Comparator::compare(const PathName& p1, const PathName& p2)
{
	Tracer t(Log::debug(), string() + "Comparator::compare: " + p1 + ", " + p2);

	odb::Reader oda1(p1);
	odb::Reader oda2(p2);

	odb::Reader::iterator it1(oda1.begin());
	odb::Reader::iterator end1(oda1.end());
	odb::Reader::iterator it2(oda2.begin());
	odb::Reader::iterator end2(oda2.end());
	
	compare(it1, end1, it2, end2, p1, p2);
}

void Comparator::compare(int nCols, const double *data1, const double *data2, const MetaData& md)
{
	for (int i=0; i < nCols; i++)
		try
		{
			switch (md[i]->type())
			{
				case INTEGER:
				case BITFIELD:
				case STRING:
				case DOUBLE:
					ASSERT(same(data1[i], data2[i]) || (NaN_isOK_ && (isnan(data1[i]) && isnan(data2[i]))));
					break;
				case REAL:
					ASSERT(same(float(data1[i]), float(data2[i])) || (NaN_isOK_ && (isnan(data1[i]) && isnan(data2[i]))));
					break;
				case IGNORE:
				default:
					ASSERT(!"Unknown type");
					break;
			}
		} catch (...) {
			Log::info(SRC) << "While comparing rows number " << nRow_ << ", columns " << i
				<< " found different." << endl;
			Log::info(SRC) << " data1[" << i << "] = " << fixed << data1[i] << endl;
			Log::info(SRC) << " data2[" << i << "] = " << fixed << data2[i] << endl;

			Log::info(SRC) << " md[" << i << "] is " << *md[i] << endl;
			
			throw;
		}
}


void Comparator::compare(const MetaData& metaData1, const MetaData& metaData2)
{
	ASSERT("Number of columns must be the same" && (metaData1.size() == metaData2.size()));

	size_t size = metaData1.size();
	for (size_t i = 0; i < size; i++)
	{
		Column &column1 = *metaData1[i];
		Column &column2 = *metaData2[i];

		try {
			ASSERT(column1.name() == column2.name());
			ASSERT(column1.type() == column2.type());

			if (column1.type() == BITFIELD)
			{
				if (! (column1.bitfieldDef() == column2.bitfieldDef()))
				{
					Log::error() << "Comparator::compare: bitfield definitions for column "
						<< i << " '" << column1.name() << "' differ." << endl;
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
						<< endl;
					ASSERT(column1.hasMissing() == column2.hasMissing());
				}

				if (column1.missingValue() != column2.missingValue())
				{
					Log::warning() << column1.name() << " : " 
						<< "column1.missingValue()=" << column1.missingValue() << ", " 
						<< "column2.missingValue()=" << column2.missingValue() << ", " 
						<< endl;
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
						<< endl;
				}
				if (column1.missingValue() != column2.missingValue())
				{
					Log::warning() << column1.name() << " : " 
						<< "column1.missingValue()=" << column1.missingValue() << ", " 
						<< "column2.missingValue()=" << column2.missingValue() << ", " 
						<< endl;
				}
			}

			if (column1.hasMissing() && column2.hasMissing())
				ASSERT(column1.missingValue() == column2.missingValue());
		} catch (...) {
			Log::info(SRC) << "While comparing column " << i << ": "
				<< column1.name() << endl;
			throw;
		}
	}
}

} // namespace odb

