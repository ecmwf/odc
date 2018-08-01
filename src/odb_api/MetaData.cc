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
#include "odb_api/CodecOptimizer.h"
#include "odb_api/MetaData.h"
#include "odb_api/MetaDataReader.h"
#include "odb_api/MetaDataReaderIterator.h"

using namespace eckit;

namespace odb {

MetaData::MetaData() : std::vector<Column*>(), rowsNumber_(0), self(*this) {}
MetaData::MetaData(int i) : std::vector<Column*>(i), rowsNumber_(0), self(*this) {}
MetaData::MetaData(int i, Column *p) : std::vector<Column*>(i, p), rowsNumber_(0), self(*this) {}
MetaData::MetaData(const MetaData& md) : std::vector<Column*>(0), rowsNumber_(0), self(*this)
{ self += md; }

odb::ColumnType MetaData::convertType(const std::string& t)
{
    std::string type(t);
    transform(type.begin(), type.end(), type.begin(), ::toupper);

    if      (type == "INTEGER")  return odb::INTEGER;
    else if (type == "YYYYMMDD") return odb::INTEGER;
    else if (type == "HHMMSS")   return odb::INTEGER;
    else if (type == "PK1INT")   return odb::INTEGER;
    else if (type == "PK9INT")   return odb::INTEGER;
    else if (type == "@LINK")    return odb::INTEGER;
    else if (type == "REAL")     return odb::REAL;
    else if (type == "FLOAT")    return odb::REAL;
    else if (type == "DOUBLE")   return odb::DOUBLE;
    else if (type == "PK9REAL")  return odb::DOUBLE;
    else if (type == "STRING")   return odb::STRING;
    else if (type.find("BITFIELD") != string::npos) return odb::BITFIELD;
    else throw eckit::UserError("Unsupported column type: " + type);

    return odb::IGNORE; // never reached
}

MetaData* MetaData::clone() const {
	const MetaData& self(*this);
	MetaData* md = new MetaData(*this);
	for (size_t i = 0; i < size(); ++i)
		(*md)[i]->coder(self[i]->coder().clone());
	return md;
}

MetaData MetaData::scanFile(const PathName& fileName)
{
    std::ostream& L(Log::debug());
	L << "Iterating over headers of '" << fileName << "'" <<  std::endl;

	typedef MetaDataReader<MetaDataReaderIterator> MDR;

	MDR mdReader(fileName);
	MDR::iterator it (mdReader.begin());
	MDR::iterator end (mdReader.end());

	MetaData wholeFileMD(it->columns());

	unsigned long int i = 0;	
	unsigned long int mds = 0;	
	for ( ; it != end; ++it)
	{
		++i;
		if (it->isNewDataset())
		{
			++mds;
			L << it->columns() << std::endl;
			wholeFileMD |= it->columns();
		}
	}
	L << "TestMetaDataReader::test i=" << i << ", mds=" << mds << std::endl;

	codec::CodecOptimizer().setOptimalCodecs<DataStream<> >(wholeFileMD);

	return wholeFileMD;
}

void MetaData::setSize(size_t n)
{
	size_t oldSize = size();

	for (size_t i = n; i < oldSize; ++i)
		delete at(i);

	std::vector<Column*>::resize(n, 0);

	for (size_t i = oldSize; i < n; ++i)
		at(i) = new Column(*this);
}

MetaData::~MetaData()
{
	for (size_type i = 0; i < size(); i++)
		delete at(i);
}

//const
Column* MetaData::columnByName(const std::string& name) const
{ return at(columnIndex(name)); }

bool MetaData::hasColumn(const std::string& name) const
{ 
	for (size_t i = 0; i < size(); i++)
		if (at(i)->name() == name || at(i)->name().find(name + "@") == 0)
			return true;
	return false;
}

size_t MetaData::columnIndex(const std::string& name) const
{
	std::vector<size_t> indices;

	for (size_t i = 0; i < size(); i++)
		if (at(i)->name() == name || at(i)->name().find(name + "@") == 0)
			indices.push_back(i);

	if (indices.size() > 1)
		throw eckit::UserError(std::string("Ambiguous column name: '") + name + "'");

	if (indices.size() == 0)
		throw eckit::UserError(std::string("Column '") + name + "' not found.");

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

		//Log::debug() << "MetaData::operator+=: adding " << rhsColumn << std::endl;

		Column* c = new Column(rhsColumn);
		ASSERT(c);
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

bool MetaData::equalsIncludingConstants(const MetaData& other, const std::vector<std::string>& constColumns) const 
{
    std::ostream& L = Log::debug();
	for (size_t i = 0; i < constColumns.size(); ++i)
	{
		const std::string& columnName = constColumns[i];
		L << "MetaData::equalsIncludingConstants: check " << columnName << std::endl;

		if ( !self.hasColumn(columnName) && !other.hasColumn(columnName))
			continue;

		Column& c1 = *(self.columnByName(columnName));
		Column& c2 = *(other.columnByName(columnName));

		if ( ! c1.isConstant() || ! c2.isConstant())
		{
			L << "MetaData::equalsIncludingConstants: c1 " << c1 << " " << c1.coder() << std::endl;
			L << "MetaData::equalsIncludingConstants: c2 " << c2 << " " << c2.coder() << std::endl;
			L << "MetaData::equalsIncludingConstants: column '" << columnName << "'" << std::endl;
			return false;
		}
		else
		{
			odb::codec::Codec& codec1 = c1.coder();
			odb::codec::Codec& codec2 = c2.coder();
			if ( codec1.min() != codec2.min() )
			{
				L << "MetaData::equalsIncludingConstants: column '" << columnName << "'" << std::endl;
				L << "MetaData::equalsIncludingConstants: min1=" << codec1.min() << ", max1=" << codec1.max() << std::endl;
				L << "MetaData::equalsIncludingConstants: min2=" << codec2.min() << ", max2=" << codec2.max() << std::endl;
				L << "MetaData::equalsIncludingConstants: c1.coder: " << codec1 << std::endl;
				L << "MetaData::equalsIncludingConstants: c2.coder: " << codec2 << std::endl;
				return false;
			}
		}
	}

	L << "MetaData::equalsIncludingConstants: yes" << std::endl;
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
	//Log::debug() << "MetaData::resetStats" << std::endl;
	for (size_t i = 0; i < size(); i++)
		(*this)[i]->resetStats();
}

void MetaData::print(std::ostream& s) const
{
	for (size_t i = 0; i < size(); i++)
		s << i << ". " << *at(i) << std::endl;
}

MetaData& MetaData::addColumn(const std::string& name, const std::string& type)
{
    return addColumnPrivate<odb::DataStream<odb::SameByteOrder, eckit::DataHandle> >(name, type);
}

bool MetaData::allColumnsInitialised() const {

    if (empty())
        return false;

    for (size_t i = 0; i < size(); i++) {
        if (!(*this)[i]->hasInitialisedCoder())
            return false;
    }

    return true;
}

MetaData& MetaData::addBitfield(const std::string& name, const eckit::sql::BitfieldDef& bf)
{
    return addBitfieldPrivate<odb::DataStream<odb::SameByteOrder, eckit::DataHandle> >(name, bf);
}

} // namespace odb

