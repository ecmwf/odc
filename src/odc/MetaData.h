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

#include "odc/Column.h"
#include "eckit/sql/SQLTypedefs.h"

#ifdef SWIGPYTHON
#include "odc/IteratorProxy.h"
#endif

namespace eckit { class PathName; }

namespace odc {

typedef std::vector<Column*> MetaDataBase;

class MetaData : public MetaDataBase {
public:
	MetaData();
	MetaData(int);
	MetaData(int, Column *);
	MetaData(const MetaData&);
	MetaData* clone() const;

	static MetaData scanFile(const eckit::PathName&);

	unsigned long long rowsNumber() const { return rowsNumber_; }
	void rowsNumber(unsigned long long n) { rowsNumber_ = n; }

	unsigned long long dataSize() const { return dataSize_; }
	void dataSize(unsigned long long n) { dataSize_ = n; }

	MetaData& operator=(const MetaData&);
	MetaData& operator+=(const MetaData&);
	MetaData operator+(const MetaData&);

	/// Check if number of columns, column names and column types are equal. Values not checked.
	bool operator==(const MetaData&) const;

	bool equalsIncludingConstants(const MetaData&, const std::vector<std::string>& constColumns) const; 

    bool operator!=(const MetaData& other) const { return ! (*this == other); }

	void operator|=(const MetaData& other);

	template<typename DATASTREAM> void save(DATASTREAM &) const;
	template<typename DATASTREAM> void load(DATASTREAM &);

	void setSize(size_t);

	MetaData& addColumn(const std::string& name, const std::string& type);

	template<typename DATASTREAM>
	MetaData& addColumnPrivate(const std::string& name, const std::string& type);

    bool allColumnsInitialised() const;

    MetaData& addBitfield(const std::string& name, const eckit::sql::BitfieldDef&);
    template<typename DATASTREAM> MetaData& addBitfieldPrivate(const std::string& name, const eckit::sql::BitfieldDef&);

	bool hasColumn(const std::string&) const;
	Column* columnByName(const std::string&) const;
	size_t columnIndex(const std::string&) const;

    static odc::ColumnType convertType(const std::string&);
#ifdef SWIGPYTHON
	std::string __str__()
	{
        std::stringstream s;
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

	virtual void print(std::ostream& s) const;

	friend std::ostream& operator<<(std::ostream& s, const MetaData& p)
		{ p.print(s); return s; }

private:
	unsigned long long rowsNumber_;
	unsigned long long dataSize_;
};


template<typename DATASTREAM>
void MetaData::save(DATASTREAM &f) const
{
	int32_t count = size();
    f.writeInt32(count);
    for(size_t i = 0; i < size(); i++) {
        at(i)->save(f);
    }
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
MetaData& MetaData::addColumnPrivate(const std::string& name, const std::string& type)
{
	Column* c = new Column(*this);
	ASSERT(c);

	c->name(name);
	c->type<DATASTREAM>(odc::Column::type(type), false);

	push_back(c);
	return *this;
}

template<typename DATASTREAM> 
MetaData& MetaData::addBitfieldPrivate(const std::string& name, const eckit::sql::BitfieldDef& bd)
{
	Column* c = new Column(*this);
	ASSERT(c);
	c->name(name);
	c->type<DATASTREAM>(BITFIELD, false);
	c->bitfieldDef(bd);
	push_back(c);

	return *this;
}

} // namespace odc

#endif

