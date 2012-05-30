/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */
#ifndef ToolFactory_H
#define ToolFactory_H

#include "odblib/Tool.h"
#include "odblib/TestCase.h"

namespace odb {
namespace tool {

class AbstractToolFactory {
public:
	static Tool* createTool(const string& name, int argc, char **argv);

	static void printToolHelp(const string&, ostream &);
	static void printToolUsage(const string& name, ostream &);
	static void printToolsHelp(ostream &);

	static void listTools(ostream&);

	static odb::tool::test::TestCases *testCases(const vector<string> & = matchAll);

	virtual Tool* create(int argc, char **argv) = 0;

	virtual void help(ostream &) = 0;
	virtual void usage(const string&, ostream &) = 0;
	virtual bool experimental() = 0;
	
protected:
	AbstractToolFactory(const string& name); 
	virtual ~AbstractToolFactory ();

private:
	static AbstractToolFactory& findTool(const string &name);
	static map<string, AbstractToolFactory *> *toolFactories;
	static const vector<string> matchAll;
};


template <class T>
class ToolFactory : public AbstractToolFactory {
public:
	ToolFactory (const string& name) : AbstractToolFactory(name) {}

	Tool* create(int argc, char **argv) { return new T(argc, argv); }

	void help(ostream &o) { T::help(o); }
	void usage(const string &name, ostream &o) { T::usage(name, o); }
	bool experimental() { return ExperimentalTool<T>::experimental; }
};

} // namespace tool 
} // namespace odb 

#endif

