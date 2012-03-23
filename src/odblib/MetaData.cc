/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eclib/DataHandle.h"
#include "eclib/Exceptions.h"

#include "odblib/oda.h"
#include "odblib/Codec.h"
#include "odblib/CodecOptimizer.h"
#include "odblib/Column.h"
#include "odblib/DataStream.h"
#include "odblib/HashTable.h"
#include "odblib/MetaData.h"
#include "odblib/MetaDataReader.h"
#include "odblib/MetaDataReaderIterator.h"

namespace odb {

MetaData::MetaData(int i) : vector<Column*>(i), rowsNumber_(0), self(*this) {}
MetaData::MetaData(int i, Column *p) : vector<Column*>(i, p), rowsNumber_(0), self(*this) {}
MetaData::MetaData(const MetaData& md) : vector<Column*>(0), rowsNumber_(0), self(*this)
{ self += md; }

MetaData MetaData::scanFile(const PathName& fileName)
{
	Log::info() << "Iterating over headers of '" << fileName << "'" <<  endl;

	MetaDataReader mdReader(fileName);
	MetaDataReader::iterator it = mdReader.begin();
	MetaDataReader::iterator end = mdReader.end();

	MetaData wholeFileMD(it->columns());

	unsigned long int i = 0;	
	unsigned long int mds = 0;	
	for ( ; it != end; ++it)
	{
		++i;
		if (it->isNewDataset())
		{
			++mds;
			Log::info() << it->columns() << endl;
			wholeFileMD |= it->columns();
		}
	}
	Log::info() << "TestMetaDataReader::test i=" << i << ", mds=" << mds << endl;

	codec::CodecOptimizer::setOptimalCodecs<DataStream<> >(wholeFileMD);

	return wholeFileMD;
}

void MetaData::setSize(size_t n)
{
	size_t oldSize = size();

	for (size_t i = n; i < oldSize; ++i)
		delete at(i);

	vector<Column*>::resize(n, 0);

	for (size_t i = oldSize; i < n; ++i)
		at(i) = new Column(*this);
}

MetaData::~MetaData()
{
	for (size_type i = 0; i < size(); i++)
		delete at(i);
}

//const
Column* MetaData::columnByName(const string& name) const
{ return at(columnIndex(name)); }

bool MetaData::hasColumn(const string& name) const
{ 
	for (size_t i = 0; i < size(); i++)
		if (at(i)->name() == name || at(i)->name().find(name + "@") == 0)
			return true;
	return false;
}

size_t MetaData::columnIndex(const string& name) const
{
	vector<size_t> indices;

	for (size_t i = 0; i < size(); i++)
		if (at(i)->name() == name || at(i)->name().find(name + "@") == 0)
			indices.push_back(i);

	if (indices.size() > 1)
		throw UserError(string("Ambiguous column name: '") + name + "'");

	if (indices.size() == 0)
		throw UserError(string("Column '") + name + "' not found.");

	return indices[0];
}

MetaData& MetaData::operator=(const MetaData& other)
{
	if(this == &other)
		return self;

	if (self.size() != other.size())
	{
		for (size_type i=0; i < self.size(); i++)
			delete self[i];
		self.clear();

		typedef Column* PColumn;
		self.resize(other.size(), PColumn(0));

		for (size_type i=0; i < self.size(); i++)
			self[i] = new Column(self);
	}
	
	for (size_type i=0; i < self.size(); i++)
		*self[i] = *other[i];

	return self;
}

MetaData& MetaData::operator+=(const MetaData& rhs)
{
	for (size_t i = 0; i < rhs.size(); ++i)
	{
		Column& rhsColumn = *rhs[i];
		Column* c = new Column(*this);
		ASSERT(c);
		c->name(rhsColumn.name());
		// DataStream not important for stats gathering
		c->type<DataStream<> >(rhsColumn.type(), false);
		push_back(c);
	}
	return *this;
}

void MetaData::operator|=(const MetaData& other)
{
	ASSERT(size() == other.size());
	for (size_t i = 0; i < size(); ++i)
	{
		ASSERT(self[i]->name() == other[i]->name());
		ASSERT(self[i]->type() == other[i]->type());

		self[i]->coder().gatherStats(other[i]->max());
		self[i]->coder().gatherStats(other[i]->min());
	}
}

MetaData MetaData::operator+(const MetaData& rhs)
{
	MetaData r = *this;
	r += rhs;
	return r;
}

bool MetaData::equalsIncludingConstants(const MetaData& other, const vector<string>& constColumns) const 
{
	//if (! MetaData::operator==(other)) return false;
	for (size_t i = 0; i < constColumns.size(); ++i)
	{
		const string& columnName = constColumns[i];

		if ( !self.hasColumn(columnName) && !other.hasColumn(columnName))
			continue;

		Column& c1 = *(self.columnByName(columnName));
		Column& c2 = *(other.columnByName(columnName));

		if ( ! c1.isConstant() || ! c2.isConstant())
			return false;
		else
		{
			odb::codec::Codec& codec1 = c1.coder();
			odb::codec::Codec& codec2 = c2.coder();
			if ( (codec1.min() != codec2.min())
				|| (codec1.max() != codec2.max()))
				return false;
		}
	}
		
	return true;
}

bool MetaData::operator==(const MetaData& other) const
{
	if (self.size() != other.size())
		return false;

	for (size_t i = 0; i < self.size(); ++i)
		if (*self[i] != *other[i])
			return false;

	return true;
}

void MetaData::resetStats()
{
	//Log::debug() << "MetaData::resetStats" << endl;
	for (size_t i = 0; i < size(); i++)
		(*this)[i]->resetStats();
}

void MetaData::print(ostream& s) const
{
	for (size_t i = 0; i < size(); i++)
		s << i << ". " << *at(i) << endl;
}

} // namespace odb

