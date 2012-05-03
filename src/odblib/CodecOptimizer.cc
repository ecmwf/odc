/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

///
/// \file CodecOptimizer.cc
///
/// @author Piotr Kuchta, Jan 2010


#include "odblib/oda.h"
#include "odblib/DataStream.h"
#include "odblib/Codec.h"
#include "odblib/CodecOptimizer.h"

//

namespace odb {
namespace codec {

template <typename DATASTREAM>
int CodecOptimizer::setOptimalCodecs(MetaData& columns)
{
	//ostream &LOG = Log::debug();

	for (size_t i = 0; i < columns.size(); i++) {
		Column& col = *columns[i];
		string codec = "long_real";
		long long n;
		double min = col.min();
		double max = col.max();
		bool hasMissing = col.hasMissing();
		double missing = col.missingValue();
		//LOG << "CodecOptimizer::setOptimalCodecs: " << i << " " << col.name() << ", min=" << min << ", max=" << max << endl;
		switch(col.type())
		{
			case REAL:
				if(max == min)
					codec = col.hasMissing() ? "real_constant_or_missing" : "constant";
				else
					codec = "short_real";
				col.coder(Codec::findCodec<DATASTREAM>(codec, false));
				col.hasMissing(hasMissing);
				col.missingValue(missing);
				col.min(min);
				col.max(max);
				//LOG << " REAL has values in range <" << col.min() << ", " << col.max()
				//	<< ">. Codec: "  << col.coder()
				//	<< endl;
				break;

			case DOUBLE:
				if(max == min)
					codec = col.hasMissing() ? "real_constant_or_missing" : "constant";
				else
					codec = "long_real";
				col.coder(Codec::findCodec<DATASTREAM>(codec, false));
				col.hasMissing(hasMissing);
				col.missingValue(missing);
				col.min(min);
				col.max(max);
				//LOG << " DOUBLE has values in range <" << col.min() << ", " << col.max()
				//	<< ">. Codec: "  << col.coder()
				//	<< endl;
				break;

			case STRING:
				{
					n = col.coder().name() == "constant_string" ? 1 : col.coder().hashTable().nextIndex();
					if(n == 1)
						codec = "constant_string";
					else if(n < 256)
						codec = "int8_string";
					else if(n < 65536)
						codec = "int16_string";
					else 
						codec = "chars";

					Codec * newCodec = Codec::findCodec<DATASTREAM>(codec, false);
					if (n > 1)
						newCodec->hashTable(col.coder().giveHashTable());
					col.coder(newCodec);
					col.hasMissing(hasMissing);
					col.missingValue(missing);
					col.min(min);
					col.max(max);
					//LOG << " STRING has " << n << " different value(s). Codec: " << codec
					//	<< endl;
				}
				break;

			case INTEGER:
			case BITFIELD:
				n = max - min;

				//LOG << " { min=" << min << ", max=" << max << ", n=" << n << "} ";

				if(col.hasMissing())
				{
					if(n == 0) codec = "constant_or_missing";
					else if(n < 0xff) codec = "int8_missing";
					else if(n < 0xffff) codec = "int16_missing";
					else codec = "int32";
				}
				else
				{
					if(n == 0) codec = "constant";
					else if(n <= 0xff) codec = "int8";
					else if(n <= 0xffff) codec = "int16";
					else codec = "int32";
				}
				col.coder(Codec::findCodec<DATASTREAM>(codec, false));
				col.hasMissing(hasMissing);
				col.missingValue(missing);
				col.min(min);
				col.max(max);
				//LOG << (col.type() == BITFIELD ? " BITFIELD" : " INTEGER")
				//	<< " has " << n + 1 << " different value(s)"
				//	<< (col.hasMissing() ? " and has missing value. " : ". ")
				//	<< "Codec: " << col.coder() << "."
				//	<< endl;
				break;

			default: Log::error() << "Unsupported type: [" << col.type() << "]" << endl;
				break;
		}
	}
	return 0;
}

} // namespace codec 
} // namespace odb 
