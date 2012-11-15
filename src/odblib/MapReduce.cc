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

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

#include <stdlib.h>
#include <memory> 

using namespace std;

#include "eclib/PathName.h"
#include "eclib/DataHandle.h"
#include "eclib/Log.h"
#include "eclib/DataHandle.h"
#include "eclib/PartFileHandle.h"
#include "eclib/ThreadPool.h"

#include "odblib/DataStream.h"
#include "odblib/HashTable.h"
#include "odblib/Codec.h"
#include "odblib/HashTable.h"
#include "odblib/Column.h"
#include "odblib/MetaData.h"
#include "odblib/RowsIterator.h"
#include "odblib/HashTable.h"
#include "odblib/SQLBitfield.h"
#include "odblib/SQLAST.h"
#include "odblib/SchemaAnalyzer.h"
#include "odblib/SQLIteratorSession.h"
#include "odblib/Header.h"
#include "odblib/Reader.h"
#include "odblib/SelectIterator.h"
#include "odblib/ReaderIterator.h"
#include "odblib/Comparator.h"
#include "odblib/Decoder.h"
#include "odblib/SQLSelectFactory.h"
#include "odblib/FastODA2Request.h"
#include "odblib/MemoryBlock.h"
#include "odblib/InMemoryDataHandle.h"
#include "odblib/oda.h"
#include "odblib/odbcapi.h"
#include "odblib/ImportTool.h"
#include "odblib/Tool.h"
#include "odblib/TestCase.h"
#include "odblib/DateTime.h"
#include "odblib/SplitTool.h"
#include "odblib/CountTool.h"
#include "odblib/MapReduce.h"

namespace odb {
namespace tool {

class PartFileProcessor : public ThreadPoolTask {
public:
	PartFileProcessor(const PathName& fileName, const Offset offset, const Length length,
						void* userData, const string& sql, CallBackProcessOneRow f) 
	: dh_(new PartFileHandle(fileName, offset, length)),
	  userData_(userData),
	  sql_(sql),
	  f_(f), 
	  fileName_(fileName),
	  offset_(offset),
	  length_(length)
	{}

	void execute()
	{
		dh_->openForRead();
		SingleThreadMapReduce::forEachRow(userData_, *dh_, sql_, f_);
	}
private:
	auto_ptr<DataHandle> dh_;
	void* userData_;
	const string sql_;
	CallBackProcessOneRow f_;
	
	PathName fileName_;
	Offset offset_;
	Length length_;
};

void * SingleThreadMapReduce::forEachRow(void* userData, DataHandle& dh, const string& sql, CallBackProcessOneRow f)
{
	if (userData == 0) userData = f.create(); 

	odb::Select o(sql, dh);
	odb::Select::iterator it(o.begin()), end(o.end());
	for (; it != end; ++it)
		f.mapper(userData, it->data(), it->columns().size());

	return userData;
}

void * SingleThreadMapReduce::forEachRow(void* userData, const PathName& fileName, const string& sql, CallBackProcessOneRow f)
{
	FileHandle dh(fileName);
	dh.openForRead();
	return forEachRow(userData, dh, sql, f);
}

void * MultipleThreadMapReduce::forEachRow(void* userData, DataHandle& dh, const string& sql, CallBackProcessOneRow f)
{
	NOTIMP; // TODO:
	return 0;
}

void * MultipleThreadMapReduce::forEachRow(void* userData, const PathName& fileName, const string& sql, CallBackProcessOneRow f)
{
	const size_t threadPoolSize = 5;
	ThreadPool pool(string("[") + fileName + " processors]", threadPoolSize);

	vector<void *> results;
	vector<pair<Offset,Length> > chunks(SplitTool::getChunks(fileName));
    for(size_t i=0; i < chunks.size(); ++i)
    {   
		pair<Offset,Length>& chunk(chunks[i]);
		Log::info() << "MultipleThreadMapReduce::forEachRow: process chunk " << chunk.first << "," << chunk.second << endl;

		void *result = f.create();
		pool.push(new PartFileProcessor(fileName, chunk.first, chunk.second, result, sql, f));
		results.push_back(result);
    } 
	pool.waitForThreads();

	//if (userData == 0) userData = f.create(); 
	
	void *r = f.create();
	for (size_t i = 0; i < results.size(); ++i)
	{
		void *oldResult = r;
		r = f.reducer(results[i], oldResult);
		f.destroy(oldResult);
	}

	return r;
}

} // namespace tool 
} // namespace odb 

