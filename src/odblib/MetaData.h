/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef MetaData_H
#define MetaData_H

#include "eclib/machine.h"
#include "odblib/Column.h"

#ifdef SWIGPYTHON
#include "odblib/IteratorProxy.h"
#endif

namespace eclib { class PathName; }

namespace odb {

typedef vector<Column*> MetaDataBase;

class MetaData : public MetaDataBase {
public:
	MetaData(int);
	MetaData(int, Column *);
	MetaData(const MetaData&);
	MetaData* clone() const;

	static MetaData scanFile(const eclib::PathName&);

	unsigned long long rowsNumber() { return rowsNumber_; }
	void rowsNumber(unsigned long long n) { rowsNumber_ = n; }

	unsigned long long dataSize() { return dataSize_; }
	void dataSize(unsigned long long n) { dataSize_ = n; }

	MetaData& operator=(const MetaData&);
	MetaData& operator+=(const MetaData&);
	MetaData operator+(const MetaData&);

	/// Check if number of columns, column names and column types are equal. Values not checked.
	bool operator==(const MetaData&) const;

	bool equalsIncludingConstants(const MetaData&, const vector<string>& constColumns) const; 

	bool operator!=(const MetaData& other) const { return ! (self == other); }

	void operator|=(const MetaData& other);

	template<typename DATASTREAM> void save(DATASTREAM &) const;
	template<typename DATASTREAM> void load(DATASTREAM &);

	void setSize(size_t);

	template<typename DATASTREAM>
	MetaData& addColumn(const string& name, const string& type, bool hasMissing = true, double missingValue = 0.0 /*FIXME*/);

	bool hasColumn(const string&) const;
	Column* columnByName(const string&) const;
	size_t columnIndex(const string&) const;

#ifdef SWIGPYTHON
	std::string __str__()
	{
		stringstream s;
		s << "[";
		for (size_t i = 0; i < size(); ++i)
		{
			s << at(i)->__repr__() << ",";
		}
		s << "]";
		return s.str();
	} 
#endif

	void resetStats();

	virtual ~MetaData();

	virtual void print(ostream& s) const;

	friend ostream& operator<<(ostream& s, const MetaData& p)
		{ p.print(s); return s; }

private:
	unsigned long long rowsNumber_;
	unsigned long long dataSize_;
	MetaData& self;
};


template<typename DATASTREAM>
void MetaData::save(DATASTREAM &f) const
{
	int32_t count = size();
	f.writeInt32(count);
	for(size_t i = 0; i < size(); i++)
		at(i)->save(f);
}

template<typename DATASTREAM>
void MetaData::load(DATASTREAM &f)
{
	for (size_t i = 0; i < size(); i++)
		delete at(i);
	clear();

	int32_t nCols;
	f.readInt32(nCols);
	resize(nCols, NULL);
	for (size_t i = 0; i < size(); i++)
	{
		delete at(i);
		at(i) = new Column(*this);
		at(i)->load(f);
	}
}

template <typename DATASTREAM>
MetaData& MetaData::addColumn(const string& name, const string& type, bool hasMissing, double missingValue)
{
	Column* c = new Column(*this);
	ASSERT(c);
	c->name(name);
	c->type<DATASTREAM>(odb::Column::type(type), false);
	c->hasMissing(hasMissing);
	//TODO:
	//c->missingValue(missingValue);
	
	push_back(c);
	return *this;
}

} // namespace odb

#endif

