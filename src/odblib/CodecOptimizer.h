/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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
