/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef SQLServerTool_H
#define SQLServerTool_H

namespace odb {
namespace tool {

class SQLServerTool : public Tool {

public:
	SQLServerTool(int argc,char **argv);
	~SQLServerTool();

	static void help(ostream &o)
	{ o << "Starts a socket server serving SQL requests"; }

	static void usage(const string& name, ostream &o)
	{ o << name << " -port <port-number>"; }
	
	virtual void run();
};

template <> struct ExperimentalTool<SQLServerTool> { enum { experimental = true }; };

} // namespace tool 
} // namespace odb 

#endif

