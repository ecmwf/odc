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
#include "eckit/log/Log.h"
#include "eckit/parser/Translator.h"
#include "eckit/types/Types.h"
#include "eckit/utils/Tokenizer.h"
#include "odblib/Decoder.h"
#include "odblib/SQLBitfield.h"
#include "odblib/SQLBit.h"
#include "odblib/SQLOutput.h"
#include "odblib/Types.h"

using namespace eckit;

namespace odb {
namespace sql {
namespace type {

SQLBitfield::SQLBitfield(const std::string& name, const FieldNames& fields, const Sizes& sizes, const std::string& ddlName)
: SQLType(name, ddlName),
  bitfieldDef_(make_pair(fields, sizes))
{
	Log::debug() << "SQLBitfield::SQLBitfield@" << this << "(name=" << name << ", fields=" << fields
		<< ", sizes=" << sizes << ", ddlName=" << ddlName << ")" << std::endl;

	int shift = 0;
	for(size_t i = 0; i < fields.size(); i++ )
	{
		shift_[fields[i]] = shift;
		mask_[fields[i]]  = Decoder::makeMask(sizes[i]) << shift;
		//cout << fields[i] << " " << sizes_[i] << " mask=" << hex << mask << dec << " shift=" << shift << std::endl;
		shift += sizes[i];
	}
	width_ = shift;
}

SQLBitfield::~SQLBitfield() {}

unsigned long SQLBitfield::mask(const std::string& n) const
{
	std::map<std::string,unsigned long>::const_iterator j = mask_.find(n);
	
	if(j == mask_.end())
		throw eckit::UserError("SQLBitfield no field", n);

	return (*j).second;
}

unsigned long SQLBitfield::shift(const std::string& n) const
{
	std::map<std::string,unsigned long>::const_iterator j = shift_.find(n);

	if(j == shift_.end())
		throw eckit::UserError("SQLBitfield no field", n);

	return (*j).second;
}

std::string SQLBitfield::make(const std::string& name, const FieldNames& fields, const Sizes& sizes, const char *ddlName)
{

    std::stringstream s;
	s << name << "[";
	for(size_t i = 0; i < fields.size(); ++i)
		s << fields[i] << ":" << Translator<int,std::string>()(sizes[i])
		  << ((i + 1 != fields.size()) ? ";" : "");
	s << "]";
	std::string typeName = s.str();

	if(! exists(typeName))
		DynamicallyCreatedTypesDestroyer::registerType(new SQLBitfield(typeName, fields, sizes, ddlName));
	else
		SQLType::createAlias(typeName, ddlName);

	return typeName;
}

size_t SQLBitfield::width() const { return width_; }

size_t SQLBitfield::size() const
{
	return sizeof(long);
}

void SQLBitfield::output(SQLOutput& o, double d, bool missing) const
{
	o.outputBitfield(d, missing);
}

const SQLType* SQLBitfield::subType(const std::string& name) const
{
	std::vector<std::string> v;
	Tokenizer(".@")(name,v);

	if(v.size() == 1) return this;
	if(v.size() == 2 && name.find('@') != std::string::npos) return this;

	ASSERT(v.size() == 3 || v.size() == 2); // name was e.g: "status.active@body" or "status.active"

	std::string field = v[1];
	std::string full  = name; //this->name() + "." + field;

	if(exists(full))
		return &lookup(full);

	return DynamicallyCreatedTypesDestroyer::registerType(new SQLBit(full, mask(field), shift(field)));
}

} // namespace type 
} // namespace sql 
} // namespace odb 

