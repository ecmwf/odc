/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file .h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

//#include <iostream>
//#include <vector>
//#include <map>
//#include <algorithm>

//#include <stdlib.h>
//#include <memory> 

using namespace std;

//#include "eckit/filesystem/PathName.h"
//#include "eckit/io/DataHandle.h"
//#include "eckit/log/Log.h"

//#include "odblib/DataStream.h"
//#include "odblib/HashTable.h"
//#include "odblib/Codec.h"
//#include "odblib/HashTable.h"
//#include "odblib/Column.h"
//#include "odblib/MetaData.h"
//#include "odblib/RowsIterator.h"
//#include "odblib/HashTable.h"
//#include "odblib/SQLBitfield.h"
//#include "odblib/SQLAST.h"
//#include "odblib/SchemaAnalyzer.h"
//#include "odblib/SQLIteratorSession.h"
//#include "odblib/Header.h"
//#include "odblib/Reader.h"
//#include "odblib/SelectIterator.h"
//#include "odblib/ReaderIterator.h"
//#include "odblib/Comparator.h"
//#include "odblib/Decoder.h"
//#include "odblib/SQLSelectFactory.h"
//#include "odblib/FastODA2Request.h"
//#include "odblib/MemoryBlock.h"
//#include "odblib/InMemoryDataHandle.h"
//#include "odblib/odb_api.h"
//#include "odblib/odbcapi.h"
//#include "odblib/ImportTool.h"
//#include "odblib/Tool.h"
//#include "odblib/TestCase.h"
//#include "odblib/DateTime.h"
//#include "odblib/SplitTool.h"
//#include "odblib/CountTool.h"

#ifndef MapReduce_H
#define MapReduce_H

namespace odb {
namespace tool {

typedef void (*CallBackProcessOneRowMapper)(void *, const double*, size_t);
typedef void* (*CallBackProcessOneRowReducer)(void *, void*);

struct Array {
	const double* data;
	size_t nCols;
	size_t nRows;
};

typedef void (*CallBackProcessArrayMapper)(void *, struct Array);
typedef void* (*CallBackProcessArrayReducer)(void *, void*);

typedef void* (*CreateResult)();
typedef void (*DestroyResult)(void *);

struct CallBackProcessOneRow {
	CallBackProcessOneRowMapper mapper;
	CallBackProcessOneRowReducer reducer;
	CreateResult create;
	DestroyResult destroy;
};

struct CallBackProcessArray {
	CallBackProcessArrayMapper mapper;
	CallBackProcessArrayReducer reducer;
	CreateResult create;
	DestroyResult destroy;
};

class SingleThreadMapReduce {
	static const size_t N;
public:
	static void * process(void* userData, eckit::DataHandle& dh, const std::string& sql, CallBackProcessOneRow);
	static void * process(void* userData, const eckit::PathName& fileName, const std::string& sql, CallBackProcessOneRow);

	static void * process(void* userData, const eckit::PathName& fileName, const std::string& sql, CallBackProcessArray);
	static void * process(void* userData, eckit::DataHandle& dh, const std::string& sql, CallBackProcessArray);
};

class MultipleThreadMapReduce {
	static const size_t threadPoolSize_;
public:
	static void * process(void* userData, eckit::DataHandle& dh, const std::string& sql, CallBackProcessOneRow);
	static void * process(void* userData, const eckit::PathName& fileName, const std::string& sql, CallBackProcessOneRow);

	static void * process(void* userData, const eckit::PathName& fileName, const std::string& sql, CallBackProcessArray);
	static void * process(void* userData, eckit::DataHandle& dh, const std::string& sql, CallBackProcessArray);
};

} // namespace tool 
} // namespace odb 

#endif

