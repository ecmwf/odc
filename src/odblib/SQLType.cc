/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <set>

#include "eclib/Exceptions.h"

#include "odblib/SQLType.h"
#include "odblib/SQLInt.h"
#include "odblib/SQLReal.h"
#include "odblib/SQLDouble.h"
#include "odblib/SQLString.h"

namespace odb {
namespace sql {
namespace type {

static map<string,SQLType*>* map_ = 0;

static set<SQLType*>* dynamicallyCreatedTypes_ = 0;

SQLType* DynamicallyCreatedTypesDestroyer::registerType(SQLType* t)
{
	Log::debug() << "DynamicallyCreatedTypesDestroyer::registerType: " << *t << endl;

	if (! dynamicallyCreatedTypes_) dynamicallyCreatedTypes_ = new set<SQLType*>();

	ASSERT(dynamicallyCreatedTypes_->find(t) == dynamicallyCreatedTypes_->end());
	dynamicallyCreatedTypes_->insert(t);
	return t;
}

DynamicallyCreatedTypesDestroyer::~DynamicallyCreatedTypesDestroyer ()
{
	if (dynamicallyCreatedTypes_)
	{
		for (set<SQLType*>::iterator it = dynamicallyCreatedTypes_->begin(); it != dynamicallyCreatedTypes_->end(); ++it)
			delete *it;
		delete dynamicallyCreatedTypes_;
		dynamicallyCreatedTypes_ = 0;
	}
}

DynamicallyCreatedTypesDestroyer destroyer_; // TODO: make it a proper singleton

size_t SQLType::width() const { return 10; }
SQLType::manipulator SQLType::format() const { return &std::right; }

SQLType::SQLType(const string& name):
	name_(name)
{
	if(!map_) map_ = new map<string,SQLType*>;

	ASSERT(map_->find(name) == map_->end());

	(*map_)[name_] = this;
}

SQLType::SQLType(const string& name, const string& shortName):
	name_(name)
{
	//cout << "********** SQLType::SQLType(name=" << name << ", shortName=" << shortName << ")" << endl;

	if(!map_) map_ = new map<string,SQLType*>;

	ASSERT(map_->find(name) == map_->end());

	(*map_)[shortName] = (*map_)[name_] = this;
}

SQLType::~SQLType()
{
	map_->erase(name_);
	if (map_->empty())
	{
		delete map_;
		map_ = 0;
	}
}

bool SQLType::exists(const string& name)
{
	registerStaticTypes();

	return (map_->find(name) != map_->end());
}

const SQLType& SQLType::lookup(const string& name)
{
	registerStaticTypes();

	map<string,SQLType*>::iterator j = map_->find(name);
	if(j == map_->end())
	{
#if 0
		cout << " ====== THROW!!! =========== SeriousBug(name + \": type not defined\"); (name = '" << name << "')" << endl;
		static int help = 0;
		if (help == 0)
		{
			help = 1;
			cout << "Known types:" << endl;
			for (map<string,SQLType*>::iterator i = map_->begin(); i != map_->end(); i++)
			{
				string k = i->first;
				SQLType* typ = i->second;
				cout << "  >>> " << k << ": " << typ->name() << " (" << typeid(*typ).name() << ")" << endl;
			}
		}
#endif
		throw SeriousBug(name + ": type not defined");
	}
	return *(*j).second;
}

void SQLType::createAlias(const string& name, const string& alias)
{
	registerStaticTypes();

	ASSERT(map_);
	ASSERT(SQLType::exists(name));

	(*map_)[alias] = (*map_)[name];
}

void SQLType::print(ostream& s) const { s << name_; }

//void SQLType::output(ostream& s,double x) const { s << x; }

const SQLType* SQLType::subType(const string&) const { return this; }

void SQLType::registerStaticTypes()
{
	static SQLInt integer("integer");

	/*
	static SQLInt_YYYYMMDD yyyymmdd;
	static SQLInt_HHMMSS   hhmmss;

	static SQLInt pk1int("pk1int");
	static SQLInt pk9int("pk9int");
	static SQLInt linkoffset_t("linkoffset_t");
	static SQLInt linklen_t("linklen_t");
	*/
	static SQLReal real("real");
	static SQLString type("string");
	static SQLDouble double_("double");
}

} // namespace type
} // namespace sql
} // namespace odb

