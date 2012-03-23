/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eclib/Exceptions.h"
#include "eclib/Log.h"
#include "eclib/Timer.h"
#include "eclib/Tokenizer.h"

#include "odblib/SQLAST.h"
#include "odblib/SQLBitfield.h"
#include "odblib/SQLBitfield.h"
#include "odblib/SQLDatabase.h"
#include "odblib/SQLStatement.h"
#include "odblib/SQLTable.h"
#include "odblib/SQLType.h"
#include "odblib/SchemaAnalyzer.h"


namespace odb {
namespace sql {

SQLDatabase::SQLDatabase(const PathName& path,const string& name):
	path_(path),
	name_(name)
{} 

SQLDatabase::SQLDatabase(const string& name):
	path_("."), 
	name_(name)
{} 

SQLDatabase::~SQLDatabase()
{
	close();
	for (Variables::iterator it = variables_.begin(); it != variables_.end(); ++it)
		delete it->second;
	variables_.clear();
}

void SQLDatabase::open()
{
	Log::info() << "Opening " << path_ << " as " << name_ << endl;
#if 0
	loadDD();
	loadIOMAP();
	loadFLAGS();
#endif
}

void SQLDatabase::close()
{
	for(map<string,SQLTable*>::iterator j = tablesByName_.begin();
		j != tablesByName_.end(); ++j)
	{
		SQLTable* table = (*j).second;
		delete table;
	}

	tablesByName_.clear();
}

#if 0
void SQLDatabase::loadFLAGS()
{
	string name   = path_.baseName(false);
	PathName path = path_ + "/" + name + ".flags";
	ifstream in(path.c_str());
	if(!in) throw CantOpenFile(path);

	Tokenizer parse("=(),");
	char line[1024];
	while(in.getline(line,sizeof(line)-1))
	{
		if(line[0] == '-' && line[1] == 'A')
		{
			string s(line+2);
			vector<string> v;
			parse(s,v);

			if(tablesByName_.find(v[0]) == tablesByName_.end())
			{
				Log::warning() << path << ": Cannot find master table '" << v[0] << "'" << endl;
			}
			else
			{
				SQLTable* master = tablesByName_[v[0]];

				for(int i = 1 ; i < v.size(); i++)
				{
					if(tablesByName_.find(v[i]) == tablesByName_.end())
						Log::warning() << path << ": Cannot find slave table '" 
							<< v[i] << "' of '" << master->name() 
							<< "'" << endl;
					else					
						tablesByName_[v[i]]->master(master);
				}
			}
			
		}
	}
}

void SQLDatabase::loadDD()
{
	string name   = path_.baseName(false);
	PathName path = path_ + "/" + name + ".dd";

	ifstream in(path.c_str());
	if(!in) throw CantOpenFile(path);

	Tokenizer parse(":@");
	Tokenizer braket("()");

	string junk;
	string s;

	//cout << path << endl;

	in >> junk >> junk >> junk; // version ?
	in >> junk >> junk ; // dates ?
	in >> junk >> junk ; // dates ?
	in >> junk >> junk ; // dates ?

	int no_pools = 0,no_tables = 0;

	in >> no_pools;
	in >> no_tables;

	//cout << "no_pools " << no_pools << endl;
	//cout << "no_tbales " << no_tables << endl;

	for(int i = 0; i < no_tables; i++)
	{
		int id; string name;
		int n;
		in >> id >> name >> n;
		while(n-->0) in >> junk;
	}

	map<string,set<string> > links;

	for(int i = 0; i < no_tables; i++)
	{
		string name; int no_cols;
		in >> name >> no_cols; name.erase(0,1);
		ASSERT(tablesByName_.find(name) == tablesByName_.end());

		SQLTable* table = new SQLTable(*this,path_,name,no_cols,i);
		tablesByName_[name] = table;


		for(int j = 0; j < no_cols; j++)
		{
			string type; int no_fields;
			in >> type >> no_fields;

			//cout << type << " " << no_fields << endl;

			vector<string> v;
			parse(type,v);

			//cout << name << " " << v[0] << " " << v[1] << endl;
			vector<string> b;
			braket(v[1],b);

			if(b.size() > 1)
			{
				if(b[0] == "LINKOFFSET") { v[1] = b[1] + "." + "offset"; }
				if(b[0] == "LINKLEN")    { v[1] = b[1] + "." + "length"; }

				links[name].insert(b[1]);
			}

			vector<string> bitmap;

			if(no_fields)
			{
				vector<string> fields;
				vector<int>    sizes;
		
				for(int k = 0; k < no_fields; k++)
				{
					string name; int size;
					in >> name >> size;

					fields.push_back(name);
					sizes.push_back(size);
				//cout <<  name << " " << size << endl;
					// table->addBitColumn(...)
					bitmap.push_back(name);
				}
				v[0] = SQLBitfield::make(v[0],fields,sizes);
			}

			table->addColumn(v[1], j,SQLType::lookup(v[0]), bitmap);
		}
	}

	int no_variables;
	in >> no_variables;

	for(int i = 0; i < no_variables; i++)
	{
		string name; double value;
		in >> name >> value;
		variables_[name] = value;

		//Log::info() << "Variable " << name << " = " << value << endl;
	}
	setLinks(links);
}
#endif

void SQLDatabase::setLinks(const Links& links)
{
	for(Links::const_iterator j = links.begin(); j != links.end() ; ++j)
	{
		const string&      from = (*j).first;
		const set<string>& to   = (*j).second;

		ASSERT(tablesByName_.find(from) != tablesByName_.end());
		SQLTable *f = tablesByName_[from];

		for(set<string>::const_iterator k = to.begin(); k != to.end() ; ++k)
		{
			ASSERT(tablesByName_.find(*k) != tablesByName_.end());
			SQLTable* t = tablesByName_[*k];

			f->addLinkTo(t);
			t->addLinkFrom(f);
		}
	}
}

#if 0
void SQLDatabase::loadIOMAP()
{
	string name   = path_.baseName(false);
	PathName path = path_ + "/" + name + ".iomap";

	ifstream in(path.c_str());
	if(!in) throw CantOpenFile(path);

	int junk;
	string s;
	string table;

	int no_tables,no_pools;

	in >> junk;
	in >> s;

	in >> no_tables >> no_pools >> junk;
	in >> junk >> junk >> junk;

	for(;;)
	{
		string name; int id, n_cols;

		in >> id >> n_cols >> name; name.erase(0,1);
		if(id == -1) break;

		ASSERT(tablesByName_.find(name) != tablesByName_.end());
		tablesByName_[name]->loadIOMAP(in);

	}
}
#endif

SQLTable* SQLDatabase::table(const string& name)
{
	map<string,SQLTable*>::iterator j = tablesByName_.find(name);
	ASSERT(j != tablesByName_.end());
	return (*j).second;
}

void SQLDatabase::setVariable(const string& name, SQLExpression* value) {
	variables_[name] = value;
}

SQLExpression* SQLDatabase::getVariable(const string& name) const
{
	Variables::const_iterator j = variables_.find(name);
	if(j == variables_.end())
		throw UserError("Undefined variable", name);
	return (*j).second;
}

bool SQLDatabase::sameAs(const SQLDatabase& other) const
{
	return path_ == other.path_;
}

} // namespace sql
} // namespace odb
