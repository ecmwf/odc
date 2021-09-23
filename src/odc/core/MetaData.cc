/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <algorithm>

#include "eckit/log/Log.h"

#include "odc/core/MetaData.h"
#include "odc/LibOdc.h"

using namespace eckit;
using namespace odc::api;

namespace odc {
namespace core {

//----------------------------------------------------------------------------------------------------------------------

MetaData::MetaData() : std::vector<Column*>(), rowsNumber_(0) {}
MetaData::MetaData(int i) : std::vector<Column*>(i), rowsNumber_(0) {}
MetaData::MetaData(int i, Column *p) : std::vector<Column*>(i, p), rowsNumber_(0) {}

// SDS: n.b. this is changed from before. We
MetaData::MetaData(const MetaData& md) :
    std::vector<Column*>(0),
    rowsNumber_(md.rowsNumber_),
    dataSize_(md.dataSize_) {
    *this += md;
}

ColumnType MetaData::convertType(const std::string& t)
{
    std::string type(t);
    std::transform(type.begin(), type.end(), type.begin(), ::toupper);

    if      (type == "INTEGER")  return INTEGER;
    else if (type == "YYYYMMDD") return INTEGER;
    else if (type == "HHMMSS")   return INTEGER;
    else if (type == "PK1INT")   return INTEGER;
    else if (type == "PK9INT")   return INTEGER;
    else if (type == "@LINK")    return INTEGER;
    else if (type == "REAL")     return REAL;
    else if (type == "FLOAT")    return REAL;
    else if (type == "DOUBLE")   return DOUBLE;
    else if (type == "PK9REAL")  return DOUBLE;
    else if (type == "STRING")   return STRING;
    else if (type.find("BITFIELD") != std::string::npos) return BITFIELD;
    else throw eckit::UserError("Unsupported column type: " + type);

    return IGNORE; // never reached
}

MetaData* MetaData::clone() const {
	MetaData* md = new MetaData(*this);
	for (size_t i = 0; i < size(); ++i)
        (*md)[i]->coder((*this)[i]->coder().clone());
	return md;
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
        return *this;

    if (size() != other.size())
	{
        for (size_type i=0; i < size(); i++)
            delete (*this)[i];
        clear();

		typedef Column* PColumn;
        resize(other.size(), PColumn(0));

        for (size_type i=0; i < size(); i++)
            (*this)[i] = new Column(*this);
	}
	
    for (size_type i=0; i < size(); i++)
        *(*this)[i] = *other[i];

    return *this;
}

MetaData& MetaData::operator+=(const MetaData& rhs)
{
	for (size_t i = 0; i < rhs.size(); ++i)
	{
		Column& rhsColumn = *rhs[i];

		//LOG_DEBUG_LIB(LibOdc) << "MetaData::operator+=: adding " << rhsColumn << std::endl;

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
        ASSERT((*this)[i]->name() == other[i]->name());
        ASSERT((*this)[i]->type() == other[i]->type());

        (*this)[i]->coder().gatherStats(other[i]->max());
        (*this)[i]->coder().gatherStats(other[i]->min());
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
	for (size_t i = 0; i < constColumns.size(); ++i)
	{
		const std::string& columnName = constColumns[i];
		LOG_DEBUG_LIB(LibOdc) << "MetaData::equalsIncludingConstants: check " << columnName << std::endl;

        if ( !hasColumn(columnName) && !other.hasColumn(columnName))
			continue;

        Column& c1 = *(columnByName(columnName));
		Column& c2 = *(other.columnByName(columnName));

		if ( ! c1.isConstant() || ! c2.isConstant())
		{
			LOG_DEBUG_LIB(LibOdc) << "MetaData::equalsIncludingConstants: c1 " << c1 << " " << c1.coder() << std::endl;
			LOG_DEBUG_LIB(LibOdc) << "MetaData::equalsIncludingConstants: c2 " << c2 << " " << c2.coder() << std::endl;
			LOG_DEBUG_LIB(LibOdc) << "MetaData::equalsIncludingConstants: column '" << columnName << "'" << std::endl;
			return false;
		}
		else
		{
            Codec& codec1 = c1.coder();
            Codec& codec2 = c2.coder();
			if ( codec1.min() != codec2.min() )
			{
				LOG_DEBUG_LIB(LibOdc) << "MetaData::equalsIncludingConstants: column '" << columnName << "'" << std::endl;
				LOG_DEBUG_LIB(LibOdc) << "MetaData::equalsIncludingConstants: min1=" << codec1.min() << ", max1=" << codec1.max() << std::endl;
				LOG_DEBUG_LIB(LibOdc) << "MetaData::equalsIncludingConstants: min2=" << codec2.min() << ", max2=" << codec2.max() << std::endl;
				LOG_DEBUG_LIB(LibOdc) << "MetaData::equalsIncludingConstants: c1.coder: " << codec1 << std::endl;
				LOG_DEBUG_LIB(LibOdc) << "MetaData::equalsIncludingConstants: c2.coder: " << codec2 << std::endl;
				return false;
			}
		}
	}

	LOG_DEBUG_LIB(LibOdc) << "MetaData::equalsIncludingConstants: yes" << std::endl;
	return true;
}

bool MetaData::operator==(const MetaData& other) const {
    return equals(other);
}

bool MetaData::equals(const MetaData& other, bool compareDataSizes) const
{
    if (size() != other.size()) {
        return false;
    }

    for (size_t i = 0; i < size(); ++i) {
        if (!at(i)->equals(*other[i], compareDataSizes)) {
            return false;
        }
    }

	return true;
}

bool MetaData::compatible(const MetaData& other) const {

    // Normal case will be equality --> try O[N] check first.

    if (size() != other.size()) return false;

    bool equal = true;
    for (size_t i = 0; i < size(); ++i) {
        if (*(*this)[i] != *other[i]) {
            equal = false;
            break;
        }
    }

    if (equal) return true;

    // Build a lookup so not O[N^2] to do match

    std::map<std::string, Column*> columnLookup;
    for (Column* col : *this) {
        columnLookup.emplace(col->name(), col);
    }

    for (Column* col : other) {
        auto it = columnLookup.find(col->name());
        if (it == columnLookup.end()) return false;
        if (*it->second != *col) return false;
    }

    return true;
}

void MetaData::resetStats()
{
    //LOG_DEBUG_LIB(LibOdc) << "MetaData::resetStats" << std::endl;
    for (size_t i = 0; i < size(); i++) {
        (*this)[i]->resetStats();
    }
    rowsNumber_ = 0;
}

void MetaData::print(std::ostream& s) const
{
    for (size_t i = 0; i < size(); i++) {
        s << i << ". " << *at(i) << std::endl;
    }
}

MetaData& MetaData::addColumn(const std::string& name, const std::string& type)
{
    return addColumnPrivate<SameByteOrder>(name, type);
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
    return addBitfieldPrivate<SameByteOrder>(name, bf);
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace core
} // namespace odc

