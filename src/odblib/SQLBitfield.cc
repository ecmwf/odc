#include "SQLBitfield.h"
#include "Translator.h"
#include "Exceptions.h"
#include "Tokenizer.h"
#include "SQLBit.h"
#include "Decoder.h"
#include "Types.h"
#include "SQLOutput.h"

namespace odb {
namespace sql {
namespace type {

SQLBitfield::SQLBitfield(const string& name, const FieldNames& fields, const Sizes& sizes, const string& ddlName)
: SQLType(name, ddlName),
  bitfieldDef_(make_pair(fields, sizes))
{
	Log::debug() << "SQLBitfield::SQLBitfield@" << this << "(name=" << name << ", fields=" << fields
		<< ", sizes=" << sizes << ", ddlName=" << ddlName << ")" << endl;

	int shift = 0;
	for(size_t i = 0; i < fields.size(); i++ )
	{
		shift_[fields[i]] = shift;
		mask_[fields[i]]  = Decoder::makeMask(sizes[i]) << shift;
		//cout << fields[i] << " " << sizes_[i] << " mask=" << hex << mask << dec << " shift=" << shift << endl;
		shift += sizes[i];
	}
}

SQLBitfield::~SQLBitfield()
{
	Log::debug() << "SQLBitfield::~SQLBitfield@" << this << endl;
}

unsigned long SQLBitfield::mask(const string& n) const
{
	map<string,unsigned long>::const_iterator j = mask_.find(n);
	
	if(j == mask_.end())
		throw UserError("SQLBitfield no field", n);

	return (*j).second;
}

unsigned long SQLBitfield::shift(const string& n) const
{
	map<string,unsigned long>::const_iterator j = shift_.find(n);

	if(j == shift_.end())
		throw UserError("SQLBitfield no field", n);

	return (*j).second;
}

string SQLBitfield::make(const string& name, const FieldNames& fields, const Sizes& sizes, const char *ddlName)
{

	string s = name;
	for(size_t i = 0; i < fields.size(); i++)
	{
		s += ":";
		s += fields[i];
		s += ":";
		s += Translator<int,string>()(sizes[i]);
	}

	if(!exists(s))
		DynamicallyCreatedTypesDestroyer::registerType(new SQLBitfield(s, fields, sizes, ddlName));
	else
		SQLType::createAlias(s, ddlName);

	return s;
}

size_t SQLBitfield::size() const
{
	return sizeof(long);
}

void SQLBitfield::output(SQLOutput& o, double d, bool missing) const
{
	o.outputBitfield(d, missing);
}

const SQLType* SQLBitfield::subType(const string& name) const
{
	vector<string> v;
	Tokenizer(".@")(name,v);

	if(v.size() == 1) return this;
	if(v.size() == 2 && name.find('@') != string::npos) return this;

	ASSERT(v.size() == 3 || v.size() == 2); // name was e.g: "status.active@body" or "status.active"

	string field = v[1];
	string full  = name; //this->name() + "." + field;

	if(exists(full))
		return &lookup(full);

	return DynamicallyCreatedTypesDestroyer::registerType(new SQLBit(full, mask(field), shift(field)));
}

//static SQLBitfield bitfield("SQLSQLBitColumn");
} // namespace type 
} // namespace sql 
} // namespace odb 

