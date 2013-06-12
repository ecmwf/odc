/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <iostream>
#include <map>

using namespace std;

#include "eckit/io/DataHandle.h"
#include "eckit/exception/Exceptions.h"

#include "odblib/odb_api.h"
#include "odblib/DataStream.h"

#include "odblib/HashTable.h"
#include "odblib/Codec.h"
#include "odblib/Column.h"
#include "odblib/SQLBitfield.h"
#include "odblib/SQLSession.h"
#include "odblib/SQLDatabase.h"
#include "odblib/SchemaAnalyzer.h"

using namespace eckit;

namespace odb {

Column::Column(MetaData &owner)
: owner_(owner),
  name_(),
  type_(IGNORE/*?*/),
  coder_(0),
  bitfieldDef_()
{}

Column::Column(const Column& o)
: owner_(o.owner_),
  name_(o.name_),
  type_(o.type_),
  coder_( 0),
  bitfieldDef_(o.bitfieldDef_)
{
	*this = o;
}

Column::~Column()
{
	//cerr << "Column::~Column():@" << this  << ", name= " << name_ << endl << endl;
	delete coder_; 
}

Column& Column::operator=(const Column& other)
{
	name(other.name()); 

	type<DataStream<SameByteOrder, DataHandle> >(other.type(), false);
	if (type_ == BITFIELD)
		bitfieldDef(other.bitfieldDef());

	//delete coder_;
	//coder_ = (other.coder_)->clone();
	//hasMissing(other.hasMissing());
	missingValue(other.missingValue());
	return *this;
}

const char *Column::columnTypeName(ColumnType type)
{
	switch(type) {
		case IGNORE:   return "IGNORE";
		case INTEGER:  return "INTEGER";
		case REAL:     return "REAL";
		case DOUBLE:   return "DOUBLE";
		case STRING:   return "STRING";
		case BITFIELD: return "BITFIELD";
		default:       return "UNKNOWN_TYPE";
	}
}

ColumnType Column::type(const string& t)
{
	if (t == "IGNORE") return IGNORE;
	if (t == "INTEGER") return INTEGER;
	if (t == "REAL") return REAL;
	if (t == "DOUBLE") return DOUBLE;
	if (t == "STRING") return STRING;
	if (t == "BITFIELD") return BITFIELD;

	Log::error() << "Unknown type: '" << t << "'" << endl;
	ASSERT(0 && "Unknown type");
	return IGNORE;
}

bool Column::isConstant()
{
	// FIXME
	return coder().name() == "constant"
		|| coder().name() == "constant_string";
}

//Column::Column(DataHandle *dataHandle) : dataHandle_(dataHandle), name_(), type_(IGNORE/*?*/), coder_(0) {}


/// return true if names and types are the same; do not compare codecs.
bool Column::operator==(const Column& other) const
{
	if (name() == other.name() && type() == other.type())
		return true;
	return false;
}

void Column::print(ostream& s) const
{
	s << "name: " << name_ << ", ";
	s << "type: " << columnTypeName(odb::ColumnType(type_));

	if (type_ == BITFIELD)
	{
		FieldNames names = bitfieldDef_.first;
		Sizes sizes = bitfieldDef_.second;
		ASSERT(names.size() == sizes.size());
		s << " [";
		for (size_t i = 0; i < names.size(); ++i)
			s << names[i] << ":" << sizes[i]
				<< (i != names.size() - 1 ? ";" : "");
		s << "] ";
	}
			
	s << ", ";

	s << "codec: ";
	if (coder_) s << *coder_;
	else s << "NONE";
}

} // namespace odb 

