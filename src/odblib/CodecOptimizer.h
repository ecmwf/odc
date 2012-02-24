///
/// \file CodecOptimizer.h
///
/// @author Piotr Kuchta, Jan 2010

#ifndef CodecOptimizer_H
#define CodecOptimizer_H

class PathName;
class DataHandle;

namespace odb {

class HashTable;
class SQLIteratorSession;

namespace codec {

class Codec;

class CodecOptimizer 
{
public:
	template <typename DATASTREAM>
		static int setOptimalCodecs(MetaData& columns);
};

} // namespace codec
} // namespace odb 

#include "CodecOptimizer.cc"

#endif
