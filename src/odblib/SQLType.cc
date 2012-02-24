#include "SQLType.h"
#include "Exceptions.h"
#include <set>

namespace odb {
namespace sql {
namespace type {

// Need a mutex

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
		{
			Log::debug() << "DynamicallyCreatedTypesDestroyer::~DynamicallyCreatedTypesDestroyer: " << *it << endl;
			delete *it;
		}
		delete dynamicallyCreatedTypes_;
		dynamicallyCreatedTypes_ = 0;
	}
}

DynamicallyCreatedTypesDestroyer destroyer_;


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
	return (map_->find(name) != map_->end());
}

const SQLType& SQLType::lookup(const string& name)
{
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
	ASSERT(map_);
	ASSERT(SQLType::exists(name));

	(*map_)[alias] = (*map_)[name];
}

void SQLType::print(ostream& s) const
{
	s << name_;
}

//void SQLType::output(ostream& s,double x) const { s << x; }

const SQLType* SQLType::subType(const string&) const
{
	return this;
}

} // namespace type
} // namespace sql
} // namespace odb

