/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

//#include <set>

//#include "eckit/exception/Exceptions.h"

#include "odblib/SQLType.h"
//#include "odblib/SQLInt.h"
//#include "odblib/SQLReal.h"
//#include "odblib/SQLDouble.h"
//#include "odblib/SQLString.h"

using namespace eckit;

namespace odb {
namespace sql {
namespace type {

static std::map<std::string,SQLType*>* map_ = 0;

static std::set<SQLType*>* dynamicallyCreatedTypes_ = 0;

SQLType* DynamicallyCreatedTypesDestroyer::registerType(SQLType* t)
{
	Log::debug() << "DynamicallyCreatedTypesDestroyer::registerType: " << *t << std::endl;

	if (! dynamicallyCreatedTypes_) dynamicallyCreatedTypes_ = new std::set<SQLType*>();

	ASSERT(dynamicallyCreatedTypes_->find(t) == dynamicallyCreatedTypes_->end());
	dynamicallyCreatedTypes_->insert(t);
	return t;
}

DynamicallyCreatedTypesDestroyer::~DynamicallyCreatedTypesDestroyer ()
{
	if (dynamicallyCreatedTypes_)
	{
		for (std::set<SQLType*>::iterator it = dynamicallyCreatedTypes_->begin(); it != dynamicallyCreatedTypes_->end(); ++it)
			delete *it;
		delete dynamicallyCreatedTypes_;
		dynamicallyCreatedTypes_ = 0;
	}
}

DynamicallyCreatedTypesDestroyer destroyer_; // TODO: make it a proper singleton

size_t SQLType::width() const { return 14; }
SQLType::manipulator SQLType::format() const { return &std::right; }

SQLType::SQLType(const std::string& name):
	name_(name)
{
	if(!map_) map_ = new std::map<std::string,SQLType*>;

	ASSERT(map_->find(name) == map_->end());

	(*map_)[name_] = this;
}

SQLType::SQLType(const std::string& name, const std::string& shortName):
	name_(name)
{
	//cout << "********** SQLType::SQLType(name=" << name << ", shortName=" << shortName << ")" << std::endl;

	if(!map_) map_ = new std::map<std::string,SQLType*>;

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

bool SQLType::exists(const std::string& name)
{
	registerStaticTypes();

	return (map_->find(name) != map_->end());
}

const SQLType& SQLType::lookup(const std::string& name)
{
	registerStaticTypes();

	std::map<std::string,SQLType*>::iterator j = map_->find(name);
	if(j == map_->end())
	{
#if 0
		std::cout << " ====== THROW!!! =========== SeriousBug(name + \": type not defined\"); (name = '" << name << "')" << std::endl;
		static int help = 0;
		if (help == 0)
		{
			help = 1;
			std::cout << "Known types:" << std::endl;
			for (std::map<std::string,SQLType*>::iterator i = map_->begin(); i != map_->end(); i++)
			{
				std::string k = i->first;
				SQLType* typ = i->second;
				std::cout << "  >>> " << k << ": " << typ->name() << " (" << typeid(*typ).name() << ")" << std::endl;
			}
		}
#endif
		throw eckit::SeriousBug(name + ": type not defined");
	}
	return *(*j).second;
}

void SQLType::createAlias(const std::string& name, const std::string& alias)
{
	registerStaticTypes();

	ASSERT(map_);
	ASSERT(SQLType::exists(name));

	(*map_)[alias] = (*map_)[name];
}

void SQLType::print(std::ostream& s) const { s << name_; }

//void SQLType::output(std::ostream& s,double x) const { s << x; }

const SQLType* SQLType::subType(const std::string&) const { return this; }

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
	static SQLString type("std::string");
	static SQLDouble double_("double");
}

} // namespace type
} // namespace sql
} // namespace odb

